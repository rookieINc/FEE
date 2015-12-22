
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : flow_parse.h
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2015-12-01 15:19
 * Description : flow_parse.h
 * History     :     
 ***********************************************************/


#ifndef _FLOW_PARSE_H_INCLUDED_
#define _FLOW_PARSE_H_INCLUDED_

#include "kdk.h"

#define PROCESS_NOR_STATUS      0
#define PROCESS_PRE_STATUS      1
#define PROCESS_ERR_STATUS      2
#define PROCESS_ERR_NOR_STATUS  3

#define NODE_ID_LEN             3
#define BRANCH_ID_LEN           6

#define PROCESS_SUCCESS         0
#define PROCESS_FAILURE         1

#define FLOW_MAIN               0
#define FLOW_ERR                1

struct flow_node_s
{
    kdk_char32             id[NODE_ID_LEN + 1];
    struct flow_node_s    *next;
    struct flow_branch_s  *branch_flow;
};

typedef struct flow_node_s flow_node_t;


struct flow_branch_s
{
    kdk_char32             id[BRANCH_ID_LEN + 1];
    kdk_uint32             node_count;
    struct flow_node_s    *node_list;
    struct flow_branch_s  *branch_next;
};

typedef struct flow_branch_s flow_branch_t; 


struct flow_branch_collection_s
{
    kdk_uint32               mem_pool_type;
    struct kdk_mem_pool_s   *mem_pool;
    struct kdk_hash_table_s *branch_collection;
};

typedef struct flow_branch_collection_s flow_branch_collection_t;


struct flow_branch_runtime_s
{
    kdk_char32              id[BRANCH_ID_LEN + 1];
    kdk_uint32              node_step;
    kdk_uint32              branch_step;
    kdk_uint32              is_main; // FLOW_MAIN FLOW_ERR
    kdk_uint32              mem_pool_type;
    struct kdk_mem_pool_s  *mem_pool;
    struct flow_node_s     *node_current;
    struct flow_branch_s   *branch_current;
    struct flow_branch_s   *branch_main;
};

typedef struct flow_branch_runtime_s flow_branch_runtime_t;



static kdk_uint32 
get_process_status_from_flow_stream(kdk_char32 *flow_stream, kdk_char32 *flow_branch_id, 
                                    kdk_uint32 flow_branch_id_len, kdk_uint32 *process_status, kdk_uint32 *offset);

static flow_node_t *
flow_node_create(flow_branch_collection_t *collection, kdk_char32 *flow_node_id);

static flow_branch_t *
flow_branch_create(flow_branch_collection_t *collection, kdk_char32 *flow_branch_id);

flow_branch_collection_t *
flow_branch_collection_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size, kdk_uint32 prime);

kdk_void 
flow_branch_collection_destroy(flow_branch_collection_t *collection);

kdk_uint32 
flow_branch_set(flow_branch_collection_t *collection, kdk_char32 *flow_branch_id, kdk_char32 *flow_stream);

static flow_branch_t *
flow_branch_get(flow_branch_collection_t *collection, kdk_char32 *flow_branch_id);

flow_branch_runtime_t *
flow_branch_runtime_create(flow_branch_collection_t *collection, kdk_char32 *flow_branch_id);

kdk_uint32
flow_branch_runtime_init(flow_branch_collection_t *collection, flow_branch_runtime_t *runtime);

kdk_void
flow_branch_runtime_clear(flow_branch_runtime_t *runtime);

kdk_void 
flow_branch_runtime_destroy(flow_branch_runtime_t *runtime);

kdk_void 
flow_branch_print(flow_branch_t *main_branch);

#endif /* _FLOW_PARSE_H_INCLUDED_ */
