## ADDED Requirements

### Requirement: Create hierarchical tasks
The system SHALL allow users to create tasks with unlimited nesting levels.

#### Scenario: Create root task
- **WHEN** user creates new task without selecting parent
- **THEN** system creates task with parent_id as NULL
- **AND** system displays task at root level

#### Scenario: Create child task
- **WHEN** user creates new task and selects parent task
- **THEN** system creates task with parent_id set to selected parent
- **AND** system displays child task nested under parent

### Requirement: Distinguish task vs step
The system SHALL allow users to mark tasks as either "task" or "step".

#### Scenario: Create task type
- **WHEN** user creates new item
- **THEN** system allows user to select type (task or step)
- **AND** system saves is_step flag in database

#### Scenario: Task type behavior
- **WHEN** user creates task
- **THEN** task has independent attributes (deadline, description, tags)
- **WHEN** user creates step
- **THEN** step may have simplified attributes (based on user setting)

### Requirement: Expand and collapse hierarchy
The system SHALL allow users to expand and collapse task hierarchy.

#### Scenario: Expand parent task
- **WHEN** user clicks expand indicator on parent task
- **THEN** system displays child tasks
- **AND** system loads child tasks from database if not already loaded

#### Scenario: Collapse parent task
- **WHEN** user clicks collapse indicator on parent task
- **THEN** system hides child tasks
- **AND** system maintains parent task visibility

#### Scenario: Expand all
- **WHEN** user selects "Expand All" option
- **THEN** system expands all tasks in current view

#### Scenario: Collapse all
- **WHEN** user selects "Collapse All" option
- **THEN** system collapses all tasks in current view

### Requirement: Lazy load child tasks
The system SHALL load child tasks only when parent is expanded.

#### Scenario: Initial load
- **WHEN** application starts
- **THEN** system loads only root tasks
- **AND** system marks parent tasks with hasChildren indicator

#### Scenario: Expand triggers load
- **WHEN** user expands parent task
- **THEN** system queries database for child tasks
- **AND** system caches child tasks in memory

### Requirement: Display hierarchy depth
The system SHALL visually indicate hierarchy depth through indentation.

#### Scenario: Nested task display
- **WHEN** system displays task hierarchy
- **THEN** system indents child tasks relative to parent
- **AND** indentation increases with each nesting level

#### Scenario: Visual hierarchy limits
- **WHEN** task hierarchy exceeds 5 levels
- **THEN** system maintains indentation
- **AND** system may use visual aids (guides) for deep levels

### Requirement: Navigate hierarchy
The system SHALL allow users to navigate task hierarchy efficiently.

#### Scenario: Keyboard navigation
- **WHEN** user uses arrow keys
- **THEN** system moves selection through hierarchy
- **AND** system respects expand/collapse state

#### Scenario: Jump to parent
- **WHEN** user activates "Jump to Parent" shortcut
- **THEN** system selects parent task
- **AND** system scrolls to ensure parent is visible

### Requirement: Count child tasks
The system SHALL display count of child tasks for parent tasks.

#### Scenario: Display child count
- **WHEN** system displays parent task
- **THEN** system shows count of direct children
- **AND** system shows count of completed children

#### Scenario: Count updates
- **WHEN** user adds or removes child task
- **THEN** system updates child count display
- **AND** system updates completed count display

### Requirement: Recursive task queries
The system SHALL support recursive queries for task hierarchy.

#### Scenario: Query task tree
- **WHEN** system needs to load full task tree
- **THEN** system uses WITH RECURSIVE SQL query
- **AND** system returns tasks in hierarchical order

#### Scenario: Query subtree
- **WHEN** user selects parent task for operations
- **THEN** system can query all descendants recursively
- **AND** system maintains hierarchy relationships
