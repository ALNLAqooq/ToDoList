## ADDED Requirements

### Requirement: Define task dependencies
The system SHALL allow users to specify dependencies between tasks.

#### Scenario: Add dependency
- **WHEN** user adds dependency in task dialog
- **THEN** system saves dependency to task_dependencies table
- **AND** system validates task exists

#### Scenario: Remove dependency
- **WHEN** user removes dependency
- **THEN** system deletes dependency record
- **AND** system updates task display

### Requirement: Display task dependencies
The system SHALL display task dependencies in a clear, non-hierarchical manner.

#### Scenario: Show dependencies on task card
- **WHEN** task has dependencies
- **THEN** system displays dependency indicator on task card
- **AND** system shows list of dependent tasks

#### Scenario: Dependency detail view
- **WHEN** user clicks on dependency indicator
- **THEN** system shows full dependency information
- **AND** system allows navigation to dependent tasks

### Requirement: Detect circular dependencies
The system SHALL detect and handle circular dependencies.

#### Scenario: Circular dependency detection
- **WHEN** user adds dependency that creates circular reference
- **THEN** system detects circular dependency
- **AND** system shows warning message
- **AND** system prevents saving circular dependency

#### Scenario: Display circular dependencies
- **WHEN** circular dependencies exist in database
- **THEN** system displays tasks at same level
- **AND** system does not attempt hierarchical display

### Requirement: Validate dependency tasks
The system SHALL ensure dependency tasks exist.

#### Scenario: Dependency task exists
- **WHEN** user creates dependency to existing task
- **THEN** system saves dependency successfully

#### Scenario: Dependency task deleted
- **WHEN** dependency task is deleted
- **THEN** system removes dependency record
- **AND** system updates dependent task

### Requirement: Multiple dependencies
The system SHALL allow tasks to have multiple dependencies.

#### Scenario: Add multiple dependencies
- **WHEN** user adds multiple dependencies to task
- **THEN** system saves all dependency records
- **AND** system displays all dependencies

#### Scenario: Remove one of many dependencies
- **WHEN** user removes one dependency
- **THEN** system removes only specified dependency
- **AND** system keeps other dependencies intact

### Requirement: Dependency impact on task display
The system SHALL indicate dependency relationships in task list.

#### Scenario: Dependency indicator
- **WHEN** task has dependencies
- **THEN** system shows chain/link icon
- **AND** system shows count of dependencies

#### Scenario: No dependencies
- **WHEN** task has no dependencies
- **THEN** system does not show dependency indicator
