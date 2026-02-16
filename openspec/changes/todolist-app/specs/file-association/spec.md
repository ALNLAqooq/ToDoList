## ADDED Requirements

### Requirement: Associate file with task
The system SHALL allow users to associate files with tasks.

#### Scenario: Add file association
- **WHEN** user clicks "Add File" in task dialog
- **THEN** system opens file selection dialog
- **WHEN** user selects file
- **THEN** system saves file path to database
- **AND** system displays file icon on task card

#### Scenario: Remove file association
- **WHEN** user clicks "Remove File" in task dialog
- **THEN** system clears file path from database
- **AND** system removes file icon from task card

### Requirement: Open associated file
The system SHALL allow users to open associated files with system default application.

#### Scenario: Open file from task card
- **WHEN** user clicks on file icon in task card
- **THEN** system opens file with default system application
- **AND** system uses QDesktopServices::openUrl()

#### Scenario: Open file from edit dialog
- **WHEN** user clicks "Open File" in task edit dialog
- **THEN** system opens file with default system application

### Requirement: Display file metadata
The system SHALL display file information on task card.

#### Scenario: Show file name
- **WHEN** task has associated file
- **THEN** system displays file name on task card

#### Scenario: Show file type
- **WHEN** task has associated file
- **THEN** system displays appropriate file type icon

### Requirement: Validate file path
The system SHALL validate that associated file exists.

#### Scenario: File exists
- **WHEN** task has valid file path
- **THEN** system displays file normally

#### Scenario: File not found
- **WHEN** task has associated file that does not exist
- **THEN** system shows warning indicator
- **AND** system offers to reselect or remove file

#### Scenario: File path update
- **WHEN** user reselects file for missing file
- **THEN** system updates file path in database
- **AND** system removes warning indicator

### Requirement: Support multiple file types
The system SHALL support association of any file type.

#### Scenario: Associate document file
- **WHEN** user selects Word document
- **THEN** system saves file path
- **AND** system opens with Word application

#### Scenario: Associate spreadsheet file
- **WHEN** user selects Excel file
- **THEN** system saves file path
- **AND** system opens with Excel application

#### Scenario: Associate other file types
- **WHEN** user selects any file type
- **THEN** system saves file path
- **AND** system opens with system default handler

### Requirement: Handle file path cross-platform
The system SHALL handle file paths correctly across platforms.

#### Scenario: Windows path handling
- **WHEN** running on Windows
- **THEN** system uses QDir for path operations
- **AND** system handles backslash separators correctly

#### Scenario: Relative path support
- **WHEN** file is in data directory
- **THEN** system may store relative path
- **AND** system resolves to absolute path when opening
