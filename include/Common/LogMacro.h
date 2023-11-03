#ifndef LogMacro_H
#define LogMacro_H

#define XLOG_INFO(message,line) \
    logger.LogInfo(message, __FILE__, line)
#define XLOG_ERROR(message,line) \
    logger.LogError(message, __FILE__, line)

#endif //LogMacro_H

