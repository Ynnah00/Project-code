const express = require('express');
const sqlite3 = require('sqlite3').verbose();
const bodyParser = require('body-parser');
const cors = require('cors');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

// Middleware
app.use(cors());
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));
app.use(express.static('public'));

// Initialize SQLite Database
const db = new sqlite3.Database('./database.db', (err) => {
    if (err) {
        console.error('Error opening database:', err);
    } else {
        console.log('Connected to SQLite database');
        initializeDatabase();
    }
});

// Initialize database schema
function initializeDatabase() {
    db.run(`
        CREATE TABLE IF NOT EXISTS items (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            type TEXT NOT NULL,
            name TEXT NOT NULL,
            category TEXT NOT NULL,
            place TEXT NOT NULL,
            date TEXT NOT NULL,
            contact TEXT NOT NULL,
            description TEXT,
            resolved INTEGER DEFAULT 0,
            created_at DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    `);
}

// Levenshtein Distance Algorithm (for fuzzy matching)
function levenshteinDistance(a, b) {
    const matrix = [];

    for (let i = 0; i <= b.length; i++) {
        matrix[i] = [i];
    }

    for (let j = 0; j <= a.length; j++) {
        matrix[0][j] = j;
    }

    for (let i = 1; i <= b.length; i++) {
        for (let j = 1; j <= a.length; j++) {
            if (b.charAt(i - 1) === a.charAt(j - 1)) {
                matrix[i][j] = matrix[i - 1][j - 1];
            } else {
                matrix[i][j] = Math.min(
                    matrix[i - 1][j - 1] + 1,
                    matrix[i][j - 1] + 1,
                    matrix[i - 1][j] + 1
                );
            }
        }
    }

    return matrix[b.length][a.length];
}

// Fuzzy Match Function
function fuzzyMatch(str1, str2) {
    const lower1 = str1.toLowerCase();
    const lower2 = str2.toLowerCase();
    const distance = levenshteinDistance(lower1, lower2);
    const maxLen = Math.max(lower1.length, lower2.length);
    return distance <= Math.floor(maxLen / 3);
}

// API Routes

// Add Item
app.post('/api/items', (req, res) => {
    const { type, name, category, place, date, contact, description } = req.body;

    db.run(
        `INSERT INTO items (type, name, category, place, date, contact, description, resolved)
         VALUES (?, ?, ?, ?, ?, ?, ?, 0)`,
        [type, name, category, place, date, contact, description || ''],
        function(err) {
            if (err) {
                return res.status(500).json({ error: err.message });
            }

            const newItem = {
                id: this.lastID,
                type,
                name,
                category,
                place,
                date,
                contact,
                description,
                resolved: 0
            };

            // Check for matches if it's a found item
            if (type === 'Found') {
                checkMatches(newItem, (matches) => {
                    res.json({ item: newItem, matches });
                });
            } else {
                res.json({ item: newItem, matches: [] });
            }
        }
    );
});

// Check Matches Function
function checkMatches(newFound, callback) {
    db.all(`SELECT * FROM items WHERE type = 'Lost' AND resolved = 0`, [], (err, rows) => {
        if (err) {
            callback([]);
            return;
        }

        const matches = rows.filter((lost) => {
            const nameMatch = fuzzyMatch(newFound.name, lost.name);
            const placeMatch = fuzzyMatch(newFound.place, lost.place);
            const categoryMatch = newFound.category === lost.category;
            return nameMatch || placeMatch || categoryMatch;
        });

        callback(matches);
    });
}

// Get All Items
app.get('/api/items', (req, res) => {
    const type = req.query.type; // 'Lost', 'Found', or undefined for all
    const filter = type ? `WHERE type = '${type}'` : '';

    db.all(`SELECT * FROM items ${filter} ORDER BY created_at DESC`, [], (err, rows) => {
        if (err) {
            return res.status(500).json({ error: err.message });
        }
        res.json(rows);
    });
});

// Get Active Items (unresolved)
app.get('/api/items/active', (req, res) => {
    const type = req.query.type;
    const filter = type ? `WHERE type = '${type}' AND resolved = 0` : `WHERE resolved = 0`;

    db.all(`SELECT * FROM items ${filter} ORDER BY created_at DESC`, [], (err, rows) => {
        if (err) {
            return res.status(500).json({ error: err.message });
        }
        res.json(rows);
    });
});

// Get Item by ID
app.get('/api/items/:id', (req, res) => {
    const { id } = req.params;

    db.get(`SELECT * FROM items WHERE id = ?`, [id], (err, row) => {
        if (err) {
            return res.status(500).json({ error: err.message });
        }
        if (!row) {
            return res.status(404).json({ error: 'Item not found' });
        }
        res.json(row);
    });
});

// Update Item
app.put('/api/items/:id', (req, res) => {
    const { id } = req.params;
    const { type, name, category, place, date, contact, description, resolved } = req.body;

    db.run(
        `UPDATE items SET type=?, name=?, category=?, place=?, date=?, contact=?, description=?, resolved=? WHERE id=?`,
        [type, name, category, place, date, contact, description, resolved, id],
        function(err) {
            if (err) {
                return res.status(500).json({ error: err.message });
            }
            if (this.changes === 0) {
                return res.status(404).json({ error: 'Item not found' });
            }
            res.json({ message: 'Item updated successfully' });
        }
    );
});

// Mark Item as Resolved
app.patch('/api/items/:id/resolve', (req, res) => {
    const { id } = req.params;

    db.run(
        `UPDATE items SET resolved = 1 WHERE id = ?`,
        [id],
        function(err) {
            if (err) {
                return res.status(500).json({ error: err.message });
            }
            if (this.changes === 0) {
                return res.status(404).json({ error: 'Item not found' });
            }
            res.json({ message: 'Item marked as resolved' });
        }
    );
});

// Delete Item
app.delete('/api/items/:id', (req, res) => {
    const { id } = req.params;

    db.run(`DELETE FROM items WHERE id = ?`, [id], function(err) {
        if (err) {
            return res.status(500).json({ error: err.message });
        }
        if (this.changes === 0) {
            return res.status(404).json({ error: 'Item not found' });
        }
        res.json({ message: 'Item deleted successfully' });
    });
});

// Search Items
app.get('/api/search', (req, res) => {
    const { keyword } = req.query;

    if (!keyword) {
        return res.status(400).json({ error: 'Keyword is required' });
    }

    const searchTerm = `%${keyword}%`;
    db.all(
        `SELECT * FROM items WHERE name LIKE ? OR place LIKE ? OR category LIKE ? OR description LIKE ? ORDER BY created_at DESC`,
        [searchTerm, searchTerm, searchTerm, searchTerm],
        (err, rows) => {
            if (err) {
                return res.status(500).json({ error: err.message });
            }
            res.json(rows);
        }
    );
});

// Get Statistics
app.get('/api/statistics', (req, res) => {
    db.all(
        `SELECT 
            type,
            COUNT(*) as total,
            SUM(CASE WHEN resolved = 0 THEN 1 ELSE 0 END) as active,
            SUM(CASE WHEN resolved = 1 THEN 1 ELSE 0 END) as resolved
         FROM items GROUP BY type`,
        [],
        (err, rows) => {
            if (err) {
                return res.status(500).json({ error: err.message });
            }
            res.json(rows);
        }
    );
});

// Start Server
app.listen(PORT, () => {
    console.log(`Lost and Found System running on http://localhost:${PORT}`);
});

// Graceful shutdown
process.on('SIGINT', () => {
    db.close((err) => {
        if (err) console.error(err.message);
        console.log('Database connection closed');
        process.exit(0);
    });
});
