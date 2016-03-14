
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : daemon.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2016-01-18 18:17
 * Description : daemon.h
 * History     : 
 ***********************************************************/


#ifndef _DAEMON_H_INCLUDED_
#define _DAEMON_H_INCLUDED_

#include "kdk.h"

#define DAEMON      "DAEMON"
#define SLEEP_TIME  "sleep_time"

#define PROCESS     "PROCESS"
#define PROC_COUNT  "process_count"
#define PROC_PATH   "process_path"
#define PROC_NAME   "process_name"

#define ARGUMENT    "ARGUMENT"
#define ARG_COUNT   "argument_count"

#define PATH_LEN    48
#define NAME_LEN    48
#define ARG_LEN     10

struct daemon_config_s
{
    kdk_uint32              sleep_time;
    kdk_uint32              proc_count;
    kdk_char32              proc_path[PATH_LEN + 1];  
    kdk_char32              proc_name[NAME_LEN + 1];  
    kdk_uint32              argc;
    kdk_char32             *argv[ARG_LEN + 2];
    kdk_uint32              mem_pool_type;
    struct kdk_mem_pool_s  *mem_pool;
};

typedef struct daemon_config_s daemon_config_t;


#endif /* _DAEMON_H_INCLUDED_ */
