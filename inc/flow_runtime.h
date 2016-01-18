
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : flow_runtime.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-01 15:19
 * Description : flow_runtime.h
 * History     :     
 ***********************************************************/


#ifndef _FLOW_RUNTIME_H_INCLUDED_
#define _FLOW_RUNTIME_H_INCLUDED_

#include "kdk.h"
#include "flow_parse.h"

struct flow_runtime_s
{
    kdk_char32              id[FLOW_ID_LEN + 1];
    kdk_uint32              node_step;
    kdk_uint32              flow_step;
    kdk_uint32              is_main; 
    kdk_uint32              mem_pool_type;
    struct kdk_mem_pool_s  *mem_pool;
    struct flow_node_s     *node_curr;
    struct flow_node_s     *node_prev;  
    struct flow_s          *flow_curr;
};

typedef struct flow_runtime_s flow_runtime_t;

flow_runtime_t *
flow_runtime_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size);

kdk_uint32
flow_runtime_init(flow_runtime_t *flow_runtime, flow_coll_t *flow_coll, kdk_char32 *flow_id);

kdk_uint32
flow_runtime_next(flow_runtime_t *flow_runtime, kdk_uint32 is_succ, kdk_char32 *err_id, kdk_char32 *node_id);

kdk_void
flow_runtime_clear(flow_runtime_t *flow_runtime);

kdk_void 
flow_runtime_destroy(flow_runtime_t *flow_runtime);

#endif /* _FLOW_RUNTIME_H_INCLUDED_ */
