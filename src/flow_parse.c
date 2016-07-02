
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : flow_parse.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-01 16:48
 * Description : flow_parse.c
 * History     :     
 ***********************************************************/


#include "flow_parse.h"

static kdk_uint32 
get_process_status_from_flow_stream(kdk_char32 *flow_stream, kdk_char32 *flow_id, kdk_uint32 flow_id_len, kdk_uint32 *process_status, kdk_uint32 *offset)
{
    kdk_char32     *flow_id_tail;
    kdk_uint32      new_status;

    if(flow_stream == KDK_NULL || flow_id == KDK_NULL || process_status == KDK_NULL || offset == KDK_NULL) 
        return KDK_INARG;

    *offset = 1;

/*此处应有注释*/
    switch(*process_status)
    {
        case PROCESS_NOR_STATUS:
            flow_id_tail = strpbrk(flow_stream, "-[");
            if(flow_id_tail != KDK_NULL && *flow_id_tail == '[')
                new_status = PROCESS_PRE_STATUS; 
            if(flow_id_tail != KDK_NULL && *flow_id_tail == '-')
                new_status = PROCESS_NOR_STATUS; 
            break;

        case PROCESS_PRE_STATUS:
            flow_id_tail = strpbrk(flow_stream, ",]");
            if(flow_id_tail != KDK_NULL && *flow_id_tail == ',')
                new_status = PROCESS_ERR_STATUS; 
            else if(flow_id_tail != KDK_NULL && *flow_id_tail == ']')
                new_status = PROCESS_NOR_STATUS; 
            break;

        case PROCESS_ERR_STATUS:
        case PROCESS_ERR_NOR_STATUS:
            flow_id_tail = strpbrk(flow_stream, "-]");
            if(flow_id_tail != KDK_NULL && *flow_id_tail == '-')
                new_status = PROCESS_ERR_NOR_STATUS; 
            else if(flow_id_tail != KDK_NULL && *flow_id_tail == ']')
                new_status = PROCESS_NOR_STATUS; 
            break;

        default:
            return KDK_INARG;
    }

/*此处应有注释*/
    if(flow_id_tail == KDK_NULL)
    {
        if(*process_status == PROCESS_NOR_STATUS)
            *offset = *offset + strlen(flow_stream);
        else
            return KDK_INARG;
    }
    else
    {
        *offset = *offset + (flow_id_tail - flow_stream) / sizeof(kdk_char32);
    }

    if(*offset > flow_id_len)
        return KDK_INARG;

/*此处应有注释*/
    strncpy(flow_id, flow_stream, *offset - 1); 
    str_trim(flow_id);

    if(strlen(flow_id) == 0)
    {
/*此处应有代码*/
    }

/*此处应有注释*/
    *process_status = new_status;

    return KDK_SUCCESS;
}


static flow_node_t *
flow_node_create(flow_coll_t *flow_coll, kdk_char32 *flow_node_id)
{
    flow_node_t    *node_new;

    if(flow_node_id == KDK_NULL || flow_coll == KDK_NULL) 
        return KDK_NULL;

    node_new = (flow_node_t *)kdk_mem_pool_malloc(flow_coll->mem_pool, sizeof(flow_node_t));
    if(node_new == KDK_NULL)
        return KDK_NULL;

    memset(node_new->id, 0, NODE_ID_LEN + 1);
    strncpy(node_new->id, flow_node_id, NODE_ID_LEN);
    node_new->next      = KDK_NULL;
    node_new->flow_list = KDK_NULL;

    return node_new;
}


static flow_t *
flow_create(flow_coll_t *flow_coll, kdk_char32 *flow_id)
{
    flow_t   *flow_new;

    if(flow_id == KDK_NULL || flow_coll == KDK_NULL) 
        return KDK_NULL;

    flow_new = (flow_t *)kdk_mem_pool_malloc(flow_coll->mem_pool, sizeof(flow_t));
    if(flow_new == KDK_NULL)
        return KDK_NULL;

    memset(flow_new->id, 0, FLOW_ID_LEN + 1);
    strncpy(flow_new->id, flow_id, FLOW_ID_LEN);
    flow_new->node_count = 0;
    flow_new->node_list  = KDK_NULL;
    flow_new->next       = flow_new;

    return flow_new;
}


