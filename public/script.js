// API Base URL
const API_URL = 'http://localhost:3000/api';

// DOM Elements
const navBtns = document.querySelectorAll('.nav-btn');
const sections = document.querySelectorAll('.section');
const addItemForm = document.getElementById('add-item-form');
const modal = document.getElementById('modal');
const closeBtn = document.querySelector('.close');

// Navigation
navBtns.forEach(btn => {
    btn.addEventListener('click', () => {
        const sectionId = btn.dataset.section;
        navigateTo(sectionId);
    });
});

function navigateTo(sectionId) {
    // Update active nav button
    navBtns.forEach(btn => btn.classList.remove('active'));
    document.querySelector(`[data-section="${sectionId}"]`).classList.add('active');

    // Update active section
    sections.forEach(section => section.classList.remove('active'));
    document.getElementById(sectionId).classList.add('active');

    // Load section data
    if (sectionId === 'lost-items') {
        loadItems('Lost');
    } else if (sectionId === 'found-items') {
        loadItems('Found');
    } else if (sectionId === 'statistics') {
        loadStatistics();
    } else if (sectionId === 'home') {
        loadHomeStatistics();
    }
}

// Add Item Form
addItemForm.addEventListener('submit', async (e) => {
    e.preventDefault();

    const formData = {
        type: document.getElementById('item-type').value,
        name: document.getElementById('item-name').value,
        category: document.getElementById('item-category').value,
        place: document.getElementById('item-place').value,
        date: document.getElementById('item-date').value,
        contact: document.getElementById('item-contact').value,
        description: document.getElementById('item-description').value
    };

    try {
        const response = await fetch(`${API_URL}/items`, {
            method: 'POST',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify(formData)
        });

        const data = await response.json();

        if (response.ok) {
            alert('✓ Item reported successfully!');
            
            // Show matches if found
            if (data.matches && data.matches.length > 0) {
                displayMatches(data.matches);
            }

            // Reset form
            addItemForm.reset();
            document.getElementById('item-type').value = '';
            document.getElementById('item-category').value = '';
        } else {
            alert('Error: ' + data.error);
        }
    } catch (error) {
        console.error('Error:', error);
        alert('Error submitting item');
    }
});

// Display Matches
function displayMatches(matches) {
    const container = document.getElementById('matches-container');
    const list = document.getElementById('matches-list');

    list.innerHTML = matches.map(match => `
        <div class="match-item">
            <strong>${match.name}</strong> (${match.category})<br>
            Location: ${match.place}<br>
            Contact: ${match.contact}<br>
            <small>Date: ${match.date}</small>
        </div>
    `).join('');

    container.classList.remove('hidden');
    container.classList.add('show');
}

// Load Items
async function loadItems(type) {
    try {
        const response = await fetch(`${API_URL}/items?type=${type}`);
        const items = await response.json();

        const listId = type === 'Lost' ? 'lost-items-list' : 'found-items-list';
        const list = document.getElementById(listId);

        if (items.length === 0) {
            list.innerHTML = '<div class="empty-state"><p>No ' + type.toLowerCase() + ' items found.</p></div>';
            return;
        }

        list.innerHTML = items.map(item => createItemCard(item)).join('');

        // Add filter event listeners
        const filterBtns = document.querySelectorAll(`#${listId.replace('-list', '')}-items .filter-btn`);
        filterBtns.forEach(btn => {
            btn.addEventListener('click', async () => {
                filterBtns.forEach(b => b.classList.remove('active'));
                btn.classList.add('active');

                const filter = btn.dataset.filter;
                await filterItems(type, filter, list);
            });
        });
    } catch (error) {
        console.error('Error loading items:', error);
    }
}

// Filter Items
async function filterItems(type, filter, list) {
    try {
        let url = `${API_URL}/items?type=${type}`;
        
        if (filter === 'active') {
            url = `${API_URL}/items/active?type=${type}`;
        }

        const response = await fetch(url);
        const items = await response.json();

        if (items.length === 0) {
            list.innerHTML = '<div class="empty-state"><p>No items found.</p></div>';
            return;
        }

        list.innerHTML = items.map(item => createItemCard(item)).join('');
    } catch (error) {
        console.error('Error filtering items:', error);
    }
}

// Create Item Card
function createItemCard(item) {
    return `
        <div class="item-card ${item.resolved ? 'resolved' : ''}" onclick="viewItemDetails(${item.id})">
            <div class="item-header">
                <span class="item-type ${item.type.toLowerCase()}">${item.type}</span>
                <span class="item-status ${item.resolved ? 'resolved' : ''}">
                    ${item.resolved ? '✓ Resolved' : 'Active'}
                </span>
            </div>
            <div class="item-name">${item.name}</div>
            <div class="item-category">${item.category}</div>
            <div class="item-details">
                <strong>📍 Location:</strong> ${item.place}<br>
                <strong>📅 Date:</strong> ${item.date}<br>
                <strong>👤 Contact:</strong> ${item.contact}
            </div>
            ${item.description ? `<div class="item-details"><strong>📝 Description:</strong> ${item.description}</div>` : ''}
            <div class="item-actions">
                <button class="btn btn-primary" onclick="event.stopPropagation(); editItem(${item.id})">Edit</button>
                <button class="btn btn-success" onclick="event.stopPropagation(); resolveItem(${item.id})">Mark Resolved</button>
                <button class="btn btn-danger" onclick="event.stopPropagation(); deleteItem(${item.id})">Delete</button>
            </div>
        </div>
    `;
}

