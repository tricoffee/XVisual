#ifndef VALUE_H
#define VALUE_H

#include <any>
#include <string>
#include <typeinfo>

namespace XVisual {

/**
 * Value: 运行时值包装
 * 
 * PR-5.2: 简单包装 std::any，提供类型安全的访问接口
 * PR-5.5: 将扩展为 TensorValue (含设备位置、dtype、shape)
 * 
 * 这是一个过渡设计，为后续异构数据路径做准备。
 */
struct Value
{
    std::any payload;       // 实际数据
    
    // 默认构造
    Value() = default;
    
    // 从任意类型构造
    template<typename T>
    explicit Value(T&& val) : payload(std::forward<T>(val)) {}
    
    /**
     * 便捷构造
     */
    template<typename T>
    static Value From(T&& val)
    {
        return Value(std::forward<T>(val));
    }
    
    /**
     * 类型安全获取（非 const 引用）
     * @throws std::bad_any_cast 如果类型不匹配
     */
    template<typename T>
    T& as()
    {
        return std::any_cast<T&>(payload);
    }
    
    /**
     * 类型安全获取（const 引用）
     * @throws std::bad_any_cast 如果类型不匹配
     */
    template<typename T>
    const T& as() const
    {
        return std::any_cast<const T&>(payload);
    }
    
    /**
     * 尝试获取，失败返回 nullptr
     */
    template<typename T>
    T* tryAs()
    {
        return std::any_cast<T>(&payload);
    }
    
    template<typename T>
    const T* tryAs() const
    {
        return std::any_cast<T>(&payload);
    }
    
    /**
     * 检查是否有值
     */
    bool hasValue() const
    {
        return payload.has_value();
    }
    
    /**
     * 检查类型
     */
    template<typename T>
    bool holdsType() const
    {
        return payload.type() == typeid(T);
    }
    
    /**
     * 获取类型名称（用于调试）
     */
    std::string typeName() const
    {
        if (!hasValue())
        {
            return "empty";
        }
        return payload.type().name();
    }
    
    /**
     * 重置
     */
    void reset()
    {
        payload.reset();
    }
};

} // namespace XVisual

#endif // VALUE_H