flow_coll_t *
flow_coll_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size, kdk_uint32 prime)
{
    kdk_uint32           mem_pool_type = OTHER_MEM_POOL;
    flow_coll_t         *flow_coll;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = OWN_MEM_POOL;
    }

    flow_coll = (flow_coll_t *)kdk_mem_pool_malloc(mem_pool, sizeof(flow_coll_t));
    if(flow_coll == KDK_NULL)
    {
        if(mem_pool_type == OWN_MEM_POOL)
        {
            kdk_mem_pool_destroy(mem_pool);
            mem_pool = KDK_NULL;
        }
        return KDK_NULL;
    }

    memset(flow_coll, 0, sizeof(flow_coll_t));
    flow_coll->mem_pool_type = mem_pool_type;
    flow_coll->mem_pool      = mem_pool;

    flow_coll->coll = kdk_hash_table_create(mem_pool, 0, prime);
    if(flow_coll->coll == KDK_NULL)
    {
        flow_coll_destroy(flow_coll);
        flow_coll = KDK_NULL;
        return KDK_NULL;
    }

    return flow_coll;
}


kdk_uint32 
flow_coll_set(flow_coll_t *flow_coll, kdk_char32 *flow_id, kdk_char32 *flow_stream)
{
    kdk_uint32          ret_code    = 0, 
                        offset      = 0, 
                        stepset     = 0, 
                        status      = PROCESS_NOR_STATUS,
                        pre_status  = status;
    kdk_char32          temp_id[FLOW_ID_LEN + 1] = {0};
    flow_t             *flow_main, *flow_new, *flow_curr;
    flow_node_t        *node_main, *node_new, *node_curr;

    if(flow_coll == KDK_NULL || flow_id == KDK_NULL || flow_stream == KDK_NULL)
        return KDK_INARG;

    flow_main = flow_create(flow_coll, flow_id);
    if(flow_main == KDK_NULL)
        return KDK_NULLPTR;

    while(offset <= strlen(flow_stream))
    {
        pre_status = status;

        memset(temp_id, 0, sizeof(temp_id));
        ret_code = get_process_status_from_flow_stream(flow_stream + offset, temp_id, FLOW_ID_LEN + 1, &status, &stepset);
        if(ret_code)
            return ret_code;

        offset = offset + stepset;

        if(strlen(temp_id) == 0)
            continue;

        if(pre_status == PROCESS_PRE_STATUS && status == PROCESS_ERR_STATUS)
        {
            flow_new = flow_create(flow_coll, temp_id);
            if(flow_new == KDK_NULL)
                return KDK_NULLPTR;

            if(node_curr == KDK_NULL)
                return KDK_NULLPTR;

            flow_new->next       = node_curr->flow_list;
            node_curr->flow_list = flow_new;
            flow_curr            = flow_new;
            node_main            = node_curr;
            node_curr            = flow_new->node_list;
        }
        else if(pre_status == PROCESS_PRE_STATUS && status == PROCESS_NOR_STATUS)
        {
            continue;
        }
        else if(pre_status == PROCESS_ERR_STATUS || pre_status == PROCESS_ERR_NOR_STATUS)
        {
            node_new = flow_node_create(flow_coll, temp_id); 
            if(node_new == KDK_NULL)
                return KDK_NULLPTR;

            if(flow_curr->node_list == KDK_NULL)
                flow_curr->node_list = node_new;
            else
                node_curr->next = node_new;

            node_curr = node_new;

            (flow_curr->node_count)++;

            if(status == PROCESS_NOR_STATUS)
            {
                flow_curr = flow_main;
                node_curr = node_main;
            }
        }
        else if(pre_status == PROCESS_NOR_STATUS)
        {
            node_new = flow_node_create(flow_coll, temp_id); 
            if(node_new == KDK_NULL)
                return KDK_NULLPTR;

            if(flow_main->node_list == KDK_NULL)
                flow_main->node_list = node_new;
            else
                node_main->next = node_new;

            node_main = node_new;

            (flow_main->node_count)++;

            flow_curr = flow_main;
            node_curr = node_main;
        }
        else
        {
            return KDK_NULLPTR;
        }
    }

    ret_code = kdk_hash_table_set_value(flow_coll->coll, flow_id, flow_main, sizeof(flow_t));
    if(ret_code)
        return ret_code;

//    flow_print(flow_main);

    return KDK_SUCCESS;
}


kdk_uint32
flow_coll_get(flow_coll_t *flow_coll, kdk_char32 *flow_id, flow_t *flow)
{
    flow_t   *flow_ret; 

    if(flow_coll == KDK_NULL || flow_id == KDK_NULL || flow == KDK_NULL)
        return KDK_INARG;

    flow_ret = (flow_t *)kdk_hash_table_get_value(flow_coll->coll, flow_id);
    if(flow_ret == KDK_NULL)
        return KDK_NULLPTR;
    else if(flow_ret == KDK_NULLFOUND)
        return KDK_NOTFOUND;

    memset(flow, 0, sizeof(flow_t));
    memcpy(flow, flow_ret, sizeof(flow_t));

    return KDK_SUCCESS;
}


