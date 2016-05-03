
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : fee_daemon.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2016-01-18 18:17
 * Description : fee_daemon.h
 * History     : 
 ***********************************************************/


#ifndef _FEE_DAEMON_H_INCLUDED_
#define _FEE_DAEMON_H_INCLUDED_

#include "kdk.h"

#define DAEMON      "DAEMON"
#define SLEEP_TIME  "sleep_time"

#define LOG         "LOG"
#define LOG_PATH    "log_path"
#define LOG_NAME    "log_name"
#define LOG_LEVEL   "log_level"

#define PROCESS     "PROCESS"
#define PROC_COUNT  "process_count"
#define PROC_PATH   "process_path"
#define PROC_NAME   "process_name"

#define ARGUMENT    "ARGUMENT"
#define ARG_COUNT   "argument_count"

struct daemon_config_s
{
    kdk_uint32              sleep_time;
    kdk_char32              log_path[PATH_LEN + 1];
    kdk_char32              log_name[NAME_LEN + 1];
    kdk_uint32              log_level;
    kdk_uint32              proc_count;
    kdk_char32              proc_path[PATH_LEN + 1];  
    kdk_char32              proc_name[NAME_LEN + 1];  
    kdk_uint32              argc;
    kdk_char32             *argv[ARG_LEN + 2];
    kdk_uint32              mem_pool_type;
    struct kdk_mem_pool_s  *mem_pool;
};

typedef struct daemon_config_s daemon_config_t;


#endif /* _FEE_DAEMON_H_INCLUDED_ */
