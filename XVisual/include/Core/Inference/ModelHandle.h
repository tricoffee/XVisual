#ifndef MODELHANDLE_H
#define MODELHANDLE_H

#include <cstdint>

namespace XVisual {

/**
 * ModelHandle: 模型句柄类型
 * 
 * 不透明句柄，由各后端内部管理实际含义。
 * 使用整数类型而非指针，避免跨后端的类型混淆。
 * 
 * 用法:
 *   ModelHandle handle = backend->load(modelDesc, deviceSpec);
 *   backend->run(handle, inputs, &outputs);
 *   backend->unload(handle);
 */
using ModelHandle = uint64_t;

/**
 * 无效句柄常量
 */
constexpr ModelHandle INVALID_MODEL_HANDLE = 0;

/**
 * 检查句柄是否有效
 */
inline bool isValidHandle(ModelHandle handle)
{
    return handle != INVALID_MODEL_HANDLE;
}

} // namespace XVisual

#endif // MODELHANDLE_H
