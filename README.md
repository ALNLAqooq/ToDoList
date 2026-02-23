# ToDoList

基于 Qt 5.15.2 + C++17 的桌面任务管理工具，聚焦层级任务、搜索筛选、提醒与备份，提供直观高效的任务管理体验。

## 功能亮点

- **任务管理**：增删改查、完成状态、优先级、截止时间设置
- **层级与步骤**：父子任务关系、子步骤管理、自动进度汇总
- **依赖关系**：任务依赖设置、循环依赖检测与提示
- **标签与文件**：多标签管理、文件关联、文件类型图标自动识别
- **视图与交互**：侧边栏分组、任务树视图、卡片列表视图、拖拽排序
- **搜索筛选**：FTS5 全文检索，按日期/状态/优先级/标签多维度过滤
- **通知与回收站**：通知中心、任务软删除、回收站恢复、自动清理
- **备份系统**：手动备份、定时自动备份、备份保留策略、快速恢复
- **主题与设置**：深色/浅色主题切换，外观/通知/备份/数据/快捷键等个性化设置

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

- `tasks`：任务主表
- `task_steps`：任务子步骤
- `tags`：标签表
- `task_tags`：任务-标签关联
- `task_dependencies`：任务依赖关系
- `task_files`：任务关联文件
- `folders`：文件夹表
- `task_folders`：任务-文件夹关联
- `notifications`：通知表
- `settings`：设置表
- `backup_history`：备份历史
- `tasks_fts`：FTS5 全文检索表

## 项目结构

```text
ToDoList/
  CMakeLists.txt          # 主构建文件
  README.md               # 项目说明
  app.rc                  # Windows 应用资源文件
  src/
    main.cpp              # 程序入口
    app.cpp/h             # 应用核心
    controllers/          # 控制器
      backupmanager.cpp/h    # 备份管理器
      database.cpp/h         # 数据库控制器
      notificationmanager.cpp/h # 通知管理器
      task_controller.cpp/h    # 任务控制器
    models/               # 数据模型
      folder.cpp/h        # 文件夹模型
      notification.cpp/h  # 通知模型
      tag.cpp/h           # 标签模型
      task.cpp/h          # 任务模型
      task_search_filters.h # 搜索过滤器
      task_step.cpp/h     # 任务步骤模型
      taskmodel.cpp/h     # 任务数据模型
    utils/                # 工具类
      date_utils.cpp/h    # 日期工具
      file_utils.cpp/h    # 文件工具
      icon_utils.cpp/h    # 图标工具
      logger.cpp/h        # 日志工具
      theme_manager.cpp/h # 主题管理器
      theme_utils.cpp/h   # 主题工具
    views/                # 视图组件
      content_area.cpp/h  # 内容区域
      empty_state_widget.cpp/h # 空状态组件
      mainwindow.cpp/h    # 主窗口
      notificationpanel.cpp/h # 通知面板
      search_widget.cpp/h # 搜索组件
      settingsdialog.cpp/h # 设置对话框
      sidebar.cpp/h       # 侧边栏
      task_card_widget.cpp/h # 任务卡片
      task_detail_widget.cpp/h # 任务详情
      task_dialog.cpp/h   # 任务对话框
      task_list_widget.cpp/h # 任务列表
      task_tree.cpp/h     # 任务树
  resources/              # 资源文件
    icons/                # 图标
      add.svg             # 添加图标
      bell.svg            # 通知图标
      check.svg           # 完成图标
      delete.svg          # 删除图标
      edit.svg            # 编辑图标
      file-*.svg          # 文件类型图标
      logo*.ico           # 应用图标
      moon.svg            # 深色主题图标
      sun.svg             # 浅色主题图标
    styles/               # 样式文件
      common.qss          # 通用样式
      dark.qss            # 深色主题
      light.qss           # 浅色主题
    icons.qrc             # 图标资源文件
    styles.qrc            # 样式资源文件
    resources.qrc         # 总资源文件
  data/                   # 数据目录
  backup/                 # 备份目录
  logs/                   # 日志目录
```

## Roadmap

- 数据导入导出（JSON/SQLite）
- 空状态与错误提示完善
- 打包发布与清理流程
- 完整测试与性能验证
- 多语言支持

## 许可

本项目仅供学习与个人使用，请勿商用倒卖！

## 更新日志

### v1.0.1 (2026-02-20)

- 完善项目结构与文档
- 优化任务卡片展示
- 改进侧边栏分组功能
- 增强文件关联与图标显示
- 修复已知问题

### v1.0.0 (2026-02-15)

- 初始版本
- 完成基础任务管理功能
- 深色/浅色主题切换
- 搜索与筛选
- 任务卡片展示
- 适配 Qt 5.15.2
