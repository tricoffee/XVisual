#include "Core/Inference/ModelDesc.h"
#include <sstream>

namespace XVisual {

ModelDesc ModelDesc::TFSavedModel(const std::string& path)
{
    return ModelDesc(path, "tf_savedmodel");
}

ModelDesc ModelDesc::ONNX(const std::string& path)
{
    return ModelDesc(path, "onnx");
}

ModelDesc ModelDesc::TorchScript(const std::string& path)
{
    return ModelDesc(path, "torchscript");
}

ModelDesc ModelDesc::TFLite(const std::string& path)
{
    return ModelDesc(path, "tflite");
}

std::string ModelDesc::toKeyString() const
{
    std::ostringstream oss;
    oss << format << ":" << path;
    
    // 如果有选项，也加入 key
    if (!options.empty())
    {
        oss << ":";
        for (const auto& kv : options)
        {
            oss << kv.first << "=" << kv.second << ";";
        }
    }
    
    return oss.str();
}

bool ModelDesc::isValid() const
{
    return !path.empty() && !format.empty();
}

} // namespace XVisual
