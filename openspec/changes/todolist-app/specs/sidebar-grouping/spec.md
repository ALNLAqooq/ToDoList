## ADDED Requirements

### Requirement: Display sidebar with groups
The system SHALL display a sidebar with multiple grouping options.

#### Scenario: Initial sidebar display
- **WHEN** application starts
- **THEN** system displays sidebar on left side
- **AND** system shows default groups

#### Scenario: Sidebar width
- **WHEN** sidebar is displayed
- **THEN** system shows sidebar at default width (280px)
- **AND** system allows user to resize sidebar

### Requirement: Time-based grouping
The system SHALL provide time-based task groups.

#### Scenario: Today group
- **WHEN** user clicks "Today" group
- **THEN** system displays tasks due today
- **AND** system shows task count for today

#### Scenario: This week group
- **WHEN** user clicks "This Week" group
- **THEN** system displays tasks due this week
- **AND** system calculates week based on current date

#### Scenario: This month group
- **WHEN** user clicks "This Month" group
- **THEN** system displays tasks due this month
- **AND** system calculates month based on current date

#### Scenario: Overdue group
- **WHEN** user clicks "Overdue" group
- **THEN** system displays tasks with past deadlines
- **AND** system highlights overdue tasks

### Requirement: Priority-based grouping
The system SHALL provide priority-based task groups.

#### Scenario: High priority group
- **WHEN** user clicks "High" group
- **THEN** system displays high priority tasks
- **AND** system shows task count for high priority

#### Scenario: Medium priority group
- **WHEN** user clicks "Medium" group
- **THEN** system displays medium priority tasks
- **AND** system shows task count for medium priority

#### Scenario: Low priority group
- **WHEN** user clicks "Low" group
- **THEN** system displays low priority tasks
- **AND** system shows task count for low priority

### Requirement: Status-based grouping
The system SHALL provide status-based task groups.

#### Scenario: Completed group
- **WHEN** user clicks "Completed" group
- **THEN** system displays completed tasks
- **AND** system shows completed task count

#### Scenario: Incomplete group
- **WHEN** user clicks "Incomplete" group
- **THEN** system displays incomplete tasks
- **AND** system shows incomplete task count

#### Scenario: In progress group
- **WHEN** user clicks "In Progress" group
- **THEN** system displays tasks with 1-99% progress
- **AND** system shows in progress count

### Requirement: Tag-based grouping
The system SHALL provide tag-based task groups.

#### Scenario: Display tags
- **WHEN** user has tasks with tags
- **THEN** system displays tags in sidebar
- **AND** system shows task count for each tag

#### Scenario: Filter by tag
- **WHEN** user clicks on tag in sidebar
- **THEN** system filters tasks by selected tag
- **AND** system highlights active tag

#### Scenario: New tag creation
- **WHEN** user adds new tag to task
- **THEN** system adds tag to sidebar
- **AND** system updates tag counts

### Requirement: Custom folders
The system SHALL allow users to create custom folders.

#### Scenario: Create folder
- **WHEN** user clicks "New Folder" in sidebar
- **THEN** system opens folder creation dialog
- **WHEN** user enters folder name and saves
- **THEN** system creates folder in database
- **AND** system displays folder in sidebar

#### Scenario: Rename folder
- **WHEN** user right-clicks folder and selects rename
- **THEN** system opens rename dialog
- **WHEN** user enters new name and saves
- **THEN** system updates folder name

#### Scenario: Delete folder
- **WHEN** user right-clicks folder and selects delete
- **THEN** system shows confirmation dialog
- **WHEN** user confirms
- **THEN** system deletes folder
- **AND** system removes folder from sidebar

### Requirement: Drag tasks to folders
The system SHALL allow users to drag tasks to custom folders.

#### Scenario: Drag single task to folder
- **WHEN** user drags task to custom folder
- **THEN** system associates task with folder
- **AND** system updates folder task count

#### Scenario: Drag multiple tasks to folder
- **WHEN** user drags multiple tasks to custom folder
- **THEN** system associates all tasks with folder
- **AND** system updates folder task count

#### Scenario: Remove task from folder
- **WHEN** user drags task out of folder
- **THEN** system removes folder association
- **AND** system updates folder task count

### Requirement: Sidebar navigation
The system SHALL allow easy navigation between groups.

#### Scenario: Switch groups
- **WHEN** user clicks different group
- **THEN** system updates task list
- **AND** system highlights active group

#### Scenario: Keyboard navigation
- **WHEN** user uses keyboard to navigate sidebar
- **THEN** system moves selection through groups
- **AND** system allows Enter to select group

### Requirement: Collapsible sidebar
The system SHALL allow collapsing sidebar to save space.

#### Scenario: Collapse sidebar
- **WHEN** user clicks collapse button
- **THEN** system hides sidebar
- **AND** system expands task list area

#### Scenario: Expand sidebar
- **WHEN** user clicks expand button
- **THEN** system shows sidebar
- **AND** system restores previous width

#### Scenario: Remember sidebar state
- **WHEN** application restarts
- **THEN** system restores sidebar state (collapsed/expanded)
