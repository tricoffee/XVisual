#ifndef MODELDESC_H
#define MODELDESC_H

#include <string>
#include <unordered_map>

namespace XVisual {

/**
 * ModelDesc: 模型描述符
 * 
 * 描述一个推理模型的元信息，与具体推理框架无关。
 */
struct ModelDesc
{
    std::string path;           // 模型路径 (文件或目录)
    std::string format;         // 模型格式:
                                //   - "tf_savedmodel" (TensorFlow SavedModel)
                                //   - "onnx"          (ONNX)
                                //   - "torchscript"   (TorchScript)
                                //   - "tflite"        (TensorFlow Lite)
    std::unordered_map<std::string, std::string> options;  // 额外选项
    
    // 默认构造
    ModelDesc() = default;
    
    // 带参构造
    ModelDesc(const std::string& p, const std::string& f)
        : path(p), format(f) {}
    
    // 便捷构造
    static ModelDesc TFSavedModel(const std::string& path);
    static ModelDesc ONNX(const std::string& path);
    static ModelDesc TorchScript(const std::string& path);
    static ModelDesc TFLite(const std::string& path);
    
    // 生成唯一标识字符串 (用于缓存 key)
    std::string toKeyString() const;
    
    // 检查是否有效
    bool isValid() const;
};

} // namespace XVisual

#endif // MODELDESC_H
