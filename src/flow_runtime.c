
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : flow_runtime.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-01 16:48
 * Description : flow_runtime.c
 * History     :     
 ***********************************************************/


#include "flow_runtime.h"

flow_runtime_t *
flow_runtime_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size) 
{
    kdk_uint32           mem_pool_type = OTHER_MEM_POOL;
    flow_runtime_t      *flow_runtime;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = OWN_MEM_POOL;
    }

    flow_runtime = (flow_runtime_t *)kdk_mem_pool_malloc(mem_pool, sizeof(flow_runtime_t));
    if(flow_runtime == KDK_NULL)
    {
        if(mem_pool_type == OWN_MEM_POOL)
        {
            kdk_mem_pool_destroy(mem_pool);
            mem_pool = KDK_NULL;
        }
        return KDK_NULL;
    }

    flow_runtime->mem_pool_type  = mem_pool_type;
    flow_runtime->mem_pool       = mem_pool;
    flow_runtime->node_curr      = KDK_NULL;
    flow_runtime->node_prev      = KDK_NULL;
    flow_runtime->flow_curr      = KDK_NULL;

    return flow_runtime;
    
}


kdk_uint32
flow_runtime_init(flow_runtime_t *flow_runtime, flow_coll_t *flow_coll, kdk_char32 *flow_id)
{
    kdk_uint32      ret_code;
    flow_t          flow_main;  

    if(flow_id == KDK_NULL || flow_runtime == KDK_NULL)
        return KDK_INARG;

    ret_code = flow_coll_get(flow_coll, flow_id, &flow_main);
    if(ret_code)
        return ret_code;

    memset(flow_runtime->id, 0, FLOW_ID_LEN + 1);
    strncpy(flow_runtime->id, flow_id, FLOW_ID_LEN);
    flow_runtime->is_main   = FLOW_MAIN;
    flow_runtime->node_step = 0;
    flow_runtime->flow_step = 0;
    flow_runtime->node_curr = flow_main.node_list;
    flow_runtime->node_prev = flow_main.node_list;
    flow_runtime->flow_curr = flow_main.next;

    return KDK_SUCCESS;
}


/*
 * is_succs : 0 - PROCESS_SUCCESS
 *            1 - PROCESS_FAILURE
 * If is_succs is FAILURE, err_id shouldn't be KDK_NULL;
 * RETURN VALUE: KDK_SUCCESS  - exec success
 *             : KDK_NOTFOUND - exec success && not any NODE to exec
 *             : KDK_FAILURE  - exec failure && not any FLOW to exec
 */
kdk_uint32
flow_runtime_next(flow_runtime_t *flow_runtime, kdk_uint32 is_succs, kdk_char32 *err_id, kdk_char32 *node_id)
{
    if(flow_runtime == KDK_NULL || node_id == KDK_NULL)
        return KDK_NULLPTR;
    
    switch(is_succs)
    {
        case PROCESS_SUCCESS:
                if(flow_runtime->node_curr == KDK_NULL)
                    return KDK_NOTFOUND;

                strncpy(node_id, flow_runtime->node_curr->id, NODE_ID_LEN);
                flow_runtime->node_prev  = flow_runtime->node_curr;
                flow_runtime->node_curr  = flow_runtime->node_curr->next;
                break;

        case PROCESS_FAILURE:
                flow_runtime->is_main = FLOW_ERR;

                if(err_id == KDK_NULL)
                    return KDK_INARG;

                flow_runtime->flow_curr = flow_runtime->node_prev->flow_list;
                while(flow_runtime->flow_curr != KDK_NULL)
                {
                    if(strcmp(flow_runtime->flow_curr->id, err_id) == 0)
                    {
                        flow_runtime->node_curr = flow_runtime->flow_curr->node_list;
                        return flow_runtime_next(flow_runtime, PROCESS_SUCCESS, KDK_NULL, node_id);
                    }
                    flow_runtime->flow_curr = flow_runtime->flow_curr->next;    
                }
                return KDK_FAILURE;
                
                break;

        default:
            return KDK_INARG;
    }

    if(flow_runtime->is_main == FLOW_MAIN)
        (flow_runtime->node_step)++;

    return KDK_SUCCESS;
}


kdk_void
flow_runtime_clear(flow_runtime_t *flow_runtime)
{
    if(flow_runtime == KDK_NULL)
        return ;
    
    memset(flow_runtime->id, 0, FLOW_ID_LEN + 1);
    flow_runtime->is_main       = FLOW_MAIN;
    flow_runtime->node_step     = 0;
    flow_runtime->flow_step     = 0;
//    flow_runtime->mem_pool_type = OTHER_MEM_POOL;
//    flow_runtime->mem_pool      = KDK_NULL;
    flow_runtime->node_curr     = KDK_NULL;
    flow_runtime->node_prev     = KDK_NULL;
    flow_runtime->flow_curr     = KDK_NULL;

    return ;
}


kdk_void 
flow_runtime_destroy(flow_runtime_t *flow_runtime)
{
    if(flow_runtime == KDK_NULL)
        return ;

    if(flow_runtime->mem_pool != KDK_NULL && flow_runtime->mem_pool_type == OWN_MEM_POOL)
    {
        kdk_mem_pool_destroy(flow_runtime->mem_pool);
        flow_runtime->mem_pool = KDK_NULL;
    }

    return ;
}
