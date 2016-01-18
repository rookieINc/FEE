
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
#define FLOW_ID_LEN             6

#define PROCESS_SUCCESS         0
#define PROCESS_FAILURE         1

#define FLOW_MAIN               0
#define FLOW_ERR                1

struct flow_node_s
{
    kdk_char32             id[NODE_ID_LEN + 1];
    struct flow_node_s    *next;
    struct flow_s         *flow_list;
};

typedef struct flow_node_s flow_node_t;

struct flow_s
{
    kdk_char32             id[FLOW_ID_LEN + 1];
    kdk_uint32             node_count;
    struct flow_node_s    *node_list;
    struct flow_s         *next;
};

typedef struct flow_s flow_t; 

struct flow_coll_s
{
    kdk_uint32               mem_pool_type;
    struct kdk_mem_pool_s   *mem_pool;
    struct kdk_hash_table_s *coll;
};

typedef struct flow_coll_s flow_coll_t;

static kdk_uint32 
get_process_status_from_flow_stream(kdk_char32 *flow_stream, kdk_char32 *flow_id, kdk_uint32 flow_id_len, kdk_uint32 *process_status, kdk_uint32 *offset);

static flow_node_t *
flow_node_create(flow_coll_t *flow_coll, kdk_char32 *flow_node_id);

static flow_t *
flow_create(flow_coll_t *flow_coll, kdk_char32 *flow_id);

flow_coll_t *
flow_coll_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size, kdk_uint32 prime);

kdk_uint32 
flow_coll_set(flow_coll_t *flow_coll, kdk_char32 *flow_id, kdk_char32 *flow_stream);

kdk_uint32
flow_coll_get(flow_coll_t *flow_coll, kdk_char32 *flow_id, flow_t *flow_branch);

kdk_void 
flow_coll_destroy(flow_coll_t *flow_coll);

kdk_void 
flow_print(flow_t *flow);

#endif /* _FLOW_PARSE_H_INCLUDED_ */
