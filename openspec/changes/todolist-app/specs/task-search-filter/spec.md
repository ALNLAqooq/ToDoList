## ADDED Requirements

### Requirement: Real-time search
The system SHALL provide real-time search as user types.

#### Scenario: Start typing search
- **WHEN** user starts typing in search box
- **THEN** system filters task list immediately
- **AND** system updates results as user types

#### Scenario: Clear search
- **WHEN** user clears search box
- **THEN** system displays all tasks in current view
- **AND** system removes search filter

### Requirement: Search multiple fields
The system SHALL search across title, description, and tags.

#### Scenario: Search by title
- **WHEN** user searches for text that matches task title
- **THEN** system returns tasks with matching title

#### Scenario: Search by description
- **WHEN** user searches for text that matches task description
- **THEN** system returns tasks with matching description

#### Scenario: Search by tags
- **WHEN** user searches for tag name
- **THEN** system returns tasks with matching tags

#### Scenario: Combined search
- **WHEN** search term matches multiple fields
- **THEN** system returns tasks matching any field

### Requirement: Advanced search
The system SHALL provide advanced search with multiple filters.

#### Scenario: Open advanced search
- **WHEN** user clicks "Advanced" button in search
- **THEN** system expands advanced search panel
- **AND** system shows filter fields (priority, status, date, tags)

#### Scenario: Apply advanced filters
- **WHEN** user sets multiple filters and searches
- **THEN** system applies all filters
- **AND** system returns tasks matching all criteria

#### Scenario: Reset advanced search
- **WHEN** user clicks "Reset" button
- **THEN** system clears all filters
- **AND** system displays all tasks

### Requirement: Filter by priority
The system SHALL allow filtering tasks by priority level.

#### Scenario: Filter high priority
- **WHEN** user selects "High" priority filter
- **THEN** system displays only high priority tasks
- **AND** system updates task count

#### Scenario: Filter multiple priorities
- **WHEN** user selects multiple priority filters
- **THEN** system displays tasks with any selected priority
- **AND** system shows count for each priority

#### Scenario: Clear priority filter
- **WHEN** user deselects all priority filters
- **THEN** system displays all tasks regardless of priority

### Requirement: Filter by status
The system SHALL allow filtering tasks by completion status.

#### Scenario: Filter completed tasks
- **WHEN** user selects "Completed" status filter
- **THEN** system displays only completed tasks

#### Scenario: Filter incomplete tasks
- **WHEN** user selects "Incomplete" status filter
- **THEN** system displays only incomplete tasks

#### Scenario: Filter by progress
- **WHEN** user selects progress range filter
- **THEN** system displays tasks within progress range

### Requirement: Filter by date
The system SHALL allow filtering tasks by deadline date.

#### Scenario: Filter today's tasks
- **WHEN** user selects "Today" date filter
- **THEN** system displays tasks due today
- **AND** system compares current date with task deadline

#### Scenario: Filter this week's tasks
- **WHEN** user selects "This Week" date filter
- **THEN** system displays tasks due this week
- **AND** system calculates week based on current date

#### Scenario: Filter overdue tasks
- **WHEN** user selects "Overdue" date filter
- **THEN** system displays tasks with past deadlines
- **AND** system highlights overdue tasks

#### Scenario: Filter by date range
- **WHEN** user selects custom date range
- **THEN** system displays tasks with deadlines in range
- **AND** system validates date range

### Requirement: Filter by tags
The system SHALL allow filtering tasks by tags.

#### Scenario: Filter by single tag
- **WHEN** user clicks on tag in sidebar
- **THEN** system displays tasks with selected tag
- **AND** system highlights active tag filter

#### Scenario: Filter by multiple tags
- **WHEN** user selects multiple tags
- **THEN** system displays tasks with any selected tag
- **AND** system shows count for each tag

#### Scenario: Create new tag filter
- **WHEN** user adds new tag in filter panel
- **THEN** system applies new tag filter
- **AND** system saves filter state

### Requirement: Sort tasks
The system SHALL allow sorting tasks by various criteria.

#### Scenario: Sort by deadline
- **WHEN** user selects "Deadline" sort
- **THEN** system sorts tasks by deadline date
- **AND** system supports ascending and descending order

#### Scenario: Sort by priority
- **WHEN** user selects "Priority" sort
- **THEN** system sorts tasks by priority level
- **AND** system orders high priority first

#### Scenario: Sort by creation date
- **WHEN** user selects "Created" sort
- **THEN** system sorts tasks by creation timestamp
- **AND** system shows newest first by default

#### Scenario: Manual sort
- **WHEN** user selects "Manual" sort
- **THEN** system uses sort_order field
- **AND** system allows drag-to-reorder

### Requirement: Search syntax
The system SHALL support advanced search syntax in search box.

#### Scenario: Tag search syntax
- **WHEN** user types "tag:work"
- **THEN** system filters tasks with "work" tag
- **AND** system recognizes "tag:" prefix

#### Scenario: Priority search syntax
- **WHEN** user types "priority:high"
- **THEN** system filters high priority tasks
- **AND** system recognizes "priority:" prefix

#### Scenario: Combined syntax
- **WHEN** user types "tag:work priority:high report"
- **THEN** system applies all filters
- **AND** system searches for "report" within filtered tasks

### Requirement: Display search results
The system SHALL clearly indicate search results and filters.

#### Scenario: Show result count
- **WHEN** search returns results
- **THEN** system displays result count
- **AND** system shows "X tasks found"

#### Scenario: Show active filters
- **WHEN** multiple filters are active
- **THEN** system displays all active filters
- **AND** system allows clearing all filters

#### Scenario: No results
- **WHEN** search returns no results
- **THEN** system displays empty state message
- **AND** system suggests clearing filters
