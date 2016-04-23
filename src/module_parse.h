
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : module_parse.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-25 07:58
 * Description : module_parse.h
 * History     : 
 ***********************************************************/


#ifndef _MODULE_PARSE_H_INCLUDED_
#define _MODULE_PARSE_H_INCLUDED_

#include "kdk.h"
#include "flow_parse.h"

#define MODULE_PATH_LEN         48
#define MODULE_FILE_NAME_LEN    48
#define MODULE_FUNC_NAME_LEN    48

#define HEAD                    "HEAD"
#define MODULE_TYPE             "module_type"
#define MODULE_COUNT            "module_count"
#define MODULE_COUNT_LEN        "module_count_len"
#define MODULE_PATH             "path"
#define MODULE_FILE_NAME        "file_name"
#define MODULE_FUNC_NAME        "func_name"

struct module_s
{
    kdk_char32      id[NODE_ID_LEN + 1];
    kdk_char32      path[MODULE_PATH_LEN + 1];
    kdk_char32      file_name[MODULE_FILE_NAME_LEN + 1];
    kdk_char32      func_name[MODULE_FUNC_NAME_LEN + 1];
};

typedef struct module_s module_t;

struct module_coll_s
{
    kdk_uint32               type;
    kdk_uint32               mem_pool_type;
    struct kdk_mem_pool_s   *mem_pool;
    struct kdk_hash_table_s *coll;
};

typedef struct module_coll_s module_coll_t;

module_coll_t*
module_coll_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size, kdk_uint32 prime);

kdk_uint32 
module_coll_set(module_coll_t *module_coll, kdk_char32 *key, module_t *module);

kdk_uint32
module_coll_get(module_coll_t *module_coll, kdk_char32 *module_id, module_t *module);

kdk_void 
module_coll_destroy(module_coll_t *module_coll);

static kdk_uint32
module_config_to_module(kdk_config_t *module_config, kdk_char32 *section, module_t *module);

kdk_uint32    
module_config_file_to_module_coll(kdk_char32 *module_config_file, module_coll_t *module_coll);

#endif /* _MODULE_PARSE_H_INCLUDED_ */
