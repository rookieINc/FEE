
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : log_load.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2016-03-30 20:35
 * Description : log_load.c
 * History     : 
 ***********************************************************/


#include "log_load.h"

kdk_uint32
log_config_file_to_log(kdk_char32 *log_config_file, kdk_log_t *log)
{
    kdk_config_t    *log_config; 
    kdk_char32       key[CONFIG_KEY_LEN], value[CONFIG_VALUE_LEN], flow_head[CONFIG_KEY_LEN] = {0};
    kdk_uint32       ret_code = 0;

    if(log_config_file == KDK_NULL || log == KDK_NULL)
        return KDK_INARG;

    log_config = kdk_config_create(KDK_NULL, 1024, log_config_file);
    if(log_config == KDK_NULL)
        return KDK_NULLPTR;

    ret_code = kdk_config_init(log_config);
    if(ret_code)
    {
        kdk_config_destroy(log_config);
        log_config = KDK_NULL;
        return ret_code;
    }

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(log_config, HEAD, LOG_PATH, value);
    if(ret_code)
    {
        kdk_config_destroy(log_config);
        log_config = KDK_NULL;
        return ret_code;
    }
    strncpy(log->file_path, value, PATH_LEN);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(log_config, HEAD, LOG_NAME, value);
    if(ret_code)
    {
        kdk_config_destroy(log_config);
        log_config = KDK_NULL;
        return ret_code;
    }
    strncpy(log->file_name, value, NAME_LEN);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(log_config, HEAD, LOG_LEVEL, value);
    if(ret_code)
    {
        kdk_config_destroy(log_config);
        log_config = KDK_NULL;
        return ret_code;
    }

    if(strcmp(value, "DEBUG") == 0)
        log->level = LOG_LEVEL_DEBUG;
    else if(strcmp(value, "INFO") == 0)
        log->level = LOG_LEVEL_INFO;
    else if(strcmp(value, "WARN") == 0)
        log->level = LOG_LEVEL_WARN;
    else if(strcmp(value, "ERROR") == 0)
        log->level = LOG_LEVEL_ERROR;
    else
        log->level = LOG_LEVEL_DEBUG;

    kdk_config_destroy(log_config);
    log_config = KDK_NULL;

    return KDK_SUCCESS;
}
