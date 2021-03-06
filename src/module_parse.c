
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : module_parse.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-25 08:08
 * Description : module_parse.c
 * History     : 
 ***********************************************************/


#include "module_parse.h"


module_coll_t *
module_coll_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size, kdk_uint32 prime)
{
    kdk_uint32           mem_pool_type = OTHER_MEM_POOL;
    module_coll_t       *module_coll;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = OWN_MEM_POOL;
    }

    module_coll = (module_coll_t *)kdk_mem_pool_malloc(mem_pool, sizeof(module_coll_t));
    if(module_coll == KDK_NULL)
    {
        if(mem_pool_type == OWN_MEM_POOL)
        {
            kdk_mem_pool_destroy(mem_pool);
            mem_pool = KDK_NULL;
        }
        return KDK_NULL;
    }

    memset(module_coll, 0, sizeof(module_coll_t));

    module_coll->type          = 0;
    module_coll->mem_pool_type = mem_pool_type;
    module_coll->mem_pool      = mem_pool;

    module_coll->coll = kdk_hash_table_create(mem_pool, 0, prime);
    if(module_coll->coll == KDK_NULL)
    {
        module_coll_destroy(module_coll);
        return KDK_NULL;
    }

    return module_coll;
}

kdk_uint32 
module_coll_set(module_coll_t *module_coll, kdk_char32 *key, module_t *module)
{
    if(module_coll == KDK_NULL)
        return KDK_INARG;

    return kdk_hash_table_set_value(module_coll->coll, key, module, sizeof(module_t));
}

kdk_uint32
module_coll_get(module_coll_t *module_coll, kdk_char32 *module_id, module_t *module)
{
    module_t    *ret_module;

    if(module_coll == KDK_NULL || module == KDK_NULL)
        return KDK_INARG;

    ret_module = (module_t *)kdk_hash_table_get_value(module_coll->coll, module_id);
    if(ret_module == KDK_NULL)
        return KDK_NULLPTR;        
    else if(ret_module == KDK_NULLFOUND)
        return KDK_NOTFOUND;

    memset(module, 0, sizeof(module_t));
    memcpy(module, ret_module, sizeof(module_t));

    return KDK_SUCCESS;
}

kdk_void 
module_coll_destroy(module_coll_t *module_coll)
{
    if(module_coll == KDK_NULL)
        return ;

    if(module_coll->coll != KDK_NULL)
    {
        kdk_hash_table_destroy(module_coll->coll);
        module_coll->coll = KDK_NULL;
    }

    if(module_coll->mem_pool != KDK_NULL && module_coll->mem_pool_type == OWN_MEM_POOL)
    {
        kdk_mem_pool_destroy(module_coll->mem_pool);
        module_coll->mem_pool = KDK_NULL;
    }

    return ;
}

static kdk_uint32
module_config_to_module(kdk_config_t *module_config, kdk_char32 *section, module_t *module)
{
    kdk_uint32  ret_code;

    if(module_config == KDK_NULL || module == KDK_NULL)
        return KDK_INARG;

    memset(module, 0, sizeof(module_t));

    strncpy(module->id, section, NODE_ID_LEN);

    ret_code = kdk_config_get_value(module_config, section, MODULE_PATH, module->path);
    if(ret_code)
        return ret_code;
    
    ret_code = kdk_config_get_value(module_config, section, MODULE_FILE_NAME, module->file_name);
    if(ret_code)
        return ret_code;

    ret_code = kdk_config_get_value(module_config, section, MODULE_FUNC_NAME, module->func_name);
    if(ret_code)
        return ret_code;

    return KDK_SUCCESS;
}

kdk_uint32    
module_config_file_to_module_coll(kdk_char32 *module_config_file, module_coll_t *module_coll)
{
    kdk_config_t    *module_config;
    module_t         module;
    kdk_char32       section[CONFIG_KEY_LEN], value[CONFIG_VALUE_LEN];
    kdk_uint32       ret_code = 0, module_count, module_count_len, i;

    if(module_config_file == KDK_NULL || module_coll == KDK_NULL)
        return KDK_INARG;

    module_config = kdk_config_create(KDK_NULL, 1024, module_config_file);
    if(module_config == KDK_NULL)
        return KDK_NULLPTR;

    ret_code = kdk_config_init(module_config);
    if(ret_code)
    {
        kdk_config_destroy(module_config);
        module_config = KDK_NULL;
        return ret_code;
    }

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(module_config, HEAD, MODULE_TYPE, value);
    if(ret_code)
    {
        kdk_config_destroy(module_config);
        module_config = KDK_NULL;
        return ret_code;
    }
    module_coll->type = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(module_config, HEAD, MODULE_COUNT, value);
    if(ret_code)
    {
        kdk_config_destroy(module_config);
        module_config = KDK_NULL;
        return ret_code;
    }
    module_count = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(module_config, HEAD, MODULE_COUNT_LEN, value);
    if(ret_code)
    {
        kdk_config_destroy(module_config);
        module_config = KDK_NULL;
        return ret_code;
    }
    module_count_len = atoi(value);

    for(i = 1; i <= module_count; i++)
    {
        memset(section, 0, sizeof(section));
        snprintf(section, CONFIG_KEY_LEN, "%0*d", module_count_len, i);  

        ret_code = module_config_to_module(module_config, section, &module);
        if(ret_code && ret_code != KDK_NOTFOUND)
        {
            kdk_config_destroy(module_config);
            module_config = KDK_NULL;
            return ret_code;
        }
        else if(ret_code == KDK_NOTFOUND)
        {
            continue;
        }

        ret_code = module_coll_set(module_coll, section, &module);
        if(ret_code)
        {
            kdk_config_destroy(module_config);
            module_config = KDK_NULL;
            return ret_code;
        }
    }

    kdk_config_destroy(module_config);
    module_config = KDK_NULL;

    return KDK_SUCCESS;
}
