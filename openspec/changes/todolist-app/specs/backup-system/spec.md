## ADDED Requirements

### Requirement: Automatic backup
The system SHALL automatically backup database at configured intervals.

#### Scenario: Daily backup
- **WHEN** backup frequency is set to "Daily"
- **AND** configured backup time is reached
- **THEN** system performs full database backup
- **AND** system saves backup to configured location

#### Scenario: Hourly backup
- **WHEN** backup frequency is set to "Hourly"
- **AND** hour has passed since last backup
- **THEN** system performs full database backup
- **AND** system saves backup with timestamp

#### Scenario: Weekly backup
- **WHEN** backup frequency is set to "Weekly"
- **AND** configured day and time is reached
- **THEN** system performs full database backup
- **AND** system saves backup to configured location

### Requirement: Manual backup
The system SHALL allow users to manually trigger backup.

#### Scenario: Immediate backup
- **WHEN** user clicks "Backup Now" in settings
- **THEN** system performs full database backup
- **AND** system shows backup progress
- **AND** system shows success message when complete

#### Scenario: Manual backup while automatic pending
- **WHEN** user triggers manual backup
- **AND** automatic backup is scheduled
- **THEN** system performs manual backup
- **AND** system reschedules next automatic backup

### Requirement: Backup retention
The system SHALL maintain configurable number of backup versions.

#### Scenario: Keep N backups
- **WHEN** backup retention is set to N
- **AND** N+1 backups exist
- **THEN** system deletes oldest backup
- **AND** system maintains N most recent backups

#### Scenario: Default retention
- **WHEN** system is first installed
- **THEN** system sets default retention to 7 backups
- **AND** system saves retention setting

### Requirement: Backup location configuration
The system SHALL allow users to configure backup location.

#### Scenario: Set backup location
- **WHEN** user selects backup directory in settings
- **THEN** system saves backup path to settings
- **AND** system validates directory exists
- **AND** system creates directory if needed

#### Scenario: Backup location validation
- **WHEN** selected backup location is not writable
- **THEN** system shows error message
- **AND** system prompts user to select different location

### Requirement: Backup progress indication
The system SHALL show backup progress to user.

#### Scenario: Show progress bar
- **WHEN** backup is in progress
- **THEN** system displays progress bar
- **AND** system updates percentage
- **AND** system shows estimated time remaining

#### Scenario: Backup complete
- **WHEN** backup finishes successfully
- **THEN** system shows success message
- **AND** system includes backup file name
- **AND** system creates completion notification

### Requirement: Backup before cleanup
The system SHALL create backup before database maintenance.

#### Scenario: Backup before vacuum
- **WHEN** system performs database VACUUM
- **THEN** system creates backup first
- **AND** system ensures data safety

#### Scenario: Backup before index rebuild
- **WHEN** system rebuilds database indexes
- **THEN** system creates backup first
- **AND** system allows recovery if rebuild fails

### Requirement: Backup file naming
The system SHALL use consistent backup file naming.

#### Scenario: Timestamped backup name
- **WHEN** system creates backup
- **THEN** system uses format: todolist_backup_YYYYMMDD_HHMMSS.db
- **AND** system ensures unique filenames

#### Scenario: Identify latest backup
- **WHEN** system lists backups
- **THEN** system identifies most recent by timestamp
- **AND** system sorts backups chronologically

### Requirement: Backup on exit
The system SHALL optionally backup when application closes.

#### Scenario: Backup on exit enabled
- **WHEN** user closes application
- **AND** backup on exit is enabled
- **THEN** system performs backup before exit
- **AND** system shows backup progress

#### Scenario: Backup on exit disabled
- **WHEN** user closes application
- **AND** backup on exit is disabled
- **THEN** system closes without backup

### Requirement: Backup failure handling
The system SHALL handle backup failures gracefully.

#### Scenario: Disk full error
- **WHEN** backup fails due to insufficient disk space
- **THEN** system shows error message
- **AND** system creates error notification
- **AND** system does not delete previous backup

#### Scenario: File permission error
- **WHEN** backup fails due to permissions
- **THEN** system shows error message
- **AND** system suggests alternative location
- **AND** system logs error details

#### Scenario: Database locked error
- **WHEN** backup fails due to database lock
- **THEN** system shows error message
- **AND** system retries after short delay
- **AND** system logs retry attempts

### Requirement: Restore from backup
The system SHALL allow users to restore from backup files.

#### Scenario: Select backup to restore
- **WHEN** user clicks "Restore from Backup"
- **THEN** system shows list of available backups
- **AND** system displays backup dates and sizes

#### Scenario: Restore backup
- **WHEN** user selects backup file to restore
- **THEN** system shows confirmation dialog
- **WHEN** user confirms
- **THEN** system closes database
- **AND** system copies backup to main database location
- **AND** system restarts application

#### Scenario: Restore validation
- **WHEN** user selects invalid backup file
- **THEN** system validates file is SQLite database
- **AND** system shows error if invalid
- **AND** system prevents restore operation

### Requirement: Backup history view
The system SHALL display backup history.

#### Scenario: View backup list
- **WHEN** user opens backup settings
- **THEN** system displays list of all backups
- **AND** system shows creation date, size, and type

#### Scenario: Delete old backup
- **WHEN** user clicks "Delete" on backup file
- **THEN** system shows confirmation dialog
- **WHEN** user confirms
- **THEN** system deletes backup file
- **AND** system updates backup list
