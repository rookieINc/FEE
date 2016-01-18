
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : module_load.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-24 14:09
 * Description : module_load.h
 * History     : 
 ***********************************************************/


#ifndef _MODULE_LOAD_H_INCLUDED_
#define _MODULE_LOAD_H_INCLUDED_

#include "kdk.h"
#include "module_parse.h"

#define HEAD                "HEAD"
#define MODULE_TYPE         "module_type"
#define MODULE_COUNT        "module_count"
#define MODULE_COUNT_LEN    "module_count_len"
#define MODULE_PATH         "path"
#define MODULE_FILE_NAME    "file_name"
#define MODULE_FUNC_NAME    "func_name"

static kdk_uint32
module_config_to_module(kdk_config_t *module_config, kdk_char32 *section, module_t *module);

kdk_uint32    
module_config_file_to_module_coll(kdk_char32 *module_config_file, module_coll_t *module_coll);

#endif /* _MODULE_LOAD_H_INCLUDED_ */
