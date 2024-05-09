#ifndef LogMacro_H
#define LogMacro_H

#define XLOG_INFO(message,threadId) \
    logger.LogInfo(message,threadId, __FILE__, __LINE__)
#define XLOG_ERROR(message,threadId) \
    logger.LogError(message,threadId, __FILE__, __LINE__)
// ���ڱ���ͼƬlog�궨��
#define XLOG_SAVE_IMAGE(image, filename, threadId) \
    logger.SaveImage(image, filename, threadId, __FILE__, __LINE__)

#endif //LogMacro_H
