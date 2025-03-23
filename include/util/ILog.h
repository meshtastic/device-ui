#pragma once
#ifdef USE_ILOG

#define ILOG_DEBUG(...) ILog::logger()->log_debug(__VA_ARGS__)
#define ILOG_INFO(...) ILog::logger()->log_info(__VA_ARGS__)
#define ILOG_WARN(...) ILog::logger()->log_warn(__VA_ARGS__)
#define ILOG_ERROR(...) ILog::logger()->log_error(__VA_ARGS__)
#define ILOG_CRIT(...) ILog::logger()->log_crit(__VA_ARGS__)
#define ILOG_TRACE(...) ILog::logger()->log_trace(__VA_ARGS__)

/**
 * abstract class to inject debug logging into the library
 */
class ILog
{
  public:
    virtual void log_debug(const char *format, ...) = 0;
    virtual void log_info(const char *format, ...) = 0;
    virtual void log_warn(const char *format, ...) = 0;
    virtual void log_error(const char *format, ...) = 0;
    virtual void log_crit(const char *format, ...) = 0;
    virtual void log_trace(const char *format, ...) = 0;

    static ILog *logger() { return _logger; }
    virtual ~ILog() {}

  protected:
    ILog(ILog *logger) { _logger = logger; }

  private:
    static ILog *_logger;
};

#elif defined(USE_LOG_DEBUG)

// alternative approach to directly use LOG_DEBUG macros
#include LOG_DEBUG_INC
#define ILOG_DEBUG(...) LOG_DEBUG("[DeviceUI] " __VA_ARGS__)
#define ILOG_INFO(...) LOG_INFO("[DeviceUI] " __VA_ARGS__)
#define ILOG_WARN(...) LOG_WARN("[DeviceUI] " __VA_ARGS__)
#define ILOG_ERROR(...) LOG_ERROR("[DeviceUI] " __VA_ARGS__)
#define ILOG_CRIT(...) LOG_CRIT("[DeviceUI] " __VA_ARGS__)
#define ILOG_TRACE(...) LOG_TRACE("[DeviceUI] " __VA_ARGS__)

#elif defined(DEBUG_UNIT_TEST)
#include <cstdio>
#define ILOG_DEBUG(...)                                                                                                          \
    {                                                                                                                            \
        printf("DEBUG " __VA_ARGS__);                                                                                            \
        printf("\n");                                                                                                            \
    }
#define ILOG_INFO(...)                                                                                                           \
    {                                                                                                                            \
        printf("INFO  " __VA_ARGS__);                                                                                            \
        printf("\n");                                                                                                            \
    }
#define ILOG_WARN(...)                                                                                                           \
    {                                                                                                                            \
        printf("WARN  " __VA_ARGS__);                                                                                            \
        printf("\n");                                                                                                            \
    }
#define ILOG_ERROR(...)                                                                                                          \
    {                                                                                                                            \
        printf("ERROR " __VA_ARGS__);                                                                                            \
        printf("\n");                                                                                                            \
    }
#define ILOG_CRIT(...)                                                                                                           \
    {                                                                                                                            \
        printf("CRIT  " __VA_ARGS__);                                                                                            \
        printf("\n");                                                                                                            \
    }
#define ILOG_TRACE(...)                                                                                                          \
    {                                                                                                                            \
        printf("TRACE " __VA_ARGS__);                                                                                            \
        printf("\n");                                                                                                            \
    }

#else // no logging

#define ILOG_DEBUG(...)
#define ILOG_INFO(...)
#define ILOG_WARN(...)
#define ILOG_ERROR(...)
#define ILOG_CRIT(...)
#define ILOG_TRACE(...)

#endif
