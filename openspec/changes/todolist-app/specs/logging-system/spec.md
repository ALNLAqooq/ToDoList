## ADDED Requirements

### Requirement: Multi-level logging
The system SHALL support multiple log levels (DEBUG, INFO, WARNING, ERROR, CRITICAL).

#### Scenario: Log debug messages
- **WHEN** code logs at DEBUG level
- **THEN** system writes to log file if DEBUG level is enabled
- **AND** system includes timestamp and level

#### Scenario: Log info messages
- **WHEN** code logs at INFO level
- **THEN** system writes to log file
- **AND** system includes timestamp and level

#### Scenario: Log warning messages
- **WHEN** code logs at WARNING level
- **THEN** system writes to log file
- **AND** system includes timestamp and level

#### Scenario: Log error messages
- **WHEN** code logs at ERROR level
- **THEN** system writes to log file
- **AND** system includes timestamp and level
- **AND** system highlights error in log viewer

#### Scenario: Log critical messages
- **WHEN** code logs at CRITICAL level
- **THEN** system writes to log file
- **AND** system includes timestamp and level
- **AND** system creates notification for critical errors

### Requirement: Log file management
The system SHALL manage log files with rotation.

#### Scenario: Create log file
- **WHEN** application starts
- **THEN** system creates new log file with date
- **AND** system uses format: todolist_YYYYMMDD.log

#### Scenario: Rotate log files
- **WHEN** current log file exceeds 10MB
- **THEN** system creates new log file with sequence number
- **AND** system keeps maximum 30 log files

#### Scenario: Delete old log files
- **WHEN** more than 30 log files exist
- **THEN** system deletes oldest log file
- **AND** system maintains 30 most recent logs

### Requirement: Log file location
The system SHALL store logs in dedicated directory.

#### Scenario: Default log location
- **WHEN** application starts for first time
- **THEN** system creates logs/ directory
- **AND** system stores log files in logs/

#### Scenario: Custom log location
- **WHEN** user configures custom log directory in settings
- **THEN** system saves log location preference
- **AND** system stores log files in custom directory

### Requirement: Configurable log level
The system SHALL allow users to configure minimum log level.

#### Scenario: Set log level to DEBUG
- **WHEN** user selects DEBUG level in settings
- **THEN** system logs all messages (DEBUG and above)
- **AND** system saves log level preference

#### Scenario: Set log level to INFO
- **WHEN** user selects INFO level in settings
- **THEN** system logs INFO and above (excludes DEBUG)
- **AND** system saves log level preference

#### Scenario: Set log level to WARNING
- **WHEN** user selects WARNING level in settings
- **THEN** system logs WARNING and above
- **AND** system saves log level preference

#### Scenario: Set log level to ERROR
- **WHEN** user selects ERROR level in settings
- **THEN** system logs ERROR and CRITICAL only
- **AND** system saves log level preference

### Requirement: Log message format
The system SHALL use consistent log message format.

#### Scenario: Standard log format
- **WHEN** system writes log message
- **THEN** system uses format: [YYYY-MM-DD HH:MM:SS] [LEVEL] [CATEGORY] message
- **AND** system includes context information

#### Scenario: Component-specific logging
- **WHEN** different components log messages
- **THEN** system includes component category
- **AND** system uses categories: [Database], [Task], [Backup], [UI]

### Requirement: View logs in settings
The system SHALL provide log viewer in settings.

#### Scenario: Open log viewer
- **WHEN** user clicks "View Logs" in settings
- **THEN** system opens log viewer dialog
- **AND** system displays log files list
- **AND** system displays log contents of selected file

#### Scenario: Filter logs by level
- **WHEN** user selects log level filter in log viewer
- **THEN** system displays only logs at or above selected level
- **AND** system updates log display

#### Scenario: Search logs
- **WHEN** user types in log search box
- **THEN** system filters log messages
- **AND** system highlights matching text

### Requirement: Clear log files
The system SHALL allow users to clear log files.

