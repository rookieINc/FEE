
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : fee_engine.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2016-01-14 03:42
 * Description : fee_engine.c
 * History     : 
 ***********************************************************/


#include "fee_engine.h"

engine_config_t *
engine_config_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size)
{
    kdk_uint32           mem_pool_type = OTHER_MEM_POOL;
    engine_config_t     *engine_config;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = OWN_MEM_POOL;
    }

    engine_config = (engine_config_t *)kdk_mem_pool_malloc(mem_pool, sizeof(engine_runtime_t));
    if(engine_config == KDK_NULL)
    {
        if(mem_pool_type == OWN_MEM_POOL)
        {
            kdk_mem_pool_destroy(mem_pool);
            mem_pool = KDK_NULL;
        }    
        return KDK_NULL;
    }

    engine_config->mem_pool_type  = mem_pool_type;
    engine_config->mem_pool       = mem_pool;

    engine_config->flow_coll = flow_coll_create(engine_config->mem_pool, 0, 101);
    if(engine_config->flow_coll == KDK_NULL)
    {
        engine_config_destroy(engine_config);
        engine_config = KDK_NULL;
        return KDK_NULL;
    }

    engine_config->module_coll = module_coll_create(engine_config->mem_pool, 0, 101);
    if(engine_config->module_coll == KDK_NULL)
    {
        engine_config_destroy(engine_config);
        engine_config = KDK_NULL;
        return KDK_NULL;
    }

    engine_config->dl_handle_coll = kdk_dl_handle_coll_create(engine_config->mem_pool, 0);
    if(engine_config->dl_handle_coll == KDK_NULL)
    {
        engine_config_destroy(engine_config);
        engine_config = KDK_NULL;
        return KDK_NULL;
    }

    engine_config->log = (kdk_log_t *)kdk_mem_pool_malloc(engine_config->mem_pool, sizeof(kdk_log_t));
    if(engine_config->log == KDK_NULL)
    {
        engine_config_destroy(engine_config);
        engine_config = KDK_NULL;
        return KDK_NULL;
    }

    return engine_config;
}


kdk_uint32
engine_config_init(engine_config_t *engine_config, kdk_char32 *flow_config_file, kdk_char32 *module_config_file, kdk_char32 *log_config_file)
{
    kdk_uint32      ret_code;
    kdk_char32      path_file[MODULE_PATH_LEN + MODULE_FILE_NAME_LEN + 5] = {0};
    module_t       *module;

    if(engine_config == KDK_NULL) 
        return KDK_INARG;

    ret_code = flow_config_file_to_flow_coll(flow_config_file, engine_config->flow_coll);
    if(ret_code)
        return ret_code;

    ret_code = module_config_file_to_module_coll(module_config_file, engine_config->module_coll);
    if(ret_code)
        return ret_code;

    ret_code = log_config_file_to_log(log_config_file, engine_config->log);
    if(ret_code)
        return ret_code;

    module = kdk_hash_table_next_value(engine_config->module_coll->coll);
    while(module != KDK_NULLFOUND)
    {
        memset(path_file, 0, sizeof(path_file));
        snprintf(path_file, MODULE_PATH_LEN + MODULE_FILE_NAME_LEN + 5, "%s/%s", module->path, module->file_name);
        ret_code = kdk_dl_handle_coll_set(engine_config->dl_handle_coll, path_file, module->func_name);
        if(ret_code)
            return ret_code;

        module = kdk_hash_table_next_value(engine_config->module_coll->coll);
    }

    return KDK_SUCCESS;
}


kdk_void
engine_config_destroy(engine_config_t *engine_config)
{
    if(engine_config == KDK_NULL)
        return ;

    if(engine_config->flow_coll != KDK_NULL)
    {
        flow_coll_destroy(engine_config->flow_coll);
        engine_config->flow_coll = KDK_NULL;
    }

    if(engine_config->module_coll != KDK_NULL)
    {
        module_coll_destroy(engine_config->module_coll);
        engine_config->module_coll = KDK_NULL;
    }

    if(engine_config->dl_handle_coll != KDK_NULL)
    {
        kdk_dl_handle_coll_destroy(engine_config->dl_handle_coll);
        engine_config->dl_handle_coll = KDK_NULL;
    }
    
    if(engine_config->log != KDK_NULL)
    {
        kdk_log_destroy(engine_config->log);
        engine_config->log = KDK_NULL;
    }
    
    if(engine_config->mem_pool != KDK_NULL && engine_config->mem_pool_type == OWN_MEM_POOL)
    {
        kdk_mem_pool_destroy(engine_config->mem_pool);
        engine_config->mem_pool = KDK_NULL;
    }

    return ;
}


