# Lost and Found System 📦

A full-stack web application to help people report and find lost items. Built with Node.js, Express, SQLite, and featuring intelligent fuzzy matching algorithms.

## 🌟 Features

### Core Functionality
- ✅ **Report Lost/Found Items** - Submit detailed information about items
- ✅ **Smart Matching** - Automatic fuzzy matching when reporting found items
- ✅ **Browse Listings** - View all lost and found items with filtering
- ✅ **Search** - Search across name, location, category, and description
- ✅ **Status Tracking** - Mark items as resolved or active
- ✅ **Contact Information** - Direct contact details for item reporters
- ✅ **Statistics Dashboard** - Real-time analytics and system overview

### Technical Features
- ✅ **Levenshtein Distance Algorithm** - Advanced fuzzy matching for item similarity
- ✅ **Persistent Storage** - SQLite database for reliable data persistence
- ✅ **RESTful API** - Complete CRUD operations via clean API endpoints
- ✅ **Responsive Design** - Works seamlessly on desktop, tablet, and mobile
- ✅ **Modern UI** - Beautiful gradient design with smooth animations
- ✅ **Real-time Updates** - Instant feedback on all operations

## 🚀 Quick Start

### Prerequisites
- Node.js (v14 or higher)
- npm (comes with Node.js)

### Installation

1. **Clone the repository:**
```bash
git clone https://github.com/Ynnah00/project-code.git
cd project-code
```

2. **Install dependencies:**
```bash
npm install
```

3. **Start the server:**
```bash
npm start
```

4. **Open in browser:**
```
http://localhost:3000
```

## 📋 Usage

### Adding an Item
1. Click "Add Item" in the navigation
2. Select type (Lost or Found)
3. Fill in details:
   - Item name
   - Category (Electronics, Jewelry, Documents, Clothing, etc.)
   - Location where lost/found
   - Date
   - Your contact information
   - Item description

4. Submit the form
5. If it's a found item, the system will automatically check for matches!

### Browsing Items
- **Lost Items** - View all reported lost items
- **Found Items** - View all reported found items
- **Filter by Status** - Active or Resolved items

### Searching
1. Go to the Search section
2. Enter a keyword (item name, location, category, etc.)
3. View matching results

### Marking as Resolved
1. Click "Mark Resolved" on an item card
2. Confirm the action
3. Item status will update to resolved

## 🏗️ Architecture

### Backend (server.js)
- **Express.js** - Web framework
- **SQLite3** - Database
- **CORS** - Cross-origin support
- **Body-parser** - JSON parsing

### Frontend (public/)
- **HTML5** - Semantic structure
- **CSS3** - Responsive design with gradients
- **Vanilla JavaScript** - No framework dependencies

### Database Schema
```sql
CREATE TABLE items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    type TEXT NOT NULL,              -- 'Lost' or 'Found'
    name TEXT NOT NULL,
    category TEXT NOT NULL,
    place TEXT NOT NULL,
    date TEXT NOT NULL,
    contact TEXT NOT NULL,
    description TEXT,
    resolved INTEGER DEFAULT 0,      -- 0 = active, 1 = resolved
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP
)
```

## 📡 API Endpoints

### Items
- `POST /api/items` - Add new item
- `GET /api/items` - Get all items (query: type)
- `GET /api/items/active` - Get active items (query: type)
- `GET /api/items/:id` - Get item by ID
- `PUT /api/items/:id` - Update item
- `DELETE /api/items/:id` - Delete item
- `PATCH /api/items/:id/resolve` - Mark as resolved

### Search & Stats
- `GET /api/search` - Search items (query: keyword)
- `GET /api/statistics` - Get statistics

## 🔍 Matching Algorithm

The system uses the **Levenshtein Distance** algorithm for intelligent item matching:

- Calculates minimum character edits needed between strings
- Case-insensitive comparison
- Tolerates up to 1/3 character differences
- Matches by name, location, or category

Example:
- "iPhone 13" matches "iphone 13" ✅
- "Samsung Galaxy" matches "Samsung Galxy" ✅

## 🎨 Design Highlights

- **Gradient Background** - Professional purple gradient
- **Card-Based Layout** - Clean, organized item cards
- **Interactive Elements** - Smooth hover effects and transitions
- **Modal Details** - View full item details in a popup
- **Mobile Responsive** - Adapts to all screen sizes
- **Color Coding** - Lost items in red, Found items in green

## 📊 Statistics

The dashboard displays:
- Total lost items
- Total found items
- Active cases
- Resolved items per category

## 🛠️ Development

### Project Structure
```
project-code/
├── server.js              # Express backend
├── package.json           # Dependencies
├── database.db            # SQLite database (auto-created)
├── public/
│   ├── index.html         # Main HTML
│   ├── style.css          # Styling
│   └── script.js          # Frontend logic
├── LICENSE                # MIT License
├── CHANGELOG.md           # Version history
└── .gitignore             # Git config
```

### Running in Development Mode
```bash
npm install --save-dev nodemon
npm run dev
```

### Available Scripts
- `npm start` - Start production server
- `npm run dev` - Start with auto-reload (requires nodemon)

## 📈 Version History

### v2.0.0 (Current) - Web Application
- Full-stack web implementation
- SQLite database
- Modern responsive UI
- RESTful API
- Advanced matching algorithms

### v1.0.0 - Console Application
- Command-line interface in C
- File-based storage
- Basic matching functionality

See [CHANGELOG.md](CHANGELOG.md) for complete version history.

## 🚀 Deployment

### Deploy to Heroku
```bash
heroku create your-app-name
git push heroku main
```

### Deploy to Railway
```bash
railway up
```

### Deploy to Vercel
- Note: Vercel is for serverless, consider Railway or Heroku for database-backed apps

## 🔐 Security Notes

- Input validation on all fields
- SQL injection protection (parameterized queries)
- CORS enabled for cross-origin requests
- No sensitive data in frontend

## 🐛 Known Limitations

- In-memory data between server restarts (can be enhanced with persistent backup)
- Single server instance (no clustering)
- No user authentication (planned for v3.0)
- No image uploads (planned feature)

## 🔄 Future Enhancements

- [ ] User authentication & accounts
- [ ] Image uploads for items
- [ ] Email/SMS notifications
- [ ] Map integration with location
- [ ] Mobile app (React Native)
- [ ] Admin dashboard
- [ ] PostgreSQL migration
- [ ] API documentation (Swagger)
- [ ] Unit tests
- [ ] Docker containerization

## 📝 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

**Copyright © 2026 Ynnah00**

## 🤝 Contributing

Contributions are welcome! Feel free to:
- Report bugs
- Suggest features
- Submit pull requests

## 📧 Contact

For questions or suggestions, reach out to the project owner.

## 🙏 Acknowledgments

- Fuzzy matching algorithm inspired by string similarity techniques
- UI design inspired by modern web applications
- Built with ❤️ as a portfolio project

---

**Happy Finding! 🎉**

*Last Updated: June 30, 2026*
