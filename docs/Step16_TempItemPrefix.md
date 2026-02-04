# Step 16: 临时 Item/Handle Temp_ 前缀机制

## 背景

在 XVisual 的 ToolBox 中，为了显示每种节点类型的图标，`createCellWidget()` 函数会临时创建一个 Item 实例，获取其 `image()` 后立即删除。这些临时创建的 Item 和 Handle 虽然不会被添加到场景，但会：

1. 生成 UUID（如 `Item_xxx`、`Handle_xxx`）
2. 调用 `ColleagueManager::getUniqueItemName()` 增加名称计数

这可能导致：
- UUID 命名空间被"污染"（虽然临时对象被删除，但它们的 UUID 已经生成）
- 用户添加第一个节点时，名称可能是 `LoadImage1` 而非 `LoadImage`（因为临时创建已经消耗了计数）

## 解决方案：临时模式标志

引入 `ItemRegistry::setTemporaryMode()` / `ItemRegistry::isTemporaryMode()` 静态 API，在创建临时对象时启用临时模式。

### 临时模式下的行为

当 `ItemRegistry::isTemporaryMode() == true` 时：

1. **UUID 前缀**：
   - `XBaseItem::createUuid()` 生成 `Temp_Item_xxx` 而非 `Item_xxx`
   - `XBaseHandle::createUuid()` 生成 `Temp_Handle_xxx` 而非 `Handle_xxx`
   - `setUuidConsistentWithItem()` 正确处理 `Temp_Item_xxx` 格式

2. **名称计数**：
   - `ColleagueManager::getUniqueItemName()` 返回 `Temp_<baseName>`，**不增加计数**

3. **全局存储**：
   - 临时对象不会被添加到 `globalItemMap`/`globalHandleMap`（因为它们不会被添加到场景）

## 修改的文件

### 1. `XVisual/include/ItemBase/ItemFactory.h`

添加临时模式 API：

```cpp
class ItemRegistry
{
public:
    // ... existing methods ...
    
    /**
     * 临时模式标志：用于区分临时创建的 Item（如 ToolBox 获取图标）
     * 
     * 当 isTemporaryMode() 为 true 时，Item/Handle 的 UUID 将使用 Temp_ 前缀：
     *   - Temp_Item_xxx
     *   - Temp_Handle_xxx
     * 
     * 使用方式：
     *   ItemRegistry::setTemporaryMode(true);
     *   auto* item = ItemRegistry::createObject(...);
     *   // 使用 item
     *   delete item;
     *   ItemRegistry::setTemporaryMode(false);
     */
    static void setTemporaryMode(bool temporary);
    static bool isTemporaryMode();

private:
    static bool temporaryMode_;
};
```

### 2. `XVisual/src/ItemBase/ItemFactory.cpp`

实现临时模式静态变量和方法：

```cpp
bool ItemRegistry::temporaryMode_ = false;

void ItemRegistry::setTemporaryMode(bool temporary)
{
    temporaryMode_ = temporary;
}

bool ItemRegistry::isTemporaryMode()
{
    return temporaryMode_;
}
```

### 3. `XVisual/src/ItemBase/XBaseItem.cpp`

在 `createUuid()` 中检查临时模式：

```cpp
void XBaseItem::createUuid()
{
    if (ItemRegistry::isTemporaryMode())
    {
        uuid = "Temp_Item_" + generateUUID();
    }
    else
    {
        uuid = "Item_" + generateUUID();
    }
}
```

### 4. `XVisual/src/HandleBase/XBaseHandle.cpp`

在 `createUuid()` 和 `setUuidConsistentWithItem()` 中处理临时模式：

