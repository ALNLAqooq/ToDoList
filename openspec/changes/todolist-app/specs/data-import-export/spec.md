## ADDED Requirements

### Requirement: Export to JSON
The system SHALL allow users to export all data to JSON format.

#### Scenario: Full JSON export
- **WHEN** user clicks "Export" and selects JSON format
- **THEN** system exports tasks, folders, settings, and notifications
- **AND** system includes version and export date
- **AND** system prompts for file save location

#### Scenario: Selective JSON export
- **WHEN** user unchecks some export options
- **THEN** system exports only selected data types
- **AND** system excludes unchecked items from export

### Requirement: Export to SQLite
The system SHALL allow users to export complete database to SQLite format.

#### Scenario: Full SQLite export
- **WHEN** user clicks "Export" and selects SQLite format
- **THEN** system copies entire database file
- **AND** system performs VACUUM before copying
- **AND** system prompts for file save location

#### Scenario: SQLite export with timestamp
- **WHEN** system saves SQLite export
- **THEN** system includes timestamp in filename
- **AND** system uses format: todolist_backup_YYYYMMDD.db

### Requirement: Import from JSON
The system SHALL allow users to import data from JSON files.

#### Scenario: Import JSON file
- **WHEN** user clicks "Import" and selects JSON file
- **THEN** system validates JSON format
- **AND** system displays import options
- **WHEN** user selects import mode and confirms
- **THEN** system imports data from JSON

#### Scenario: JSON validation
- **WHEN** user selects invalid JSON file
- **THEN** system shows error message
- **AND** system prevents import operation

### Requirement: Import from SQLite
The system SHALL allow users to restore from SQLite database files.

#### Scenario: Import SQLite file
- **WHEN** user clicks "Import" and selects SQLite file
- **THEN** system validates SQLite format
- **AND** system shows warning about replacing data
- **WHEN** user confirms
- **THEN** system replaces current database with imported file

#### Scenario: SQLite validation
- **WHEN** user selects invalid SQLite file
- **THEN** system shows error message
- **AND** system prevents import operation

### Requirement: Import merge mode
The system SHALL allow importing data with merge option.

#### Scenario: Merge import
- **WHEN** user selects "Merge" import mode
- **THEN** system adds new tasks to existing data
- **AND** system updates existing tasks with matching IDs
- **AND** system preserves current data

#### Scenario: Merge conflict handling
- **WHEN** import has task with same ID as existing task
- **THEN** system applies conflict resolution setting
- **AND** system either skips, overwrites, or regenerates ID

### Requirement: Import overwrite mode
The system SHALL allow importing data with overwrite option.

#### Scenario: Overwrite import
- **WHEN** user selects "Overwrite" import mode
- **THEN** system clears existing data
- **AND** system imports all data from file
- **AND** system shows warning about data loss

#### Scenario: Overwrite confirmation
- **WHEN** user selects "Overwrite" mode
- **THEN** system shows strong confirmation dialog
- **AND** system explains data will be lost

### Requirement: Import append mode
The system SHALL allow importing tasks without importing settings.

#### Scenario: Append import
- **WHEN** user selects "Append" import mode
- **THEN** system imports only tasks and folders
- **AND** system does not import settings or notifications
- **AND** system preserves current settings

### Requirement: Conflict resolution
The system SHALL provide options for handling import conflicts.

#### Scenario: Skip duplicate IDs
- **WHEN** import has duplicate IDs and user selects "Skip"
- **THEN** system skips tasks with existing IDs
- **AND** system imports only new tasks

#### Scenario: Overwrite duplicate IDs
- **WHEN** import has duplicate IDs and user selects "Overwrite"
- **THEN** system replaces existing tasks with imported data
- **AND** system updates all task attributes

#### Scenario: Regenerate IDs
- **WHEN** import has duplicate IDs and user selects "Regenerate"
- **THEN** system assigns new IDs to all imported tasks
- **AND** system updates internal references

### Requirement: Export progress indication
The system SHALL show export progress to user.

#### Scenario: Show export progress
- **WHEN** export is in progress
- **THEN** system displays progress bar
- **AND** system updates percentage
- **AND** system shows estimated time remaining

#### Scenario: Export complete
- **WHEN** export finishes successfully
- **THEN** system shows success message
- **AND** system includes file location
- **AND** system offers to open containing folder

### Requirement: Import progress indication
The system SHALL show import progress to user.

#### Scenario: Show import progress
- **WHEN** import is in progress
- **THEN** system displays progress bar
- **AND** system updates percentage
- **AND** system shows number of items processed

#### Scenario: Import complete
- **WHEN** import finishes successfully
- **THEN** system shows success message
- **AND** system displays count of imported items
- **AND** system refreshes task list

### Requirement: Export file validation
The system SHALL validate export file location and format.

#### Scenario: Validate export location
- **WHEN** user selects export location
- **THEN** system checks directory exists and is writable
- **AND** system shows error if location is invalid

#### Scenario: File overwrite warning
- **WHEN** export file already exists
- **THEN** system shows overwrite warning
- **AND** system allows user to cancel or overwrite

### Requirement: Import file validation
The system SHALL validate import file before processing.

#### Scenario: Validate JSON structure
- **WHEN** user selects JSON import file
- **THEN** system checks required fields exist
- **AND** system validates data types
- **AND** system shows specific error if invalid

#### Scenario: Validate SQLite structure
- **WHEN** user selects SQLite import file
- **THEN** system checks for required tables
- **AND** system validates table structure
- **AND** system shows error if incompatible

### Requirement: JSON export format
The system SHALL export JSON with consistent structure.

#### Scenario: JSON structure
- **WHEN** system exports to JSON
- **THEN** system includes: version, export_date, data object
- **AND** data object contains: tasks, folders, settings, notifications
- **AND** tasks include full hierarchy (subtasks nested)

#### Scenario: JSON data types
- **WHEN** system exports dates
- **THEN** system uses ISO 8601 format (YYYY-MM-DDTHH:MM:SSZ)
- **WHEN** system exports progress
- **THEN** system uses integer (0-100)
- **WHEN** system exports tags
- **THEN** system uses array of strings

### Requirement: Export options
The system SHALL allow users to select export options.

#### Scenario: Export all data
- **WHEN** user checks all export options
- **THEN** system exports: tasks, folders, settings, notifications
- **AND** system includes all task data (dependencies, files)

#### Scenario: Export tasks only
- **WHEN** user unchecks settings and notifications
- **THEN** system exports only tasks and folders
- **AND** system excludes settings and notifications

#### Scenario: Export with subtasks
- **WHEN** user exports tasks
- **THEN** system includes all subtasks in hierarchy
- **AND** system maintains parent-child relationships
