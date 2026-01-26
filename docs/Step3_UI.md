我已经成功将 XVisual 的主窗口 UI 从硬编码转换为使用 Qt Designer 的 `.ui` 文件维护。

**主要更改包括：**

1.  **创建了 UI 定义文件** (`XVisual/src/MainWindow/mainwindow.ui`)：
    *   定义了完整的窗口布局，包括菜单栏、工具栏、状态栏。
    *   使用 `QSplitter` 实现了左侧工具箱、中间绘图区和右侧属性栏的布局。
    *   配置了所有的 `QAction`（如删除、置顶、置底、字体样式等）及其图标和快捷键。
    *   预留了 `GraphicsWidget` 和 `SideWidget` 的提升部件位置。

2.  **更新了项目文件** (`XVisual/XVisual.vcxproj`)：
    *   添加了对 `src/MainWindow/mainwindow.ui` 的引用，确保编译时会自动生成 `ui_mainwindow.h`。

3.  **重构了 MainWindow 类**：
    *   **头文件** (`mainwindow.h`)：引入了 `Ui::MainWindow` 命名空间，移除了大量手动管理的 UI 指针（如 `toolBox`, `graphicsWidget`, `editToolBar` 等），改为使用 `ui` 指针统一管理。
    *   **实现文件** (`mainwindow.cpp`)：
        *   构造函数中调用 `ui->setupUi(this)` 初始化界面。
        *   移除了 `createActions`, `createMenus`, `createToolbars` 等手动构建 UI 的代码。
        *   将原有的逻辑（如信号槽连接、工具箱填充）适配到新的 `ui` 对象上。

4.  **增强了 GraphicsWidget 和 DiagramScene**：
    *   为了支持通过 UI 文件自动实例化 `GraphicsWidget`，为其添加了不带 `QMenu` 参数的构造函数。
    *   添加了 `setItemMenu()` 方法，允许在 UI 初始化完成后再注入上下文菜单。

现在，您可以使用 Qt Designer 直接编辑 `mainwindow.ui` 来修改界面布局，而无需深入 C++ 代码。重新编译项目后，Qt 会自动生成相应的界面代码。