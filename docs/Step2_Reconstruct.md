将 MainWindow 的 UI 构建逻辑从 C++ 代码移至 .ui 文件
你希望将 `XVisual` 的 UI 从硬编码方式转换为使用 Qt 的 `.ui` 文件维护。

### 转换计划

这是一个涉及重构的计划，旨在将 `MainWindow` 的 UI 构建逻辑从 C++ 代码移至 `.ui` 文件。

**步骤:**

1.  **创建 MainWindow.ui**:
    *   我将根据 `mainwindow.cpp` 中现有的布局和控件代码，创建一个对应的 XML 格式的 `.ui` 文件。
    *   包括：菜单栏、工具栏、`QSplitter` 布局、`QToolBox` (作为占位符或完全重构)、`GraphicsWidget` (作为自定义控件占位符) 和 `SideWidget` (作为自定义控件占位符)。
    *   资源路径将引用 `XVisual.qrc` 中定义的资源。

2.  **更新项目配置 (XVisual.vcxproj)**:
    *   添加 `mainwindow.ui` 到项目文件中。
    *   配置 Qt 的构建工具 (uic) 来处理该文件。

3.  **重构 MainWindow.h**:
    *   引入 `ui_mainwindow.h`。
    *   在 `MainWindow` 类中添加 `Ui::MainWindow *ui;` 指针。
    *   移除手动创建控件的成员变量（如 `toolBox`, `graphicsWidget` 等，如果它们在 UI 文件中定义）。

4.  **重构 MainWindow.cpp**:
    *   在构造函数中调用 `ui->setupUi(this)`。
    *   移除 `createActions`, `createMenus`, `createToolbars`, `createToolBox` 等手动创建 UI 的代码，改为从 `ui` 指针访问控件。
    *   更新信号槽连接，使用 UI 文件中定义的控件名称。

### 详细 UI 结构分析 (基于代码)

*   **Central Widget**: 包含一个水平 `QSplitter`。
    *   **左**: `QToolBox` (Item Box)。
    *   **中**: `GraphicsWidget` (自定义控件)。
    *   **右**: `SideWidget` (自定义控件)。
*   **Menu Bar**: File, Item, Settings, Help.
*   **Tool Bars**: Edit, Font, Color, Scale, linePointer, Run, Export, Load.

### 下一步行动

在 PLAN MODE 下，我需要先仔细检查 `mainwindow.cpp` 中创建 `QToolBox` 的逻辑，因为动态创建的部分可能需要保留在代码中，或者在 `.ui` 中创建一个空的容器。

我将从创建 `XVisual/forms/MainWindow.ui` 开始（通常放在 `forms` 或 `ui` 文件夹，但考虑到现有结构，可能直接放在 `src/MainWindow` 或根目录，我将检查是否有 `forms` 目录，如果没有则创建）。

然后我将修改 `XVisual.vcxproj`。

你是否同意这个计划？