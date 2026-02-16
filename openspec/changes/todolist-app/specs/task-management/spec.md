## ADDED Requirements

### Requirement: Create task
The system SHALL allow users to create new tasks with title, description, priority, deadline, tags, and progress.

#### Scenario: Create task with all fields
- **WHEN** user clicks "New Task" button
- **THEN** system opens task creation dialog
- **WHEN** user enters task details and saves
- **THEN** system creates task with provided details
- **AND** system saves task to database
- **AND** system displays task in task list

#### Scenario: Create task with minimal fields
- **WHEN** user creates task with only title
- **THEN** system creates task with default values (priority: low, progress: 0%, no deadline, no tags)

### Requirement: Edit task
The system SHALL allow users to edit existing task details.

#### Scenario: Edit task details
- **WHEN** user clicks edit button on a task
- **THEN** system opens task edit dialog with current task details
- **WHEN** user modifies details and saves
- **THEN** system updates task in database
- **AND** system refreshes task display with new details

#### Scenario: Edit task with dependencies
- **WHEN** user edits task that has dependencies
- **THEN** system displays task dependencies in edit dialog
- **AND** system allows user to modify dependencies

### Requirement: Delete task
The system SHALL allow users to delete tasks with confirmation and optional child handling.

#### Scenario: Delete task without children
- **WHEN** user clicks delete button on a task with no children
- **THEN** system deletes task from database
- **AND** system removes task from display

#### Scenario: Delete task with children
- **WHEN** user clicks delete button on a task with children
- **THEN** system shows confirmation dialog with child count
- **WHEN** user confirms deletion
- **THEN** system performs deletion based on user setting (cascade or promote children)
- **AND** system moves deleted task(s) to recycle bin

### Requirement: Mark task completion
The system SHALL allow users to mark tasks as completed or incomplete.

#### Scenario: Mark task as completed
- **WHEN** user clicks completion checkbox on a task
- **THEN** system sets task completed status to true
- **AND** system updates task progress to 100%
- **AND** system visually marks task as completed

#### Scenario: Mark task as incomplete
- **WHEN** user unchecks completion checkbox on a completed task
- **THEN** system sets task completed status to false
- **AND** system visually marks task as incomplete

### Requirement: Set task priority
The system SHALL allow users to set task priority as high, medium, or low.

#### Scenario: Set task priority
- **WHEN** user selects priority level in task dialog
- **THEN** system saves priority level to database
- **AND** system displays priority indicator on task card

#### Scenario: Filter by priority
- **WHEN** user selects priority filter
- **THEN** system displays only tasks with selected priority

### Requirement: Set task deadline
The system SHALL allow users to set task deadline date and time.

#### Scenario: Set task deadline
- **WHEN** user selects deadline date and time in task dialog
- **THEN** system saves deadline to database
- **AND** system displays deadline on task card

#### Scenario: Task nearing deadline
- **WHEN** current time is within configured reminder time before deadline
- **THEN** system creates notification for approaching deadline

### Requirement: Add task description
The system SHALL allow users to add detailed description to tasks.

#### Scenario: Add description
- **WHEN** user enters description in task dialog
- **THEN** system saves description to database
- **AND** system displays description on task card or in detail view

#### Scenario: Long description
- **WHEN** user enters description longer than 1KB
- **THEN** system stores full description in database
- **AND** system truncates display with "Read more" link

### Requirement: Manage task tags
The system SHALL allow users to add and remove tags to organize tasks.

#### Scenario: Add tags to task
- **WHEN** user adds tags to task in task dialog
- **THEN** system saves tags to database
- **AND** system displays tags on task card

#### Scenario: Remove tags from task
- **WHEN** user removes tag from task
- **THEN** system removes tag from database
- **AND** system updates task card display

#### Scenario: Filter by tags
- **WHEN** user selects tag filter
- **THEN** system displays only tasks with selected tag

### Requirement: Track task progress
The system SHALL allow users to set task progress percentage (0-100%).

#### Scenario: Set task progress
- **WHEN** user sets progress percentage in task dialog
- **THEN** system saves progress to database
- **AND** system displays progress bar on task card

#### Scenario: Auto-calculate parent progress
- **WHEN** user updates child task completion status
- **THEN** system recalculates parent task progress based on children
