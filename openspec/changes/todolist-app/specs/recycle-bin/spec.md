## ADDED Requirements

### Requirement: Soft delete tasks
The system SHALL mark tasks as deleted instead of permanently removing them.

#### Scenario: Delete task to recycle bin
- **WHEN** user deletes task
- **THEN** system sets is_deleted flag to true
- **AND** system records deletion timestamp
- **AND** system moves task to recycle bin

#### Scenario: Delete task with children
- **WHEN** user deletes parent task with children
- **THEN** system marks parent and children as deleted
- **AND** system handles based on user setting (cascade or promote)

### Requirement: Display recycle bin
The system SHALL provide a recycle bin view for deleted tasks.

#### Scenario: View recycle bin
- **WHEN** user clicks "Recycle Bin" in sidebar
- **THEN** system displays deleted tasks
- **AND** system shows deletion dates
- **AND** system shows days until permanent deletion

#### Scenario: Empty recycle bin
- **WHEN** recycle bin has no tasks
- **THEN** system displays empty state message
- **AND** system offers to return to task list

### Requirement: Restore deleted tasks
The system SHALL allow users to restore tasks from recycle bin.

#### Scenario: Restore single task
- **WHEN** user clicks "Restore" on deleted task
- **THEN** system sets is_deleted flag to false
- **AND** system removes deletion timestamp
- **AND** system moves task back to original location

#### Scenario: Restore task with children
- **WHEN** user restores parent task with children
- **THEN** system restores parent and all children
- **AND** system maintains hierarchy

### Requirement: Permanently delete tasks
The system SHALL allow users to permanently delete tasks from recycle bin.

#### Scenario: Permanently delete task
- **WHEN** user clicks "Permanently Delete" on task
- **THEN** system shows confirmation dialog
- **WHEN** user confirms
- **THEN** system removes task from database
- **AND** system removes from recycle bin view

#### Scenario: Empty recycle bin
- **WHEN** user clicks "Empty Recycle Bin"
- **THEN** system shows confirmation dialog with task count
- **WHEN** user confirms
- **THEN** system permanently deletes all tasks in recycle bin

### Requirement: Promote children on delete
The system SHALL promote children tasks when parent is deleted (configurable).

#### Scenario: Promote children (default)
- **WHEN** user setting is "promote children"
- **AND** user deletes parent task
- **THEN** system sets children's parent_id to NULL
- **AND** system marks children as deleted
- **AND** system displays children as root tasks in recycle bin

#### Scenario: Cascade delete (alternative)
- **WHEN** user setting is "cascade delete"
- **AND** user deletes parent task
- **THEN** system marks parent and all children as deleted
- **AND** system maintains hierarchy in recycle bin

### Requirement: Auto-cleanup old deleted tasks
The system SHALL automatically delete tasks older than configured days.

#### Scenario: 14-day auto-cleanup
- **WHEN** deleted task is 14 days old
- **THEN** system permanently deletes task
- **AND** system frees database space

#### Scenario: Configurable cleanup days
- **WHEN** user changes cleanup days in settings
- **THEN** system uses new value for future cleanups
- **AND** system validates minimum value (e.g., 1 day)

### Requirement: Notify before permanent deletion
The system SHALL warn users before tasks are permanently deleted.

#### Scenario: 3 days before deletion
- **WHEN** deleted task will be permanently deleted in 3 days
- **THEN** system creates notification
- **AND** notification includes task details
- **AND** notification offers "Restore" action

#### Scenario: 1 day before deletion
- **WHEN** deleted task will be permanently deleted in 1 day
- **THEN** system creates urgent notification
- **AND** system highlights urgency

### Requirement: Recycle bin task actions
The system SHALL provide context actions for deleted tasks.

#### Scenario: View deleted task details
- **WHEN** user double-clicks deleted task
- **THEN** system opens task details (read-only)
- **AND** system shows original location

#### Scenario: Batch restore
- **WHEN** user selects multiple deleted tasks
- **AND** user clicks "Restore Selected"
- **THEN** system restores all selected tasks
- **AND** system shows success message

#### Scenario: Batch permanent delete
- **WHEN** user selects multiple deleted tasks
- **AND** user clicks "Permanently Delete Selected"
- **THEN** system shows confirmation with task count
- **WHEN** user confirms
- **THEN** system permanently deletes all selected tasks

### Requirement: Recycle bin filtering
The system SHALL allow filtering deleted tasks.

#### Scenario: Filter by deletion date
- **WHEN** user selects date filter in recycle bin
- **THEN** system displays tasks deleted in selected range

#### Scenario: Search deleted tasks
- **WHEN** user searches in recycle bin
- **THEN** system searches deleted tasks only
- **AND** system excludes active tasks from results

### Requirement: Recycle bin capacity
The system SHALL display recycle bin statistics.

#### Scenario: Show task count
- **WHEN** user views recycle bin
- **THEN** system displays total deleted task count
- **AND** system displays space used

#### Scenario: Show deletion schedule
- **WHEN** user views recycle bin
- **THEN** system shows next cleanup date
- **AND** system shows tasks approaching permanent deletion
