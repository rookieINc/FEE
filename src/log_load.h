
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : log_load.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2016-03-30 20:38
 * Description : log_load.h
 * History     : 
 ***********************************************************/


#ifndef _LOG_LOAD_H_INCLUDED_
#define _LOG_LOAD_H_INCLUDED_

#include "kdk.h"

#define HEAD        "HEAD"
#define LOG_PATH    "log_path"
#define LOG_NAME    "log_name"
#define LOG_LEVEL   "log_level"

kdk_uint32
log_config_file_to_log(kdk_char32 *log_config_file, kdk_log_t *log);

#endif /* _LOG_LOAD_H_INCLUDED_ */
