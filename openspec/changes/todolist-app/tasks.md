## 1. Project Setup

- [x] 1.1 Create CMakeLists.txt with Qt 5.15.2 and MSVC 2019 configuration
- [x] 1.2 Create project directory structure (src/, resources/, data/, backup/, logs/, docs/)
- [x] 1.3 Initialize SQLite database with all required tables
- [x] 1.4 Implement Logger utility class with multi-level support
- [x] 1.5 Create basic application entry point (main.cpp, App class)

## 2. Core Models and Data Layer

- [x] 2.1 Implement Task data model (task.h/cpp)
- [x] 2.2 Implement TaskModel (QStandardItemModel) for tree structure
- [x] 2.3 Implement Notification data model (notification.h/cpp)
- [x] 2.4 Implement Folder data model (folder.h/cpp)
- [x] 2.5 Implement Database controller class (database.h/cpp)
- [x] 2.6 Add database connection and query methods
- [x] 2.7 Implement database initialization and table creation

## 3. UI Foundation - MainWindow

- [x] 3.1 Create MainWindow class (mainwindow.h/cpp)
- [x] 3.2 Implement main window layout (sidebar + content area)
- [x] 3.3 Add top toolbar with search box and buttons
- [x] 3.4 Configure window size (1280x720) and minimum size (1024x768)
- [x] 3.5 Implement window state persistence (size, position)

## 4. Sidebar Implementation

- [x] 4.1 Create Sidebar widget (sidebar.h/cpp)
- [x] 4.2 Implement time-based groups (Today, This Week, This Month, Overdue)
- [x] 4.3 Implement priority-based groups (High, Medium, Low)
- [x] 4.4 Implement status-based groups (Completed, Incomplete, In Progress)
- [x] 4.5 Implement tag-based grouping
- [x] 4.6 Add custom folder functionality (create, rename, delete)
- [x] 4.7 Implement sidebar width adjustment (280px default, resizable with QSplitter)
- [x] 4.8 Add collapse/expand sidebar functionality

## 5. Task Tree View

- [x] 5.1 Create TaskTree widget using QTreeView + QStandardItemModel
- [x] 5.2 Implement tree display with indentation for hierarchy
- [x] 5.3 Add expand/collapse functionality for parent tasks
- [x] 5.4 Implement lazy loading of child tasks
- [x] 5.5 Add drag-and-drop reordering support
- [x] 5.6 Implement recursive query for task hierarchy (WITH RECURSIVE)

## 6. Task Card Component

- [x] 6.1 Create TaskCard widget (taskcard.h/cpp)
- [x] 6.2 Implement task card layout (title, description, metadata)
- [x] 6.3 Add priority indicator with colors (red/amber/green)
- [x] 6.4 Add deadline display with date formatting
- [x] 6.5 Add progress bar (0-100%)
- [x] 6.6 Add tags display
- [x] 6.7 Add completion checkbox
- [x] 6.8 Add edit and delete buttons
- [x] 6.9 Implement hover effects with theme colors

## 7. Task Dialogs

- [x] 7.1 Create TaskDialog for creating/editing tasks (taskdialog.h/cpp)
- [x] 7.2 Add title input field
- [x] 7.3 Add description text area
- [x] 7.4 Add priority selector (High/Medium/Low)
- [x] 7.5 Add deadline date and time pickers
- [x] 7.6 Add tag input with multi-select
- [x] 7.7 Add progress slider (0-100%)
- [x] 7.8 Add file association (browse, remove, open)
- [x] 7.9 Add subtask steps editor
- [x] 7.10 Add dependency selection

## 8. Task CRUD Operations

- [x] 8.1 Implement create task operation
- [x] 8.2 Implement edit task operation
- [x] 8.3 Implement delete task operation (with confirmation)
- [x] 8.4 Implement mark complete/incomplete operation
- [x] 8.5 Add parent-child task relationship handling
- [x] 8.6 Implement auto-calculate parent progress from children

## 9. Theme System

- [x] 9.1 Create dark.qss stylesheet with blue primary colors
- [x] 9.2 Create light.qss stylesheet with blue primary colors
- [x] 9.3 Implement ThemeUtils class for theme management
- [x] 9.4 Add theme switching functionality (dark/light)
- [x] 9.5 Apply theme to all widgets dynamically
- [x] 9.6 Implement priority color schemes (red/amber/green)
- [x] 9.7 Add theme persistence (save/load preference)

## 10. Search and Filter

- [x] 10.1 Create SearchWidget (searchwidget.h/cpp)
- [x] 10.2 Implement real-time search as user types
- [x] 10.3 Add advanced search panel with filters
- [x] 10.4 Implement priority filter dropdown
- [x] 10.5 Implement status filter dropdown
- [x] 10.6 Implement date filter dropdown (Today, This Week, This Month, Overdue)
- [x] 10.7 Implement tag filter (multi-select)
- [x] 10.8 Implement sort options (deadline, priority, created, manual)
- [x] 10.9 Set up FTS5 full-text search in SQLite
- [x] 10.10 Create FTS virtual table and triggers

## 11. Task Dependencies

- [x] 11.1 Implement task_dependencies table operations
- [x] 11.2 Add dependency display on task card
- [x] 11.3 Implement dependency selection in task dialog
- [x] 11.4 Add circular dependency detection
- [x] 11.5 Implement flat display for circular dependencies

## 12. File Association

