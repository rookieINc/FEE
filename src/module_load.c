
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : module_load.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-25 08:27
 * Description : module_load.c
 * History     : 
 ***********************************************************/


#include "module_load.h"

static kdk_uint32
module_config_to_module(kdk_config_t *module_config, kdk_char32 *section, module_t *module)
{
    kdk_uint32  ret_code;

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
module_config_to_module_collection(kdk_char32 *file_name, module_collection_t *collection)
{
    kdk_config_t    *module_config;
    module_t         module;
    kdk_char32       section[CONFIG_KEY_LEN], value[CONFIG_VALUE_LEN];
    kdk_uint32       ret_code = 0, module_count, module_count_len, i;

    if(file_name == KDK_NULL || collection == KDK_NULL)
        return KDK_INARG;

    module_config = kdk_config_create(KDK_NULL, 1024, file_name);
    if(module_config == KDK_NULL)
        return KDK_NULLPTR;

    ret_code = kdk_config_parse(module_config);
    if(ret_code)
    {
        kdk_config_destroy(module_config);
        return ret_code;
    }

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(module_config, HEAD, MODULE_TYPE, value);
    if(ret_code)
    {
        kdk_config_destroy(module_config);
        return ret_code;
    }
    collection->module_type = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(module_config, HEAD, MODULE_COUNT, value);
    if(ret_code)
    {
        kdk_config_destroy(module_config);
        return ret_code;
    }
    module_count = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(module_config, HEAD, MODULE_COUNT_LEN, value);
    if(ret_code)
    {
        kdk_config_destroy(module_config);
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
            return ret_code;
        }
        else if(ret_code == KDK_NOTFOUND)
        {
            continue;
        }

        ret_code = module_set(collection, section, &module);
        if(ret_code)
        {
            kdk_config_destroy(module_config);
            return ret_code;
        }
    }

    kdk_config_destroy(module_config);

    return KDK_SUCCESS;
}
