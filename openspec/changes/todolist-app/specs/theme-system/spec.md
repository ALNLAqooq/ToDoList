## ADDED Requirements

### Requirement: Support dark and light themes
The system SHALL provide dark and light color themes.

#### Scenario: Default dark theme
- **WHEN** application starts for first time
- **THEN** system applies dark theme
- **AND** system saves theme preference

#### Scenario: Light theme selection
- **WHEN** user selects light theme in settings
- **THEN** system applies light theme
- **AND** system saves theme preference

### Requirement: Theme switching
The system SHALL allow users to switch between themes at runtime.

#### Scenario: Switch to dark theme
- **WHEN** user selects dark theme
- **THEN** system loads dark.qss stylesheet
- **AND** system updates all UI elements
- **AND** system updates window background

#### Scenario: Switch to light theme
- **WHEN** user selects light theme
- **THEN** system loads light.qss stylesheet
- **AND** system updates all UI elements
- **AND** system updates window background

### Requirement: Remember theme preference
The system SHALL save and restore user's theme choice.

#### Scenario: Save theme on exit
- **WHEN** user closes application
- **THEN** system saves theme preference to settings
- **AND** system persists across sessions

#### Scenario: Restore theme on startup
- **WHEN** application starts
- **THEN** system loads saved theme preference
- **AND** system applies saved theme

### Requirement: Follow system theme
The system SHALL optionally follow system theme setting.

#### Scenario: Follow system theme enabled
- **WHEN** user enables "Follow system theme"
- **AND** system theme changes
- **THEN** application switches to matching theme
- **AND** system updates preference

#### Scenario: Follow system theme disabled
- **WHEN** user disables "Follow system theme"
- **THEN** system uses manually selected theme
- **AND** system ignores system theme changes

### Requirement: Primary color scheme
The system SHALL use blue as primary color across themes.

#### Scenario: Dark theme primary color
- **WHEN** dark theme is active
- **THEN** system uses #3B82F6 for primary elements
- **AND** system uses #2563EB for hover states
- **AND** system uses #60A5FA for disabled states

#### Scenario: Light theme primary color
- **WHEN** light theme is active
- **THEN** system uses #2563EB for primary elements
- **AND** system uses #1D4ED8 for hover states
- **AND** system uses #60A5FA for disabled states

### Requirement: Priority color indicators
The system SHALL use consistent colors for task priorities.

#### Scenario: High priority color
- **WHEN** task has high priority
- **THEN** system uses red (#EF4444)
- **AND** system displays priority indicator

#### Scenario: Medium priority color
- **WHEN** task has medium priority
- **THEN** system uses amber (#F59E0B)
- **AND** system displays priority indicator

#### Scenario: Low priority color
- **WHEN** task has low priority
- **THEN** system uses green (#10B981)
- **AND** system displays priority indicator

### Requirement: Background colors
The system SHALL use appropriate background colors for each theme.

#### Scenario: Dark theme backgrounds
- **WHEN** dark theme is active
- **THEN** system uses slate-900 (#0F172A) for main background
- **AND** system uses slate-800 (#1E293B) for secondary background
- **AND** system uses slate-700 (#334155) for card hover

#### Scenario: Light theme backgrounds
- **WHEN** light theme is active
- **THEN** system uses slate-50 (#F8FAFC) for main background
- **AND** system uses white (#FFFFFF) for secondary background
- **AND** system uses slate-100 (#F1F5F9) for card hover

### Requirement: Text colors
The system SHALL use appropriate text colors for each theme.

#### Scenario: Dark theme text
- **WHEN** dark theme is active
- **THEN** system uses slate-50 (#F8FAFC) for primary text
- **AND** system uses slate-400 (#94A3B8) for secondary text
- **AND** system uses slate-500 (#64748B) for disabled text

#### Scenario: Light theme text
- **WHEN** light theme is active
- **THEN** system uses slate-900 (#0F172A) for primary text
- **AND** system uses slate-500 (#64748B) for secondary text
- **AND** system uses slate-400 (#94A3B8) for disabled text

### Requirement: Card styling
The system SHALL apply consistent card styling across themes.

#### Scenario: Dark theme card
- **WHEN** dark theme is active
- **THEN** task cards use slate-800 (#1E293B) background
- **AND** cards have slate-700 (#334155) border
- **AND** cards have rounded corners

#### Scenario: Light theme card
- **WHEN** light theme is active
- **THEN** task cards use white (#FFFFFF) background
- **AND** cards have slate-200 (#E2E8F0) border
- **AND** cards have rounded corners

### Requirement: QSS stylesheet structure
The system SHALL maintain separate QSS files for each theme.

#### Scenario: Dark theme file
- **WHEN** dark theme is loaded
- **THEN** system reads dark.qss
- **AND** system applies styles to all widgets

#### Scenario: Light theme file
- **WHEN** light theme is loaded
- **THEN** system reads light.qss
- **AND** system applies styles to all widgets

#### Scenario: Style reusability
- **WHEN** themes share style rules
- **THEN** system uses common selectors
- **AND** system maintains consistent structure

### Requirement: Theme transition animation
The system SHALL provide smooth transition when switching themes.

#### Scenario: Fade to new theme
- **WHEN** user switches themes
- **THEN** system fades current theme out
- **AND** system fades new theme in
- **AND** transition duration is 300ms

### Requirement: Icon colors
The system SHALL adapt icon colors to current theme.

#### Scenario: Dark theme icons
- **WHEN** dark theme is active
- **THEN** system uses light-colored icons
- **AND** icons have sufficient contrast

#### Scenario: Light theme icons
- **WHEN** light theme is active
- **THEN** system uses dark-colored icons
- **AND** icons have sufficient contrast
