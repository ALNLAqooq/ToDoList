## ADDED Requirements

### Requirement: Display notification bell icon
The system SHALL display a bell icon with unread count indicator.

#### Scenario: No unread notifications
- **WHEN** user has no unread notifications
- **THEN** system displays bell icon
- **AND** system does not show red dot

#### Scenario: Has unread notifications
- **WHEN** user has unread notifications
- **THEN** system displays bell icon with red dot
- **AND** system shows unread count number

#### Scenario: Clear all notifications
- **WHEN** user reads all notifications
- **THEN** system removes red dot
- **AND** system hides unread count

### Requirement: Display notification panel
The system SHALL provide a panel to view notifications.

#### Scenario: Open notification panel
- **WHEN** user clicks bell icon
- **THEN** system opens notification panel
- **AND** system displays all notifications

#### Scenario: Close notification panel
- **WHEN** user clicks outside panel
- **THEN** system closes notification panel
- **AND** system keeps notifications as read

### Requirement: Mark notification as read
The system SHALL allow users to mark notifications as read.

#### Scenario: Click notification
- **WHEN** user clicks on notification
- **THEN** system marks notification as read
- **AND** system updates unread count

#### Scenario: Mark all as read
- **WHEN** user clicks "Mark All as Read"
- **THEN** system marks all notifications as read
- **AND** system clears red dot indicator

### Requirement: Delete notification
The system SHALL allow users to delete notifications.

#### Scenario: Delete single notification
- **WHEN** user clicks delete on notification
- **THEN** system removes notification from database
- **AND** system removes from notification panel

#### Scenario: Delete all notifications
- **WHEN** user clicks "Clear All"
- **THEN** system removes all notifications
- **AND** system clears notification panel

### Requirement: Recycle bin deletion warning
The system SHALL notify users before permanent deletion from recycle bin.

#### Scenario: 3 days before deletion
- **WHEN** deleted task will be permanently deleted in 3 days
- **THEN** system creates notification with task details
- **AND** system sets notification type as "delete_warning"

#### Scenario: 1 day before deletion
- **WHEN** deleted task will be permanently deleted in 1 day
- **THEN** system creates urgent notification
- **AND** system highlights urgency

### Requirement: Task deadline reminder
The system SHALL notify users of approaching task deadlines.

#### Scenario: 1 day before deadline
- **WHEN** task deadline is in 1 day
- **THEN** system creates deadline notification
- **AND** system includes task title and time

#### Scenario: 1 hour before deadline
- **WHEN** task deadline is in 1 hour
- **THEN** system creates urgent notification
- **AND** system may use system notification (if enabled)

#### Scenario: Overdue task
- **WHEN** task deadline has passed
- **THEN** system creates overdue notification
- **AND** system highlights overdue status

### Requirement: Backup completion notification
The system SHALL notify users when backup completes.

#### Scenario: Successful backup
- **WHEN** automatic backup completes successfully
- **THEN** system creates backup notification
- **AND** system includes backup file name

#### Scenario: Backup failed
- **WHEN** automatic backup fails
- **THEN** system creates error notification
- **AND** system includes error reason

### Requirement: System messages
The system SHALL provide system-wide notifications.

#### Scenario: Version update available
- **WHEN** new version is available
- **THEN** system creates update notification
- **AND** system includes version information

#### Scenario: Feature introduction
- **WHEN** user updates to new version
- **THEN** system creates feature notification
- **AND** system explains new features

#### Scenario: Error notification
- **WHEN** system error occurs
- **THEN** system creates error notification
- **AND** system includes error details

### Requirement: Notification types
The system SHALL support different notification types with appropriate styling.

#### Scenario: Delete warning type
- **WHEN** notification is delete warning
- **THEN** system displays with warning icon
- **AND** system uses warning color

#### Scenario: Deadline type
- **WHEN** notification is deadline reminder
- **THEN** system displays with clock icon
- **AND** system uses deadline color based on urgency

#### Scenario: Success type
- **WHEN** notification is backup completion
- **THEN** system displays with checkmark icon
- **AND** system uses success color

#### Scenario: Error type
- **WHEN** notification is error
- **THEN** system displays with error icon
- **AND** system uses error color

### Requirement: Notification actions
The system SHALL provide actions for certain notification types.

#### Scenario: Delete warning action
- **WHEN** user views delete warning notification
- **THEN** system provides "Restore" and "Ignore" actions
- **WHEN** user clicks "Restore"
- **THEN** system restores task from recycle bin

#### Scenario: Deadline reminder action
- **WHEN** user views deadline notification
- **THEN** system provides "View Task" action
- **WHEN** user clicks "View Task"
- **THEN** system opens task edit dialog

### Requirement: Notification history
The system SHALL maintain notification history.

#### Scenario: View all notifications
- **WHEN** notification panel is open
- **THEN** system displays all notifications (read and unread)
- **AND** system sorts by newest first

#### Scenario: Auto-delete old notifications
- **WHEN** notification is older than 30 days
- **THEN** system deletes notification from database
- **AND** system frees storage space
