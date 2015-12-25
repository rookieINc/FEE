
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : flow_load.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-24 16:00
 * Description : flow_load.c
 * History     : 
 ***********************************************************/


#include "flow_load.h"

kdk_uint32    
flow_config_to_flow_branch_collection(kdk_char32 *file_name, flow_branch_collection_t *collection)
{
    kdk_config_t    *flow_config;
    kdk_char32       key[CONFIG_KEY_LEN], value[CONFIG_VALUE_LEN], flow_head[CONFIG_KEY_LEN] = {0};
    kdk_uint32       ret_code = 0, flow_count, flow_count_len, i;

    if(file_name == KDK_NULL || collection == KDK_NULL)
        return KDK_INARG;

    flow_config = kdk_config_create(KDK_NULL, 1024, file_name);
    if(flow_config == KDK_NULL)
        return KDK_NULLPTR;

    ret_code = kdk_config_parse(flow_config);
    if(ret_code)
    {
        kdk_config_destroy(flow_config);
        return ret_code;
    }

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(flow_config, HEAD, FLOW_COUNT, value);
    if(ret_code)
    {
        kdk_config_destroy(flow_config);
        return ret_code;
    }
    flow_count = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(flow_config, HEAD, FLOW_COUNT_LEN, value);
    if(ret_code)
    {
        kdk_config_destroy(flow_config);
        return ret_code;
    }
    flow_count_len = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(flow_config, HEAD, FLOW_HEAD, value);
    if(ret_code)
    {
        kdk_config_destroy(flow_config);
        return ret_code;
    }
    strncpy(flow_head, value, strlen(value));

    for(i = 1; i <= flow_count; i++)
    {
        memset(key, 0, sizeof(key));
        snprintf(key, CONFIG_KEY_LEN, "%s%0*d", flow_head, flow_count_len, i);  

        memset(value, 0, sizeof(value));
        ret_code = kdk_config_get_value(flow_config, CONTENT, key, value);
        if(ret_code && ret_code != KDK_NOTFOUND)
        {
            kdk_config_destroy(flow_config);
            return ret_code;
        }
        else if(ret_code == KDK_NOTFOUND)
        {
            continue ;
        }

        ret_code = flow_branch_set(collection, key, value);
        if(ret_code)
        {
            kdk_config_destroy(flow_config);
            return ret_code;
        }
    }

    kdk_config_destroy(flow_config);

    return KDK_SUCCESS;
}
