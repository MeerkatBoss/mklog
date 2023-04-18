/**
 * @file logs.h
 * @author MeerkatBoss (solodovnikov.ia@phystech.edu)
 * 
 * @brief
 *
 * @version 0.1
 * @date 2023-04-18
 *
 * @copyright Copyright MeerkatBoss (c) 2023
 */
#ifndef __MEERKAT_LOGS_H
#define __MEERKAT_LOGS_H

#include <stdio.h>

enum log_level
{
    LOG_TRACE = 0,
    LOG_INFO  = 1,
    LOG_WARN  = 2,
    LOG_ERROR = 3,
    LOG_FATAL = 4
};

enum log_content_options
{
    LOG_CONTENT_MSG_ONLY        = 0x00,
    LOG_CONTENT_TIME            = 0x01,
    LOG_CONTENT_MSG_TYPE        = 0x02,
    LOG_CONTENT_SRC_FILE        = 0x04,
    LOG_CONTENT_SRC_LINE        = 0x08,
    LOG_CONTENT_SRC_FUNC        = 0x10,
    LOG_CONTENT_SRC_LOGGER_NAME = 0x20,
    LOG_CONTENT_ERRNO           = 0x40,
    LOG_CONTENT_STRERROR        = 0x80

    LOG_CONTENT_ALL = 2*LOG_CONTENT_STRERROR - 1
};

enum log_content_type
{
    LOG_TYPE_PLAIN_TEXT,
    LOG_TYPE_MARKDOWN,
    LOG_TYPE_HTML,
    LOG_TYPE_ESCAPE
};

struct logger
{
    const char* name;
    const char* file_name;

    int is_in_section;
    int is_active;
};

struct logging_provider
{
    const char* name;
    FILE* output_stream;

    int is_stream_owned;
    int is_active;

    log_level           minimum_level;
    log_content_options content_options;
    log_content_type    content type;
};

#ifndef SUPPRESS_LOGS

#define USE_LOGS(name)          \
    logger __file_logger = {};  \
    int __useless_##__COUNTER__ = _logger_ctor(&__file_logger, name, __FILE__)

#define LOG_TRACE(message) _log_message(&__file_logger, LOG_TRACE, message)
#define LOG_INFO(message)  _log_message(&__file_logger, LOG_INFO,  message)
#define LOG_WARN(message)  _log_message(&__file_logger, LOG_WARN,  message)
#define LOG_ERROR(message) _log_message(&__file_logger, LOG_ERROR, message)
#define LOG_FATAL(message) _log_message(&__file_logger, LOG_FATAL, message)

#define LOGF_TRACE(message, ...) \
    _log_message(&__file_logger, LOG_TRACE, message, __VA_ARGS__)
#define LOGF_INFO(message, ...) \
    _log_message(&__file_logger, LOG_INFO,  message, __VA_ARGS__)
#define LOGF_WARN(message, ...) \
    _log_message(&__file_logger, LOG_WARN,  message, __VA_ARGS__)
#define LOGF_ERROR(message, ...) \
    _log_message(&__file_logger, LOG_ERROR, message, __VA_ARGS__)
#define LOGF_FATAL(message, ...) \
    _log_message(&__file_logger, LOG_FATAL, message, __VA_ARGS__)

#define LOG_TRACE_SECTION(log_level, header)                    \
    for (_log_start_section(&__file_logger, log_level, header), \
            __file_logger.is_in_section = 1;                    \
         __file_logger.is_in_section;                           \
         _log_end_section(&__file_logger, log_level, header),   \
            __file_logger.is_in_section = 0)

#define LOG_SUSPEND __file_logger.is_active = 0
#define LOG_RESUME  __file_logger.is_active = 1

#else

#define USE_LOGS(name)

#define LOG_TRACE(message)
#define LOG_INFO(message)
#define LOG_WARN(message)
#define LOG_ERROR(message)
#define LOG_FATAL(message)

#define LOGF_TRACE(message, ...)
#define LOGF_INFO(message, ...)
#define LOGF_WARN(message, ...)
#define LOGF_ERROR(message, ...)
#define LOGF_FATAL(message, ...)

#define LOG_TRACE_SECTION(log_level, header)

#define LOG_SUSPEND
#define LOG_RESUME

#endif

#define DEFAULT_STDOUT_PROVIDER                 \
{                                               \
    .name = "Default console provider",         \
    .output_stream   = stdout,                  \
    .is_stream_owned = 0,                       \
    .is_active       = 1,                       \
    .minumum_level   = LOG_INFO,                \
    .content_options = LOG_CONTENT_TIME         \
                        | LOG_CONTENT_MSG_TYPE  \
                        | LOG_CONTENT_SRC_FILE  \
                        | LOG_CONTENT_SRC_LINE, \
    .content_type    = LOG_TYPE_ESCAPE          \
}

#define MSG_ONLY_STDOUT_PROVIDER                \
{                                               \
    .name = "Message-only console provider",    \
    .output_stream   = stdout,                  \
    .is_stream_owned = 0,                       \
    .is_active       = 1,                       \
    .minumum_level   = LOG_INFO,                \
    .content_options = LOG_CONTENT_MSG_ONLY,    \
    .content_type    = LOG_TYPE_ESCAPE          \
}

#define DEFAULT_HTML_PROVIDER(name)                     \
{                                                       \
    .name = "'" name "' HTML provider",                 \
    .output_stream   = fopen("logs/" name ".html", "w"),\
    .is_stream_owned = 1,                               \
    .is_active       = 1,                               \
    .minumum_level   = LOG_TRACE,                       \
    .content_options = LOG_CONTENT_ALL,                 \
    .content_type    = LOG_TYPE_HTML                    \
}

#define DEFAULT_PLAIN_TEXT_PROVIDER(name)               \
{                                                       \
    .name = "'" name "' plain-text provider",           \
    .output_stream   = fopen("logs/" name ".log", "w"), \
    .is_stream_owned = 1,                               \
    .is_active       = 1,                               \
    .minumum_level   = LOG_TRACE,                       \
    .content_options = LOG_CONTENT_ALL,                 \
    .content_type    = LOG_TYPE_PLAIN_TEXT              \
}

void link_logger_provider(logging_provider* provider);

void add_logger_provider(const char* name, FILE* stream,
                         log_level min_level,
                         log_content_options options,
                         log_content_type type);

void add_file_logger_provider(const char* name, const char* filename,
                              log_level min_level,
                              log_content_options options,
                              log_content_type type);

int _logger_ctor(logger* logger, const char* name, const char* filename);

void _log_message(const logger* logger, log_level level,
                  const char* format, ...)
    __attribute__((format (printf, 3, 4)));

void _log_start_section(const logger* logger, log_level level,
                        const char* section name);

void _log_end_section  (const logger* logger, log_level level,
                        const char* section name);


#endif /* logs.h */
