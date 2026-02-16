## Why

创建一个基于 Qt 5.15.2 的个人任务管理桌面应用，用于日常任务管理。当前缺乏一个支持无限层级子任务、任务依赖、文件关联等高级功能的本地化任务管理工具，现有的简单应用无法满足复杂任务组织的需要。现在开发是因为有充足的个人时间和资源，可以构建一个功能完整、用户体验良好的个人任务管理系统。

## What Changes

创建一个全新的 Windows 桌面应用，包含以下功能：

**Phase 1: 核心 MVP**
- 任务基础 CRUD 操作（添加、编辑、删除、标记完成）
- 任务属性：优先级（高/中/低）、截止日期、描述
- 无限层级子任务结构（支持递归 parent_id）
- 树形列表视图，支持展开/折叠

**Phase 2: 增强功能**
- 侧边栏分组：时间（今天/本周/本月/已过期）、优先级、状态、标签
- 搜索筛选：实时搜索、高级搜索、下拉菜单筛选
- 深色/浅色主题切换
- 任务依赖显示（平级显示，循环依赖处理）
- 文件关联（存储文件路径，系统默认程序打开）
- 任务进度跟踪（0-100%）

**Phase 3: 高级功能**
- 软删除回收站（14天自动清理）
- 通知系统（小铃铛 + 站内消息）
- 自动备份（可配置频率、时间、保留数）
- 自定义文件夹（手动拖拽关联任务）
- 数据导入导出（JSON + SQLite 格式）
- 完整设置系统（8个分类：通用/外观/通知/备份/数据/删除/快捷键/关于）

## Capabilities

### New Capabilities
- `task-management`: 核心任务管理，包括 CRUD 操作、优先级、截止日期、描述、标签、完成状态、进度跟踪
- `task-hierarchy`: 无限层级子任务结构，支持任务/步骤两种类型，树形展示，展开折叠
- `task-dependencies`: 任务依赖关系管理，平级显示依赖，循环依赖检测
- `file-association`: 任务关联文件功能，存储文件路径，系统默认程序打开
- `task-search-filter`: 任务搜索和筛选，实时搜索、高级搜索、多维度筛选（优先级、状态、日期、标签）
- `sidebar-grouping`: 侧边栏分组功能，时间分组、优先级分组、状态分组、标签分组、自定义文件夹
- `notification-system`: 通知系统，小铃铛图标、站内消息、未读红点、多种通知类型
- `recycle-bin`: 软删除回收站，子任务提升为根任务（可配置），14天自动清理，3天前通知
- `backup-system`: 自动备份系统，可配置频率/时间/保留数/路径，备份进度显示
- `data-import-export`: 数据导入导出，JSON 格式（可读性好）、SQLite 格式（完整备份），合并/覆盖/追加导入
- `theme-system`: 主题系统，深色/浅色模式切换，蓝色主题色，QSS 样式表
- `settings-system`: 设置系统，8个分类（通用/外观/通知/备份/数据/删除/快捷键/关于），配置持久化
- `logging-system`: 日志系统，多级别日志（DEBUG/INFO/WARNING/ERROR/CRITICAL），日志轮转，设置中可配置

### Modified Capabilities
（无现有能力修改，全新项目）

## Impact

**技术栈:**
- Qt 5.15.2 (LTS)
- 编译器: MSVC 2019 64位
- 数据库: SQLite (QtSql 模块)
- 样式: QSS (Qt Style Sheets)
- 图标: SVG 格式

**架构:**
- MVC 架构：models（数据模型）、views（UI组件）、controllers（业务逻辑）
- 数据层：Database 类管理 SQLite 连接和查询
- 工具类：Logger（日志）、DateUtils（日期工具）、FileUtils（文件工具）、ThemeUtils（主题工具）

**项目结构:**
```
ToDoList/
├── CMakeLists.txt
├── src/
│   ├── main.cpp
│   ├── app.h/cpp
│   ├── models/ (task, taskmodel, notification, folder)
│   ├── views/ (mainwindow, sidebar, tasktree, taskcard, dialogs)
│   ├── controllers/ (taskcontroller, database, backupmanager, notificationmanager)
│   ├── utils/ (logger, dateutils, fileutils, themeutils)
│   └── styles/ (dark.qss, light.qss)
├── resources/icons/*.svg
├── data/todolist.db
├── backup/
├── logs/
└── docs/
```

**数据库设计:**
- 6张表：tasks, task_dependencies, notifications, folders, folder_tasks, settings
- 支持递归查询子任务树（WITH RECURSIVE）
- FTS5 全文搜索索引
- 多个索引优化查询性能

**部署:**
- 自包含 Release 文件夹，使用 windeployqt 打包
- 无第三方依赖（SQLite Qt 内置）
- 无需安装，直接运行 exe
