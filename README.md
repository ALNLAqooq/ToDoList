# ToDoList

基于 Qt5 和 C++ 的个人任务管理工具

## 项目特点

- 使用 Qt5.15.2 + C++17 开发
- SQLite 数据库存储
- 支持深色/浅色主题切换
- 简约的卡片式 UI 设计
- 支持任务优先级、截止日期、描述等功能
- 支持多层级子任务
- 支持任务依赖关系
- 支持文件关联
- 任务进度跟踪

## 项目结构

```
ToDoList/
├── CMakeLists.txt           # CMake 构建配置文件
├── src/
│   ├── main.cpp             # 程序入口
│   ├── app/                 # 应用程序初始化
│   │   ├── app.h
│   │   └── app.cpp
│   ├── models/              # 数据模型
│   │   ├── task.h
│   │   ├── task.cpp
│   │   ├── tag.h
│   │   └── tag.cpp
│   ├── database/            # 数据库管理
│   │   ├── database_manager.h
│   │   └── database_manager.cpp
│   ├── controllers/         # 业务逻辑控制器
│   │   ├── task_controller.h
│   │   └── task_controller.cpp
│   ├── views/               # UI 界面
│   │   ├── main_window.h
│   │   ├── main_window.cpp
│   │   ├── task_list_widget.h
│   │   ├── task_list_widget.cpp
│   │   ├── task_card_widget.h
│   │   └── task_card_widget.cpp
│   └── utils/               # 工具类
│       ├── theme_manager.h
│       └── theme_manager.cpp
├── resources/               # 资源文件
│   ├── icons/               # 图标文件（SVG）
│   ├── styles/              # 样式文件（由 ThemeManager 动态生成）
│   └── resources.qrc       # Qt 资源文件
└── docs/                    # 文档
    └── 思路.md              # 项目规划文档
```

## 构建说明

### 环境要求

- CMake 3.16 或更高版本
- Qt 5.15.2
- C++17 支持的编译器（MSVC 2019 或更高版本）

### 构建步骤

1. 克隆或下载项目代码
2. 在 Qt Creator 中打开项目，或者使用命令行构建：

```bash
# 创建构建目录
mkdir build
cd build

# 配置 CMake
cmake ..

# 编译项目
cmake --build .

# 运行程序
./ToDoList (Linux/macOS)
ToDoList.exe (Windows)
```

### 在 Qt Creator 中构建

1. 打开 Qt Creator
2. 文件 -> 打开文件或项目 -> 选择 `CMakeLists.txt`
3. 配置项目套件（选择 MSVC 2019 + Qt 5.15.2）
4. 点击构建按钮（或按 Ctrl+B）

## 功能说明

### 已实现功能

- 基础任务管理（增删改查）
- 任务完成状态切换
- 任务优先级（高/中/低）
- 任务截止日期显示
- 任务描述显示
- 深色/浅色主题切换
- 任务搜索和筛选
- 任务卡片式展示
- 数据库自动创建和初始化

### 待实现功能

- 新建/编辑任务对话框
- 任务标签管理
- 多层级子任务
- 任务依赖关系
- 文件关联
- 任务进度计算
- 任务统计面板

## 数据库结构

项目使用 SQLite 数据库，数据库文件为 `todolist.db`，包含以下表：

- `tasks` - 任务表
- `tags` - 标签表
- `task_tags` - 任务-标签关联表
- `task_dependencies` - 任务依赖表
- `task_files` - 任务文件关联表

## 使用说明

### 基本操作

1. **新建任务**：点击工具栏的"新建任务"按钮（功能待实现）
2. **编辑任务**：点击任务卡片上的"编辑"按钮（功能待实现）
3. **删除任务**：点击任务卡片上的"删除"按钮
4. **完成任务**：勾选任务卡片前的复选框
5. **切换主题**：点击工具栏的"切换主题"按钮或使用快捷键 Ctrl+T

### 搜索任务

在搜索框中输入关键词，任务列表会实时过滤显示匹配的任务。

## 开发计划

- [ ] 实现新建任务对话框
- [ ] 实现编辑任务对话框
- [ ] 实现任务标签功能
- [ ] 实现多层级子任务
- [ ] 实现任务依赖关系
- [ ] 实现文件关联功能
- [ ] 实现任务进度自动计算
- [ ] 添加任务统计面板
- [ ] 优化 UI/UX

## 技术栈

- **框架**: Qt 5.15.2
- **语言**: C++17
- **构建工具**: CMake
- **数据库**: SQLite
- **IDE**: Qt Creator

## 许可证

本项目仅供个人学习和使用。

## 作者

GoodIdea

## 更新日志

### v1.0.0 (2026-02-15)

- 初始版本
- 实现基础任务管理功能
- 实现深色/浅色主题切换
- 实现任务搜索和筛选
- 实现任务卡片式展示
- 适配 Qt 5.15.2