engine_runtime_t *
engine_runtime_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size)
{
    kdk_uint32           mem_pool_type = OTHER_MEM_POOL;
    engine_runtime_t    *engine_runtime;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = OWN_MEM_POOL;
    }

    engine_runtime = (engine_runtime_t *)kdk_mem_pool_malloc(mem_pool, sizeof(engine_runtime_t));
    if(engine_runtime == KDK_NULL)
    {
        if(mem_pool_type == OWN_MEM_POOL)
        {
            kdk_mem_pool_destroy(mem_pool);
            mem_pool = KDK_NULL;
        }
        return KDK_NULL;
    }

    engine_runtime->mem_pool_type  = mem_pool_type;
    engine_runtime->mem_pool       = mem_pool;

    engine_runtime->flow_runtime = flow_runtime_create(engine_runtime->mem_pool, 0);
    if(engine_runtime->flow_runtime == KDK_NULL)
    {
        engine_runtime_destroy(engine_runtime);
        engine_runtime = KDK_NULL;
        return KDK_NULL;
    }

    engine_runtime->data_pool = kdk_hash_table_create(engine_runtime->mem_pool, 0, 101);
    if(engine_runtime->data_pool == KDK_NULL)
    {
        engine_runtime_destroy(engine_runtime);
        engine_runtime = KDK_NULL;
        return KDK_NULL;
    }

    return engine_runtime;
}


kdk_uint32
engine_runtime_init(engine_runtime_t *engine_runtime, engine_config_t *engine_config, kdk_char32 *flow_id)
{
    if(engine_config == KDK_NULL || engine_runtime == KDK_NULL)
        return KDK_INARG;

    return flow_runtime_init(engine_runtime->flow_runtime, engine_config->flow_coll, flow_id);
}


kdk_void
engine_runtime_clear(engine_runtime_t *engine_runtime)
{
    if(engine_runtime == KDK_NULL)
        return ;

    flow_runtime_clear(engine_runtime->flow_runtime);

    return ;
}


kdk_void
engine_runtime_destroy(engine_runtime_t *engine_runtime)
{
    if(engine_runtime == KDK_NULL)
        return ;

    if(engine_runtime->flow_runtime != KDK_NULL)
    {
        flow_runtime_destroy(engine_runtime->flow_runtime);
        engine_runtime->flow_runtime = KDK_NULL;
    }

    if(engine_runtime->data_pool != KDK_NULL)
    {
        kdk_hash_table_destroy(engine_runtime->data_pool);
        engine_runtime->data_pool = KDK_NULL;
    }
    
    if(engine_runtime->mem_pool != KDK_NULL && engine_runtime->mem_pool_type == OWN_MEM_POOL)
    {
        kdk_mem_pool_destroy(engine_runtime->mem_pool);
        engine_runtime->mem_pool = KDK_NULL;
    }

    return ;
}


engine_t *
engine_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size)
{
    kdk_uint32       mem_pool_type = OTHER_MEM_POOL;
    engine_t        *engine;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = OWN_MEM_POOL;
    }

    engine = (engine_t *)kdk_mem_pool_malloc(mem_pool, sizeof(engine_t));
    if(engine == KDK_NULL)
    {
        if(mem_pool_type == OWN_MEM_POOL)
        {
            kdk_mem_pool_destroy(mem_pool);
            mem_pool = KDK_NULL;
        }
        return KDK_NULL;
    }

    engine->mem_pool_type  = mem_pool_type;
    engine->mem_pool       = mem_pool;

    engine->config = engine_config_create(engine->mem_pool, 0);
    if(engine->config == KDK_NULL)
    {
        engine_destroy(engine);
        engine = KDK_NULL;
        return KDK_NULL;
    }

    engine->runtime = engine_runtime_create(engine->mem_pool, 0);
    if(engine->runtime == KDK_NULL)
    {
        engine_destroy(engine);
        engine = KDK_NULL;
        return KDK_NULL;
    }

    return engine;
}


kdk_uint32
engine_init(engine_t *engine, kdk_char32 *flow_config_file, kdk_char32 *module_config_file, kdk_char32 *log_config_file, kdk_char32 *flow_id)
{
    kdk_uint32      ret_code;

    if(engine == KDK_NULL)
        return KDK_INARG;

    ret_code = engine_config_init(engine->config, flow_config_file, module_config_file, log_config_file);
    if(ret_code)
        return ret_code;

    ret_code = engine_runtime_init(engine->runtime, engine->config, flow_id);
    if(ret_code)
        return ret_code;

    return KDK_SUCCESS;
}


kdk_void
engine_destroy(engine_t *engine)
{
    if(engine == KDK_NULL)
        return ;

    if(engine->runtime != KDK_NULL)
    {
        engine_runtime_destroy(engine->runtime);
        engine->runtime = KDK_NULL;
    }

    if(engine->config != KDK_NULL)
    {
        engine_config_destroy(engine->config);
        engine->config = KDK_NULL;
    }
    
    if(engine->mem_pool != KDK_NULL && engine->mem_pool_type == OWN_MEM_POOL)
    {
        kdk_mem_pool_destroy(engine->mem_pool);
        engine->mem_pool = KDK_NULL;
    }

    return ;
}
