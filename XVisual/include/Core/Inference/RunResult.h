#ifndef RUNRESULT_H
#define RUNRESULT_H

#include <string>
#include <chrono>

namespace XVisual {

/**
 * RunResult: 推理执行结果
 * 
 * 封装推理执行的结果状态，包括成功/失败、错误信息、执行时间等。
 */
struct RunResult
{
    bool success = false;                           // 是否成功
    std::string errorMessage;                       // 错误信息（失败时）
    std::chrono::microseconds duration{0};          // 执行耗时
    
    // 默认构造
    RunResult() = default;
    
    // 带参构造
    RunResult(bool s, const std::string& msg = "", std::chrono::microseconds dur = {})
        : success(s), errorMessage(msg), duration(dur) {}
    
    /**
     * 创建成功结果
     */
    static RunResult Success(std::chrono::microseconds dur = {})
    {
        return RunResult(true, "", dur);
    }
    
    /**
     * 创建失败结果
     */
    static RunResult Failure(const std::string& msg)
    {
        return RunResult(false, msg, {});
    }
    
    /**
     * 隐式转换为 bool
     */
    operator bool() const { return success; }
    
    /**
     * 获取执行时间（毫秒）
     */
    double durationMs() const
    {
        return duration.count() / 1000.0;
    }
};

} // namespace XVisual

#endif // RUNRESULT_H
