## ADDED Requirements

### Requirement: Provide settings dialog
The system SHALL provide a settings dialog with multiple categories.

#### Scenario: Open settings
- **WHEN** user clicks settings icon
- **THEN** system opens settings dialog
- **AND** system displays settings categories in sidebar

#### Scenario: Close settings
- **WHEN** user clicks close button or applies settings
- **THEN** system saves settings to database
- **AND** system closes settings dialog
- **AND** system applies settings changes

### Requirement: General settings category
The system SHALL provide general settings for application behavior.

#### Scenario: Set theme
- **WHEN** user selects theme in general settings
- **THEN** system saves theme preference
- **AND** system applies selected theme immediately

#### Scenario: Set language
- **WHEN** user selects language in general settings
- **THEN** system saves language preference
- **AND** system applies selected language

#### Scenario: Set startup options
- **WHEN** user configures startup options in general settings
- **THEN** system saves startup behavior
- **AND** system applies on next launch

### Requirement: Appearance settings category
The system SHALL provide appearance settings for UI customization.

#### Scenario: Set card style
- **WHEN** user selects card style in appearance settings
- **THEN** system saves card style preference
- **AND** system updates task card display

#### Scenario: Set font size
- **WHEN** user selects font size in appearance settings
- **THEN** system saves font size preference
- **AND** system updates font across application

#### Scenario: Set icon style
- **WHEN** user selects icon style in appearance settings
- **THEN** system saves icon style preference
- **AND** system updates icons across application

#### Scenario: Set card corner radius
- **WHEN** user adjusts card corner radius in appearance settings
- **THEN** system saves corner radius preference
- **AND** system updates task card styling

### Requirement: Notification settings category
The system SHALL provide notification settings for alerts and reminders.

#### Scenario: Enable/disable notifications
- **WHEN** user toggles notifications in notification settings
- **THEN** system saves notification enabled state
- **AND** system enables/disables notifications

#### Scenario: Configure deadline reminders
- **WHEN** user configures deadline reminder times
- **THEN** system saves reminder preferences
- **AND** system creates notifications at configured times

#### Scenario: Configure recycle bin reminders
- **WHEN** user configures recycle bin warning times
- **THEN** system saves warning preferences
- **AND** system creates warnings at configured times

#### Scenario: Configure backup notifications
- **WHEN** user toggles backup notifications
- **THEN** system saves backup notification preference
- **AND** system creates/removes backup notifications

#### Scenario: Configure system notifications
- **WHEN** user enables system notifications
- **THEN** system configures Windows notifications
- **AND** system shows system notifications for events

#### Scenario: Configure notification sounds
- **WHEN** user enables notification sounds
- **THEN** system saves sound preference
- **AND** system plays sound for notifications

### Requirement: Backup settings category
The system SHALL provide backup settings for data protection.

#### Scenario: Enable automatic backup
- **WHEN** user toggles automatic backup
- **THEN** system saves backup enabled state
- **AND** system starts/stops automatic backup schedule

#### Scenario: Set backup frequency
- **WHEN** user selects backup frequency
- **THEN** system saves frequency preference
- **AND** system schedules backups accordingly

#### Scenario: Set backup time
- **WHEN** user sets backup time
- **THEN** system saves backup time preference
- **AND** system schedules backup at specified time

#### Scenario: Set backup retention count
- **WHEN** user sets number of backups to keep
- **THEN** system saves retention preference
- **AND** system deletes old backups exceeding limit

#### Scenario: Set backup location
- **WHEN** user selects backup directory
- **THEN** system validates directory is writable
- **AND** system saves backup location
- **AND** system uses location for future backups

#### Scenario: Trigger manual backup
- **WHEN** user clicks "Backup Now"
- **THEN** system performs immediate backup
- **AND** system shows backup progress

#### Scenario: View backup history
- **WHEN** user clicks "View Backup History"
- **THEN** system displays list of all backups
- **AND** system shows backup dates and sizes

### Requirement: Data settings category
The system SHALL provide data settings for database management.

#### Scenario: View database location
- **WHEN** user views data settings
- **THEN** system displays current database path
- **AND** system shows database file size

#### Scenario: Open database folder
- **WHEN** user clicks "Open Folder"
- **THEN** system opens database directory in file explorer

#### Scenario: Export data
- **WHEN** user clicks "Export Data"
- **THEN** system opens export dialog
- **AND** system allows user to select format and location

