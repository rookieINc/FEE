
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : engine_exec.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2016-01-14 03:42
 * Description : engine_exec.h
 * History     : 
 ***********************************************************/


#ifndef _ENGINE_EXEC_H_INCLUDED_
#define _ENGINE_EXEC_H_INCLUDED_

#include "kdk.h"
#include "flow_parse.h"
#include "flow_load.h"
#include "flow_runtime.h"
#include "module_parse.h"
#include "module_load.h"


struct engine_config_s
{
    kdk_uint32                   mem_pool_type;
    struct kdk_mem_pool_s       *mem_pool;
    struct flow_coll_s          *flow_coll;  
    struct module_coll_s        *module_coll;
    struct kdk_dl_handle_coll_s *dl_handle_coll;
};

typedef struct engine_config_s engine_config_t;

struct engine_runtime_s
{
    kdk_uint32               mem_pool_type;
    struct kdk_mem_pool_s   *mem_pool;
    struct flow_runtime_s   *flow_runtime;
    struct kdk_hash_table_s *data_pool;
};

typedef struct engine_runtime_s engine_runtime_t;

struct engine_s
{
    kdk_uint32               mem_pool_type;
    struct kdk_mem_pool_s   *mem_pool;
    struct engine_config_s  *config;
    struct engine_runtime_s *runtime;
};

typedef struct engine_s engine_t;


engine_config_t *
engine_config_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size);

kdk_uint32
engine_config_init(engine_config_t *engine_config, kdk_char32 *flow_config_file, kdk_char32 *module_config_file);

kdk_void
engine_config_destroy(engine_config_t *engine_config);


engine_runtime_t *
engine_runtime_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size);

kdk_uint32
engine_runtime_init(engine_runtime_t *engine_runtime, engine_config_t *engine_config, kdk_char32 *flow_id);

kdk_void
engine_runtime_destroy(engine_runtime_t *engine_runtime);


engine_t *
engine_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size);

kdk_uint32
engine_init(engine_t *engine, kdk_char32 *flow_config_file, kdk_char32 *module_config_file, kdk_char32 *flow_id);

kdk_void
engine_destroy(engine_t *engine);

#endif /* _ENGINE_EXEC_H_INCLUDED */