kdk_void 
flow_coll_destroy(flow_coll_t *flow_coll)
{
    if(flow_coll == KDK_NULL)
        return ;
    
    if(flow_coll->coll != KDK_NULL)
    {
        kdk_hash_table_destroy(flow_coll->coll);
        flow_coll->coll = KDK_NULL;
    }

    if(flow_coll->mem_pool != KDK_NULL && flow_coll->mem_pool_type == OWN_MEM_POOL)
    {
        kdk_mem_pool_destroy(flow_coll->mem_pool);
        flow_coll->mem_pool = KDK_NULL;
    }

    return ;
}

kdk_uint32    
flow_config_file_to_flow_coll(kdk_char32 *flow_config_file, flow_coll_t *flow_coll)
{
    kdk_config_t    *flow_config;
    kdk_char32       key[CONFIG_KEY_LEN], value[CONFIG_VALUE_LEN], flow_head[CONFIG_KEY_LEN] = {0};
    kdk_uint32       ret_code = 0, flow_count, flow_count_len, i;

    if(flow_config_file == KDK_NULL || flow_coll == KDK_NULL)
        return KDK_INARG;

    flow_config = kdk_config_create(KDK_NULL, 1024, flow_config_file);
    if(flow_config == KDK_NULL)
        return KDK_NULLPTR;

    ret_code = kdk_config_init(flow_config);
    if(ret_code)
    {
        kdk_config_destroy(flow_config);
        flow_config = KDK_NULL;
        return ret_code;
    }

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(flow_config, HEAD, FLOW_COUNT, value);
    if(ret_code)
    {
        kdk_config_destroy(flow_config);
        flow_config = KDK_NULL;
        return ret_code;
    }
    flow_count = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(flow_config, HEAD, FLOW_COUNT_LEN, value);
    if(ret_code)
    {
        kdk_config_destroy(flow_config);
        flow_config = KDK_NULL;
        return ret_code;
    }
    flow_count_len = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(flow_config, HEAD, FLOW_HEAD, value);
    if(ret_code)
    {
        kdk_config_destroy(flow_config);
        flow_config = KDK_NULL;
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
            flow_config = KDK_NULL;
            return ret_code;
        }
        else if(ret_code == KDK_NOTFOUND)
        {
            continue ;
        }

        ret_code = flow_coll_set(flow_coll, key, value);
        if(ret_code)
        {
            kdk_config_destroy(flow_config);
            flow_config = KDK_NULL;
            return ret_code;
        }
/*
        flow_branch_t * flow = flow_branch_get(flow_coll, key);
        if(flow != KDK_NULL)
            flow_branch_print(flow);
*/
    }

    kdk_config_destroy(flow_config);
    flow_config = KDK_NULL;

    return KDK_SUCCESS;
}

kdk_void 
flow_print(flow_t *flow_main)
{
    kdk_uint32      i = 0;
    flow_t         *flow_curr;
    flow_node_t    *node_curr, *node_sub_curr, *node_main;

    if(flow_main == KDK_NULL)
        return ;

    flow_curr = flow_main;
    node_curr = flow_main->node_list;
    node_main = flow_main->node_list;

    fprintf(stderr, "\n");
    fprintf(stderr,     "[FLOWID:%s]\n", flow_curr->id);
    while(node_main != KDK_NULL)
    {
        fprintf(stderr, "       |\n");
        fprintf(stderr, "     [%3s]\n", node_main->id);

        node_curr = node_main;
        flow_curr = node_curr->flow_list;

        while(flow_curr != KDK_NULL)
        {
            if(i == 0)
            {
                fprintf(stderr, "       |---[ERR:%3s]", flow_curr->id);
            }
            else
            {
                fprintf(stderr, "       |       |\n");
                fprintf(stderr, "       |   [ERR:%3s]", flow_curr->id);
            }
            i++;

            node_sub_curr = flow_curr->node_list; 

            while(node_sub_curr != KDK_NULL)
            {
                fprintf(stderr, "-[%3s]", node_sub_curr->id);
                node_sub_curr = node_sub_curr->next;
            }
            fprintf(stderr, "\n");
            flow_curr = flow_curr->next;
        }
        i = 0;

        node_main = node_main->next;
    }
    fprintf(stderr, "       |\n");
    fprintf(stderr, "     [end]\n");

    return ;
}