#### Scenario: Import data
- **WHEN** user clicks "Import Data"
- **THEN** system opens import dialog
- **AND** system allows user to select import file and options

#### Scenario: Clear cache
- **WHEN** user clicks "Clear Cache"
- **THEN** system shows cache size
- **WHEN** user confirms
- **THEN** system clears cache
- **AND** system shows space freed

### Requirement: Delete settings category
The system SHALL provide delete settings for task removal behavior.

#### Scenario: Set parent task delete action
- **WHEN** user selects delete action for parent tasks
- **THEN** system saves delete preference (cascade or promote)
- **AND** system applies action when parent tasks are deleted

#### Scenario: Enable recycle bin auto-cleanup
- **WHEN** user toggles auto-cleanup
- **THEN** system saves auto-cleanup preference
- **AND** system starts/stops cleanup schedule

#### Scenario: Set cleanup days
- **WHEN** user sets number of days before cleanup
- **THEN** system validates minimum value (e.g., 1 day)
- **AND** system saves cleanup days preference

### Requirement: Keyboard shortcuts settings category
The system SHALL provide keyboard shortcuts settings for productivity.

#### Scenario: View shortcuts
- **WHEN** user opens shortcuts settings
- **THEN** system displays all keyboard shortcuts
- **AND** system shows current key bindings

#### Scenario: Customize shortcut
- **WHEN** user clicks on a shortcut
- **THEN** system prompts for new key combination
- **WHEN** user presses new keys
- **THEN** system saves new shortcut
- **AND** system detects conflicts with existing shortcuts

#### Scenario: Reset shortcuts
- **WHEN** user clicks "Reset to Defaults"
- **THEN** system restores default shortcuts
- **AND** system updates all key bindings

### Requirement: About settings category
The system SHALL provide about information and support options.

#### Scenario: View application version
- **WHEN** user opens about settings
- **THEN** system displays application name and version
- **AND** system shows Qt version

#### Scenario: Check for updates
- **WHEN** user clicks "Check for Updates"
- **THEN** system queries for latest version
- **AND** system shows update status

#### Scenario: View license
- **WHEN** user clicks "View License"
- **THEN** system displays license information

#### Scenario: Provide feedback link
- **WHEN** user clicks "Feedback"
- **THEN** system opens feedback form or email client

#### Scenario: Provide GitHub link
- **WHEN** user clicks "GitHub Repository"
- **THEN** system opens GitHub repository in browser

### Requirement: Settings persistence
The system SHALL save all settings and restore on startup.

#### Scenario: Save settings on change
- **WHEN** user modifies any setting
- **THEN** system saves setting to database
- **AND** system applies setting immediately (if applicable)

#### Scenario: Restore settings on startup
- **WHEN** application starts
- **THEN** system loads all settings from database
- **AND** system applies saved settings

### Requirement: Settings validation
The system SHALL validate settings before saving.

#### Scenario: Validate numeric settings
- **WHEN** user enters invalid numeric value
- **THEN** system shows validation error
- **AND** system prevents saving invalid value

#### Scenario: Validate path settings
- **WHEN** user selects invalid path
- **THEN** system shows validation error
- **AND** system prompts for valid path

#### Scenario: Validate time settings
- **WHEN** user enters invalid time
- **THEN** system shows validation error
- **AND** system accepts only valid time formats

### Requirement: Reset settings
The system SHALL allow users to reset all settings to defaults.

#### Scenario: Reset all settings
- **WHEN** user clicks "Reset All Settings"
- **THEN** system shows confirmation dialog
- **WHEN** user confirms
- **THEN** system deletes all custom settings
- **AND** system restores default values
- **AND** system applies default settings

### Requirement: Settings categories navigation
The system SHALL provide easy navigation between settings categories.

#### Scenario: Navigate categories
- **WHEN** user clicks different category in settings sidebar
- **THEN** system displays selected category
- **AND** system highlights active category

#### Scenario: Keyboard navigation
- **WHEN** user uses arrow keys in settings
- **THEN** system moves selection between categories
- **AND** system allows Enter to select category

### Requirement: Search settings
The system SHALL allow searching within settings.

#### Scenario: Search settings
- **WHEN** user types in settings search box
- **THEN** system filters settings options
- **AND** system highlights matching settings

#### Scenario: Clear settings search
- **WHEN** user clears settings search
- **THEN** system displays all settings options
- **AND** system removes highlighting
