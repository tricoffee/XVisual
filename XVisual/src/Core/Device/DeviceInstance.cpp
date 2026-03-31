#include "Core/Device/DeviceInstance.h"

namespace XVisual {

std::string DeviceInstance::name() const
{
    std::string prefix;
    switch (type)
    {
    case DeviceType::CPU:
        prefix = "cpu";
        break;
    case DeviceType::GPU:
        prefix = "gpu";
        break;
    case DeviceType::NPU:
        prefix = "npu";
        break;
    case DeviceType::Any:
        prefix = "any";
        break;
    default:
        prefix = "unknown";
        break;
    }
    return prefix + ":" + std::to_string(index);
}

} // namespace XVisual