#### Scenario: Clear all logs
- **WHEN** user clicks "Clear Logs" in settings
- **THEN** system shows confirmation dialog
- **WHEN** user confirms
- **THEN** system deletes all log files
- **AND** system creates new empty log file

#### Scenario: Clear specific log file
- **WHEN** user right-clicks on log file and selects delete
- **THEN** system shows confirmation for that file
- **WHEN** user confirms
- **THEN** system deletes selected log file

### Requirement: Open log directory
The system SHALL allow users to open log directory in file explorer.

#### Scenario: Open log folder
- **WHEN** user clicks "Open Log Folder" in settings
- **THEN** system opens logs/ directory in file explorer
- **AND** system allows user to access log files directly

### Requirement: Database logging
The system SHALL log database operations.

#### Scenario: Log database queries
- **WHEN** system executes SQL query
- **THEN** system logs query at DEBUG level
- **AND** system includes execution time

#### Scenario: Log database errors
- **WHEN** database operation fails
- **THEN** system logs error at ERROR level
- **AND** system includes SQL statement and error message

#### Scenario: Log connection issues
- **WHEN** database connection fails
- **THEN** system logs connection error at CRITICAL level
- **AND** system includes connection string and error details

### Requirement: Backup logging
The system SHALL log backup operations.

#### Scenario: Log backup start
- **WHEN** backup operation starts
- **THEN** system logs backup start at INFO level
- **AND** system includes backup file path

#### Scenario: Log backup progress
- **WHEN** backup progresses
- **THEN** system logs progress at DEBUG level
- **AND** system includes percentage

#### Scenario: Log backup completion
- **WHEN** backup completes successfully
- **THEN** system logs completion at INFO level
- **AND** system includes file size and duration

#### Scenario: Log backup failure
- **WHEN** backup operation fails
- **THEN** system logs failure at ERROR level
- **AND** system includes error details

### Requirement: Task operation logging
The system SHALL log task operations.

#### Scenario: Log task creation
- **WHEN** user creates new task
- **THEN** system logs creation at INFO level
- **AND** system includes task ID and title

#### Scenario: Log task update
- **WHEN** user modifies task
- **THEN** system logs update at INFO level
- **AND** system includes task ID and changed fields

#### Scenario: Log task deletion
- **WHEN** user deletes task
- **THEN** system logs deletion at INFO level
- **AND** system includes task ID and deletion type

### Requirement: Error logging
The system SHALL log all errors with context.

#### Scenario: Log file errors
- **WHEN** file operation fails (e.g., cannot open file)
- **THEN** system logs error at ERROR level
- **AND** system includes file path and error reason

#### Scenario: Log UI errors
- **WHEN** UI operation fails (e.g., cannot load widget)
- **THEN** system logs error at ERROR level
- **AND** system includes widget type and error details

#### Scenario: Log configuration errors
- **WHEN** setting is invalid or missing
- **THEN** system logs error at WARNING level
- **AND** system includes setting name and default value used

### Requirement: Performance logging
The system SHALL log performance metrics.

#### Scenario: Log startup time
- **WHEN** application finishes loading
- **THEN** system logs startup duration at INFO level
- **AND** system includes task count loaded

#### Scenario: Log slow queries
- **WHEN** database query takes longer than threshold
- **THEN** system logs warning at WARNING level
- **AND** system includes query and execution time

### Requirement: Log viewer features
The system SHALL provide useful log viewer features.

#### Scenario: Auto-scroll to bottom
- **WHEN** new log messages arrive
- **THEN** system auto-scrolls to bottom
- **AND** system shows latest messages

#### Scenario: Copy log entry
- **WHEN** user selects log entry and copies
- **THEN** system copies log text to clipboard
- **AND** system includes timestamp and level

#### Scenario: Export log file
- **WHEN** user clicks "Export Log" in log viewer
- **THEN** system opens save dialog
- **AND** system saves log file to selected location