// View Item Details
function viewItemDetails(id) {
    fetch(`${API_URL}/items/${id}`)
        .then(res => res.json())
        .then(item => {
            const modalBody = document.getElementById('modal-body');
            modalBody.innerHTML = `
                <h2>${item.name}</h2>
                <p><strong>Type:</strong> ${item.type}</p>
                <p><strong>Category:</strong> ${item.category}</p>
                <p><strong>Location:</strong> ${item.place}</p>
                <p><strong>Date:</strong> ${item.date}</p>
                <p><strong>Contact:</strong> ${item.contact}</p>
                <p><strong>Description:</strong> ${item.description || 'No description'}</p>
                <p><strong>Status:</strong> ${item.resolved ? '✓ Resolved' : 'Active'}</p>
            `;
            showModal();
        });
}

// Edit Item
function editItem(id) {
    // Simple edit - could be enhanced with a form modal
    const newName = prompt('Enter new name:');
    if (newName) {
        fetch(`${API_URL}/items/${id}`, {
            method: 'PUT',
            headers: { 'Content-Type': 'application/json' },
            body: JSON.stringify({ name: newName })
        }).then(() => {
            alert('Item updated!');
            location.reload();
        });
    }
}

// Resolve Item
async function resolveItem(id) {
    if (confirm('Mark this item as resolved?')) {
        try {
            const response = await fetch(`${API_URL}/items/${id}/resolve`, { method: 'PATCH' });
            if (response.ok) {
                alert('✓ Item marked as resolved!');
                location.reload();
            }
        } catch (error) {
            console.error('Error:', error);
        }
    }
}

// Delete Item
async function deleteItem(id) {
    if (confirm('Are you sure you want to delete this item?')) {
        try {
            const response = await fetch(`${API_URL}/items/${id}`, { method: 'DELETE' });
            if (response.ok) {
                alert('✓ Item deleted!');
                location.reload();
            }
        } catch (error) {
            console.error('Error:', error);
        }
    }
}

// Perform Search
async function performSearch() {
    const keyword = document.getElementById('search-input').value;

    if (!keyword.trim()) {
        alert('Please enter a search keyword');
        return;
    }

    try {
        const response = await fetch(`${API_URL}/search?keyword=${encodeURIComponent(keyword)}`);
        const results = await response.json();

        const searchResults = document.getElementById('search-results');

        if (results.length === 0) {
            searchResults.innerHTML = '<div class="empty-state"><p>No results found.</p></div>';
            return;
        }

        searchResults.innerHTML = results.map(item => createItemCard(item)).join('');
    } catch (error) {
        console.error('Error:', error);
    }
}

// Load Home Statistics
async function loadHomeStatistics() {
    try {
        const response = await fetch(`${API_URL}/items`);
        const items = await response.json();

        let lostCount = 0, foundCount = 0, activeCount = 0;

        items.forEach(item => {
            if (item.type === 'Lost') lostCount++;
            if (item.type === 'Found') foundCount++;
            if (!item.resolved) activeCount++;
        });

        document.getElementById('lost-count').textContent = lostCount;
        document.getElementById('found-count').textContent = foundCount;
        document.getElementById('active-count').textContent = activeCount;
    } catch (error) {
        console.error('Error:', error);
    }
}

// Load Statistics
async function loadStatistics() {
    try {
        const response = await fetch(`${API_URL}/statistics`);
        const stats = await response.json();

        const statsContent = document.getElementById('statistics-content');

        if (stats.length === 0) {
            statsContent.innerHTML = '<div class="empty-state"><p>No data available.</p></div>';
            return;
        }

        statsContent.innerHTML = stats.map(stat => `
            <div class="stat-box">
                <h3>${stat.type} Items</h3>
                <div class="stat-detail">
                    <span class="stat-label">Total:</span>
                    <span class="stat-value">${stat.total}</span>
                </div>
                <div class="stat-detail">
                    <span class="stat-label">Active:</span>
                    <span class="stat-value">${stat.active}</span>
                </div>
                <div class="stat-detail">
                    <span class="stat-label">Resolved:</span>
                    <span class="stat-value">${stat.resolved}</span>
                </div>
            </div>
        `).join('');
    } catch (error) {
        console.error('Error:', error);
    }
}

// Modal Functions
function showModal() {
    modal.classList.remove('hidden');
    modal.classList.add('show');
}

function closeModal() {
    modal.classList.add('hidden');
    modal.classList.remove('show');
}

closeBtn.addEventListener('click', closeModal);

window.addEventListener('click', (e) => {
    if (e.target === modal) {
        closeModal();
    }
});

// Initialize on load
window.addEventListener('load', () => {
    loadHomeStatistics();
});