- [x] 12.1 Add file_path field to tasks table
- [x] 12.2 Implement file association in task dialog
- [x] 12.3 Add file type icon display
- [x] 12.4 Implement open file with default application (QDesktopServices)
- [x] 12.5 Add file validation (exists check)
- [x] 12.6 Handle missing file scenarios with warning

## 13. Notification System

- [x] 13.1 Create NotificationManager class (notificationmanager.h/cpp)
- [x] 13.2 Create NotificationPanel widget (notificationpanel.h/cpp)
- [x] 13.3 Implement bell icon with red dot indicator
- [x] 13.4 Add notification display in panel
- [x] 13.5 Implement mark as read functionality
- [x] 13.6 Add notification types (delete_warning, deadline, backup, system)
- [x] 13.7 Implement delete notification action
- [x] 13.8 Add notification history view

## 14. Recycle Bin

- [x] 14.1 Implement soft delete (is_deleted flag)
- [x] 14.2 Add recycle bin view in sidebar
- [x] 14.3 Implement restore from recycle bin
- [x] 14.4 Implement permanent delete with confirmation
- [x] 14.5 Add parent task delete handling (cascade or promote children)
- [x] 14.6 Implement 14-day auto-cleanup
- [x] 14.7 Add 3-day and 1-day deletion warnings

## 15. Backup System

- [x] 15.1 Create BackupManager class (backupmanager.h/cpp)
- [x] 15.2 Implement daily automatic backup
- [x] 15.3 Add backup frequency configuration (hourly/daily/weekly)
- [x] 15.4 Implement backup time setting
- [x] 15.5 Add backup retention (keep N backups)
- [x] 15.6 Add backup location configuration
- [x] 15.7 Add manual backup trigger
- [x] 15.8 Implement backup progress indication
- [x] 15.9 Add backup completion notification

## 16. Settings System

- [x] 16.1 Create SettingsDialog class (settingsdialog.h/cpp)
- [x] 16.2 Add General settings category (theme, language, startup options)
- [x] 16.3 Add Appearance settings category (card style, font size, icon style, corner radius)
- [x] 16.4 Add Notification settings category (enable/disable, reminders, system notifications, sounds)
- [x] 16.5 Add Backup settings category (enable, frequency, time, retention, location)
- [x] 16.6 Add Data settings category (database location, export, import, clear cache)
- [x] 16.7 Add Delete settings category (parent action, auto-cleanup, cleanup days)
- [x] 16.8 Add Keyboard shortcuts category (view, customize, reset)
- [x] 16.9 Add About settings category (version, check updates, feedback, GitHub)
- [x] 16.10 Implement settings persistence (save/load from database)

## 17. Data Import/Export

- [x] 17.1 Implement JSON export with all data
- [x] 17.2 Add export options (selective export)
- [x] 17.3 Implement SQLite export (full database copy)
- [x] 17.4 Implement JSON import with validation
- [x] 17.5 Implement SQLite import with overwrite warning
- [x] 17.6 Add import modes (merge, overwrite, append)
- [x] 17.7 Implement conflict resolution (skip, overwrite, regenerate IDs)
- [x] 17.8 Add export/import progress indication

## 18. Utility Classes

- [x] 18.1 Implement DateUtils for date formatting and calculations
- [x] 18.2 Implement FileUtils for path handling and validation
- [x] 18.3 Complete ThemeUtils with QSS loading methods
- [x] 18.4 Add IconUtils for SVG icon loading and caching

## 19. Database Optimization

- [x] 19.1 Create indexes for tasks table (parent_id, deadline, priority, is_deleted)
- [x] 19.2 Create indexes for performance optimization
- [x] 19.3 Implement database VACUUM for maintenance (weekly)
- [x] 19.4 Add periodic maintenance task (cleanup old notifications, deleted tasks, scheduled)

## 20. Empty States and Error Handling

- [x] 20.1 Create empty state widgets for no tasks
- [x] 20.2 Create empty state for search no results
- [x] 20.3 Create empty state for recycle bin
- [x] 20.4 Add error dialogs for database corruption
- [x] 20.5 Add error dialogs for file operations
- [x] 20.6 Add error dialogs for save failures
- [x] 20.7 Add error dialogs for import/export failures

## 21. Final Polish and Testing

- [ ] 21.1 Test all CRUD operations
- [ ] 21.2 Test task hierarchy (create, expand, collapse, edit, delete)
- [ ] 21.3 Test search and filter functionality
- [ ] 21.4 Test theme switching (dark/light)
- [ ] 21.5 Test notification system
- [ ] 21.6 Test backup and restore
- [ ] 21.7 Test import/export (JSON, SQLite)
- [ ] 21.8 Test recycle bin (delete, restore, auto-cleanup)
- [ ] 21.9 Test all settings categories
- [ ] 21.10 Verify performance with 1000+ tasks

## 22. Build and Packaging

- [ ] 22.1 Configure CMake for Release build
- [ ] 22.2 Build Release version with MSVC 2019 64-bit
- [ ] 22.3 Create resources directory structure (icons, styles)
- [ ] 22.4 Package Qt DLLs using windeployqt
- [ ] 22.5 Create Release folder with all dependencies
- [ ] 22.6 Copy resources (icons, QSS files) to Release folder
- [ ] 22.7 Create README.txt with usage instructions
- [ ] 22.8 Test packaged application on clean Windows system
- [ ] 22.9 Create distributable zip package

## 23. Documentation

- [ ] 23.1 Update README.md with project description
- [ ] 23.2 Add installation instructions
- [ ] 23.3 Document all keyboard shortcuts
- [ ] 23.4 Document settings and configuration
- [ ] 23.5 Add troubleshooting section
