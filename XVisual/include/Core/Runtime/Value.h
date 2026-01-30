#ifndef XVISUAL_CORE_RUNTIME_VALUE_H
#define XVISUAL_CORE_RUNTIME_VALUE_H

#include <any>

namespace XVisual {

/**
 * Value: CPU-only 通用值类型（PR-3 阶段先用 std::any，PR-5 可扩展到 Tensor wrapper）
 */
using Value = std::any;

} // namespace XVisual

#endif // XVISUAL_CORE_RUNTIME_VALUE_H
