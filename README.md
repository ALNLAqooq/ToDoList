# ToDoList

基于 Qt 5.15.2 + C++17 的桌面任务管理工具，聚焦层级任务、搜索筛选、提醒与备份。

## 功能亮点

- 任务管理：增删改查、完成状态、优先级、截止时间
- 层级与步骤：父子任务、子步骤、自动进度汇总
- 依赖关系：依赖选择与循环依赖检测
- 标签与文件：标签管理、文件关联与类型图标
- 视图与交互：侧边栏分组、任务树、卡片列表、拖拽排序
- 搜索筛选：FTS5 全文检索，按日期/状态/优先级/标签过滤
- 通知与回收站：通知中心、软删除、恢复、自动清理
- 备份：手动/定时备份、保留策略、恢复
- 主题与设置：深色/浅色主题，外观/通知/备份/数据/快捷键等设置

## 环境要求

- CMake 3.16+
- Qt 5.15.2（Core/Gui/Widgets/Sql/Svg）
- C++17 编译器（建议 MSVC 2019 64-bit 或更新）

## 构建与运行

1. 克隆或下载项目
2. 生成构建目录并编译

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

运行应用：

```bash
# Windows
.\ToDoList.exe

# Linux/macOS
./ToDoList
```

提示：应用会在当前工作目录下创建 `data/`、`backup/`、`logs/` 等目录。若使用 IDE 运行，请确认工作目录指向项目根目录，或自行在运行目录下准备这些目录。

## 快捷键

- Ctrl+N：新建任务
- Ctrl+F：搜索
- Ctrl+S：保存
- Ctrl+T：切换主题

## 数据与目录

- `data/todolist.db`：SQLite 数据库
- `backup/`：备份文件
- `logs/`：日志文件

以上路径均相对于运行时的当前工作目录。

## 数据库表

- `tasks`
- `task_steps`
- `tags`
- `task_tags`
- `task_dependencies`
- `task_files`
- `folders`
- `task_folders`
- `notifications`
- `settings`
- `backup_history`
- `tasks_fts`（FTS5）

## 项目结构

```text
ToDoList/
  CMakeLists.txt
  src/
    app.*
    controllers/
    models/
    utils/
    views/
  resources/
    icons/
    styles/
    icons.qrc
    styles.qrc
  data/
  backup/
  logs/
  docs/
  openspec/
```

## Roadmap

- 数据导入导出（JSON/SQLite）
- 空状态与错误提示完善
- 打包发布与清理流程
- 完整测试与性能验证

## 许可

本项目仅供学习与个人使用。

## 更新日志

### v1.0.0 (2026-02-15)

- 初始版本
- 完成基础任务管理功能
- 深色/浅色主题切换
- 搜索与筛选
- 任务卡片展示
- 适配 Qt 5.15.2
