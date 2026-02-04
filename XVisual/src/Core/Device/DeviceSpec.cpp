#include "Core/Device/DeviceSpec.h"
#include <sstream>

namespace XVisual {

DeviceSpec DeviceSpec::CPU()
{
    return DeviceSpec(DeviceType::CPU, 0, "CPU");
}

DeviceSpec DeviceSpec::GPU(int index)
{
    return DeviceSpec(DeviceType::GPU, index, "GPU" + std::to_string(index));
}

DeviceSpec DeviceSpec::NPU(int index)
{
    return DeviceSpec(DeviceType::NPU, index, "NPU" + std::to_string(index));
}

DeviceSpec DeviceSpec::Any()
{
    return DeviceSpec(DeviceType::Any, 0, "Any");
}

DeviceSpec DeviceSpec::FromType(DeviceType type, int index)
{
    switch (type)
    {
    case DeviceType::CPU:
        return CPU();
    case DeviceType::GPU:
        return GPU(index);
    case DeviceType::NPU:
        return NPU(index);
    case DeviceType::Any:
    default:
        return Any();
    }
}

bool DeviceSpec::operator==(const DeviceSpec& other) const
{
    return type == other.type && index == other.index;
}

bool DeviceSpec::operator!=(const DeviceSpec& other) const
{
    return !(*this == other);
}

std::string DeviceSpec::toKeyString() const
{
    std::ostringstream oss;
    oss << deviceTypeToString(type);
    if (type == DeviceType::GPU || type == DeviceType::NPU)
    {
        oss << "[" << index << "]";
    }
    return oss.str();
}

std::string DeviceSpec::toString() const
{
    if (!name.empty())
    {
        return name;
    }
    return toKeyString();
}

} // namespace XVisual

// std::hash 特化实现
namespace std {
    size_t hash<XVisual::DeviceSpec>::operator()(const XVisual::DeviceSpec& spec) const
    {
        size_t h1 = std::hash<int>{}(static_cast<int>(spec.type));
        size_t h2 = std::hash<int>{}(spec.index);
        return h1 ^ (h2 << 1);
    }
}