```cpp
void XBaseHandle::createUuid()
{
    if (ItemRegistry::isTemporaryMode())
    {
        uuid = "Temp_Handle_" + generateUUID();
    }
    else
    {
        uuid = "Handle_" + generateUUID();
    }
}

void XBaseHandle::setUuidConsistentWithItem(std::string xitemUuid)
{
    // 处理 Temp_Item_xxx 和 Item_xxx 两种格式
    if (xitemUuid.rfind("Temp_Item_", 0) == 0)
    {
        std::string uuidPart = xitemUuid.substr(10);  // "Temp_Item_".length() == 10
        this->uuid = "Temp_Handle_" + uuidPart;
    }
    else
    {
        std::string delimiter = "_";
        std::string uuidStr2 = extractSubstrAfterDelimiter(xitemUuid, delimiter);
        this->uuid = "Handle_" + uuidStr2;
    }
}
```

### 5. `XVisual/src/GlobalStorage/ColleagueManager.cpp`

在 `getUniqueItemName()` 中，临时模式下不增加计数，并移除了原来的 `itemCount - 1` 逻辑：

**原来的设计**：
- 临时创建和正式创建共用计数器
- 用 `itemCount - 1` 来补偿临时创建占用的计数

**新设计**：
- 临时模式下直接返回 `Temp_xxx`，不增加计数
- 正式创建时从 0 开始计数
- 命名规则：`LoadImage` (第一个) -> `LoadImage1` (第二个) -> `LoadImage2` (第三个)

```cpp
std::string ColleagueManager::getUniqueItemName(const std::string& baseName)
{
    // 如果是临时模式，返回临时名称，不增加计数
    if (ItemRegistry::isTemporaryMode())
    {
        return "Temp_" + baseName;
    }
    
    int& itemCount = itemCounts[baseName];
    
    std::string uniqueItemName;
    if (itemCount == 0)
    {
        // 第一个节点：无后缀
        uniqueItemName = baseName;
    }
    else
    {
        // 后续节点：添加数字后缀
        uniqueItemName = baseName + std::to_string(itemCount);
    }
    ++itemCount;
    return uniqueItemName;
}
```

### 6. `XVisual/src/MainWindow/mainwindow.cpp`

在 `createCellWidget()` 中启用临时模式：

```cpp
QWidget* MainWindow::createCellWidget(const QString& text)
{
    idnames[itemtype] = text;
    std::string itemclass = text.toStdString();
    
    // 启用临时模式：临时 Item/Handle 的 UUID 将使用 Temp_ 前缀
    ItemRegistry::setTemporaryMode(true);
    XBaseItem* item = ItemRegistry::createObject(itemclass, graphicsWidget, itemMenu, nullptr);
    QIcon icon(item->image());
    delete item;
    item = nullptr;
    ItemRegistry::setTemporaryMode(false);

    // ... rest of the function ...
}
```

## 验证

### 编译

确保以下文件编译无错误：
- `ItemFactory.cpp`
- `XBaseItem.cpp`
- `XBaseHandle.cpp`
- `ColleagueManager.cpp`
- `mainwindow.cpp`

### 运行时验证

1. 启动应用程序
2. 观察第一个添加到场景的节点名称应该是 `LoadImage`（而非 `LoadImage1`）
3. 添加的节点 UUID 应该是 `Item_xxx` 格式（无 `Temp_` 前缀）

## 注意事项

1. **线程安全**：`temporaryMode_` 是静态变量，在多线程环境下需要注意。当前实现假设 UI 操作在主线程进行。

2. **RAII 模式**：可以考虑使用 RAII 包装器自动重置临时模式：

```cpp
class TemporaryModeGuard
{
public:
    TemporaryModeGuard() { ItemRegistry::setTemporaryMode(true); }
    ~TemporaryModeGuard() { ItemRegistry::setTemporaryMode(false); }
};

// 使用
{
    TemporaryModeGuard guard;
    XBaseItem* item = ItemRegistry::createObject(...);
    // ...
} // 自动重置
```

3. **扩展性**：如果将来有其他需要临时创建对象的场景，可以使用相同的模式。
