
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

/*
flow_data_t *
flow_data_init(kdk_void (*fill_data)(flow_data_t *))
{
    flow_data_t *data;

    data = (flow_data_t *)kdk_malloc(sizeof(flow_data_t));
    if(data == KDK_NULL)
        return KDK_NULL;
    
    fill_data(data);

    return data;
}

kdk_void 
flow_branch_runtime_clear(flow_branch_t *branch)
{
    if(branch == KDK_NULL) 
        return ;

    branch->node_current   = branch->node_list;
    branch->branch_current = branch;
    branch->step = 0;

    return ;
}
*/

static kdk_uint32 
get_process_status_from_flow_stream(kdk_char32 *flow_stream, kdk_char32 *flow_branch_id, kdk_uint32 flow_branch_id_len, kdk_uint32 *process_status, kdk_uint32 *offset)
{
    kdk_char32     *flow_branch_id_tail;
    kdk_uint32      new_status;

    if(flow_stream == KDK_NULL || flow_branch_id == KDK_NULL || process_status == KDK_NULL || offset == KDK_NULL) 
        return KDK_INARG;

    *offset = 1;

/*此处应有注释*/
    switch(*process_status)
    {
        case PROCESS_NOR_STATUS:
            flow_branch_id_tail = strpbrk(flow_stream, "-[");
            if(flow_branch_id_tail != KDK_NULL && *flow_branch_id_tail == '[')
                new_status = PROCESS_PRE_STATUS; 
            if(flow_branch_id_tail != KDK_NULL && *flow_branch_id_tail == '-')
                new_status = PROCESS_NOR_STATUS; 
            break;

        case PROCESS_PRE_STATUS:
            flow_branch_id_tail = strpbrk(flow_stream, ",]");
            if(flow_branch_id_tail != KDK_NULL && *flow_branch_id_tail == ',')
                new_status = PROCESS_ERR_STATUS; 
            else if(flow_branch_id_tail != KDK_NULL && *flow_branch_id_tail == ']')
                new_status = PROCESS_NOR_STATUS; 
            break;

        case PROCESS_ERR_STATUS:
        case PROCESS_ERR_NOR_STATUS:
            flow_branch_id_tail = strpbrk(flow_stream, "-]");
            if(flow_branch_id_tail != KDK_NULL && *flow_branch_id_tail == '-')
                new_status = PROCESS_ERR_NOR_STATUS; 
            else if(flow_branch_id_tail != KDK_NULL && *flow_branch_id_tail == ']')
                new_status = PROCESS_NOR_STATUS; 
            break;

        default:
            return KDK_INARG;
    }

/*此处应有注释*/
    if(flow_branch_id_tail == KDK_NULL)
    {
        if(*process_status == PROCESS_NOR_STATUS)
            *offset = *offset + strlen(flow_stream);
        else
            return KDK_INARG;
    }
    else
    {
        *offset = *offset + (flow_branch_id_tail - flow_stream) / sizeof(kdk_char32);
    }

    if(*offset > flow_branch_id_len)
        return KDK_INARG;

/*此处应有注释*/
    strncpy(flow_branch_id, flow_stream, *offset - 1); 
    str_trim(flow_branch_id);

    if(strlen(flow_branch_id) == 0)
    {
/*此处应有代码*/
    }

/*此处应有注释*/
    *process_status = new_status;

    return KDK_SUCCESS;
}


static flow_node_t *
flow_node_create(flow_branch_collection_t *collection, kdk_char32 *flow_node_id)
{
    if(flow_node_id == KDK_NULL) 
        return KDK_NULL;

    flow_node_t *new_node = (flow_node_t *)kdk_mem_pool_malloc(collection->mem_pool, sizeof(flow_node_t));
    if(new_node == KDK_NULL)
        return KDK_NULL;

    memset(new_node->id, 0, NODE_ID_LEN + 1);
    strncpy(new_node->id, flow_node_id, NODE_ID_LEN);
    new_node->next        = KDK_NULL;
    new_node->branch_flow = KDK_NULL;

    return new_node;
}


static flow_branch_t *
flow_branch_create(flow_branch_collection_t *collection, kdk_char32 *flow_branch_id)
{
    if(flow_branch_id == KDK_NULL) 
        return KDK_NULL;

    flow_branch_t *new_branch = (flow_branch_t *)kdk_mem_pool_malloc(collection->mem_pool, sizeof(flow_branch_t));
    if(new_branch == KDK_NULL)
        return KDK_NULL;

    memset(new_branch->id, 0, BRANCH_ID_LEN + 1);
    strncpy(new_branch->id, flow_branch_id, BRANCH_ID_LEN);
    new_branch->node_count    = 0;
    new_branch->node_list     = KDK_NULL;
    new_branch->branch_next   = KDK_NULL;

    return new_branch;
}


flow_branch_collection_t *
flow_branch_collection_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size, kdk_uint32 prime)
{
    kdk_uint32           mem_pool_type = 1;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = 0;
    }

    flow_branch_collection_t *collection = (flow_branch_collection_t *)kdk_mem_pool_malloc(mem_pool, sizeof(flow_branch_collection_t));
    if(collection == KDK_NULL)
    {
        if(mem_pool_type == 0)
            kdk_mem_pool_destroy(mem_pool);
        return KDK_NULL;
    }

    kdk_hash_table_t *branch_collection = kdk_hash_table_create(mem_pool, 0, prime);
    if(branch_collection == KDK_NULL)
    {
        flow_branch_collection_destroy(collection);
        return KDK_NULL;
    }

    memset(collection, 0, sizeof(flow_branch_collection_t));
    collection->mem_pool_type     = mem_pool_type;
    collection->mem_pool          = mem_pool;
    collection->branch_collection = branch_collection;

    return collection;
}


kdk_void 
flow_branch_collection_destroy(flow_branch_collection_t *collection)
{
    if(collection == KDK_NULL)
        return ;

    if(collection->mem_pool != KDK_NULL && collection->mem_pool_type == 0)
    {
        kdk_mem_pool_destroy(collection->mem_pool);
    }

    return ;
}


kdk_uint32 
flow_branch_set(flow_branch_collection_t *collection, kdk_char32 *flow_branch_id, kdk_char32 *flow_stream)
{
    kdk_uint32          ret_code    = 0, 
                        offset      = 0, 
                        stepset     = 0, 
                        status      = PROCESS_NOR_STATUS,
                        pre_status  = status;
    kdk_char32          temp_id[BRANCH_ID_LEN + 1] = {0};
    //flow_branch_t      *main_branch, *branch, *branch_current;
    //flow_node_t        *node, *node_current, *node_main_current;
    flow_branch_t       *branch_main;
    flow_branch_t       *branch_new;
    flow_branch_t       *branch_current;
    flow_node_t         *node_new;
    flow_node_t         *node_current;
    flow_node_t         *node_current_main;

    if(collection == KDK_NULL || flow_branch_id == KDK_NULL || flow_stream == KDK_NULL)
        return KDK_INARG;

    branch_main = flow_branch_create(collection, flow_branch_id);
    if(branch_main == KDK_NULL)
        return KDK_NULLPTR;

/*
    branch_current = main_branch;
    node_current   = main_branch->node_list;
    node_main_current = node_current;
*/

    int i = 0;
    while(offset <= strlen(flow_stream))
    {
        pre_status = status;

        memset(temp_id, 0, sizeof(temp_id));
        ret_code = get_process_status_from_flow_stream(flow_stream + offset, temp_id, BRANCH_ID_LEN + 1, &status, &stepset);
        if(ret_code)
            return ret_code;

        offset = offset + stepset;

        if(strlen(temp_id) == 0)
            continue;



        if(pre_status == PROCESS_PRE_STATUS && status == PROCESS_ERR_STATUS)
        {
            branch_new = flow_branch_create(collection, temp_id);
            if(branch_new == KDK_NULL)
                return KDK_NULLPTR;

            if(node_current == KDK_NULL)
                return KDK_NULLPTR;

            branch_new->branch_next   = node_current->branch_flow;
            node_current->branch_flow = branch_new;
            branch_current            = branch_new;
            node_current_main         = node_current;
            node_current              = branch_new->node_list;
        }
        else if(pre_status == PROCESS_PRE_STATUS && status == PROCESS_NOR_STATUS)
        {
            continue;
        }
        else if(pre_status == PROCESS_ERR_STATUS || pre_status == PROCESS_ERR_NOR_STATUS)
        {
            node_new = flow_node_create(collection, temp_id); 
            if(node_new == KDK_NULL)
                return KDK_NULLPTR;

            if(branch_current->node_list == KDK_NULL)
                branch_current->node_list = node_new;
            else
                node_current->next = node_new;

            node_current = node_new;

            (branch_current->node_count)++;

            if(status == PROCESS_NOR_STATUS)
            {
                branch_current = branch_main;
                node_current   = node_current_main;
            }
        }
        else if(pre_status == PROCESS_NOR_STATUS)
        {
            node_new = flow_node_create(collection, temp_id); 
            if(node_new == KDK_NULL)
                return KDK_NULLPTR;

            if(branch_main->node_list == KDK_NULL)
                branch_main->node_list = node_new;
            else
                node_current_main->next = node_new;

            node_current_main = node_new;

            (branch_main->node_count)++;

            branch_current = branch_main;
            node_current   = node_current_main;
        }
        else
        {
            return KDK_NULLPTR;
        }
    }

    ret_code = kdk_hash_table_set_value(collection->branch_collection, flow_branch_id, branch_main, sizeof(flow_branch_t));
    if(ret_code)
        return ret_code;

    return KDK_SUCCESS;
}


static flow_branch_t *
flow_branch_get(flow_branch_collection_t *collection, kdk_char32 *flow_branch_id)
{
    return (flow_branch_t *)kdk_hash_table_get_value(collection->branch_collection, flow_branch_id);
}

flow_branch_runtime_t *
flow_branch_runtime_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size,  kdk_char32 *flow_branch_id)
{
    kdk_uint32           mem_pool_type = 1;

    if(flow_branch_id == KDK_NULL)
        return KDK_NULL;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = 0;
    }

    flow_branch_runtime_t *runtime = (flow_branch_runtime_t *)kdk_mem_pool_malloc(mem_pool, sizeof(flow_branch_runtime_t));
    if(runtime == KDK_NULL)
    {
        if(mem_pool_type == 0)
            kdk_mem_pool_destroy(mem_pool);
        return KDK_NULL;
    }

    memset(runtime->id, 0, BRANCH_ID_LEN + 1);
    strncpy(runtime->id, flow_branch_id, BRANCH_ID_LEN);
    runtime->is_main        = FLOW_MAIN;
    runtime->node_step      = 0;
    runtime->branch_step    = 0;
    runtime->mem_pool_type  = mem_pool_type;
    runtime->mem_pool       = mem_pool;
    runtime->node_current   = KDK_NULL;
    runtime->branch_current = KDK_NULL;
    runtime->branch_main    = KDK_NULL;

    return runtime;
    
}


kdk_uint32
flow_branch_runtime_init(flow_branch_collection_t *collection, flow_branch_runtime_t *runtime)
{
    flow_branch_t   *branch_main;  

    branch_main = flow_branch_get(collection, runtime->id);
    if(branch_main == KDK_NULL)
        return KDK_NULLPTR;

    runtime->node_current   = branch_main->node_list;
    runtime->branch_current = branch_main;
    runtime->branch_main    = branch_main;
        

    return KDK_SUCCESS;
}


kdk_void
flow_branch_runtime_clear(flow_branch_runtime_t *runtime)
{
    if(runtime == KDK_NULL)
        return ;
    
    memset(runtime->id, 0, BRANCH_ID_LEN + 1);
    runtime->is_main        = FLOW_MAIN;
    runtime->node_step      = 0;
    runtime->branch_step    = 0;
    runtime->mem_pool_type  = mem_pool_type;
    runtime->mem_pool       = mem_pool;
    runtime->node_current   = KDK_NULL;
    runtime->branch_current = KDK_NULL;
    runtime->branch_main    = KDK_NULL;

    return ;
}


/*
 * isSuccess : 0 - PROCESS_SUCCESS
 *             1 - PROCESS_FAILURE
 * If isSuccess is FAILURE, err_id shouldn't be KDK_NULL;
 * RETURN VALUE: KDK_SUCCESS  - exec success
 *             : KDK_NOTFOUND - exec success && not any NODE to exec
 *             : KDK_FAILURE  - exec failure && not any BRANCH / NODE to exec
 */
kdk_uint32
flow_branch_runtime_next(flow_branch_runtime_t *runtime, kdk_uint32 isSuccess, kdk_char32 *err_id, kdk_char32 *node_id)
{
    if(runtime == KDK_NULL || node_id == KDK_NULL)
        return KDK_NULLPTR;
    
    switch(isSuccess)
    {
        case PROCESS_SUCCESS:
                if(runtime->node_current == KDK_NULL)
                    return KDK_FAILURE;

                strncpy(node_id, runtime->node_current->id, NODE_ID_LEN);
                if((runtime->node_current = runtime->node_current->next) == KDK_NULL)
                    return KDK_NOTFOUND;
                break;

        case PROCESS_FAILURE:
                runtime->is_main = FLOW_ERR;

                if(err_id == KDK_NULL)
                    return KDK_INARG;

                runtime->branch_current = runtime->node_current->branch_flow;
                while(runtime->branch_current != KDK_NULL)
                {
                    if(strcmp(runtime->branch_current->id, err_id) == 0)
                    {
                        runtime->node_current = runtime->branch_current->node_list;
                        return flow_branch_runtime_next(runtime, isSuccess, KDK_NULL, node_id);
                    }
                    runtime->branch_current = runtime->branch_current->branch_next;    
                }
                return KDK_FAILURE;
                
                break;

        default:
            return KDK_INARG;
    }

    if(runtime->is_main == FLOW_MAIN)
        (runtime->node_step)++;

    return KDK_SUCCESS;
}


kdk_void 
flow_branch_runtime_destroy(flow_branch_runtime_t *runtime)
{
    if(runtime == KDK_NULL)
        return ;

    if(runtime->mem_pool != KDK_NULL && runtime->mem_pool_type == 0)
    {
        kdk_mem_pool_destroy(runtime->mem_pool);
    }

    return ;
}


kdk_void 
flow_branch_print(flow_branch_t *main_branch)
{
    kdk_uint32      i = 0;
    flow_branch_t   *branch_current;
    flow_node_t     *node_current, *node_sub_current, *node_current_main;

    if(main_branch == KDK_NULL)
        return ;

    branch_current = main_branch;
    node_current   = main_branch->node_list;
    node_current_main = main_branch->node_list;

    fprintf(stderr, "\n");
    fprintf(stderr,     "[FLOWID:%s]\n", branch_current->id);
    while(node_current_main != KDK_NULL)
    {
        fprintf(stderr, "       |\n");
        fprintf(stderr, "     [%3s]\n", node_current_main->id);

        node_current = node_current_main;
        branch_current = node_current->branch_flow;

        while(branch_current != KDK_NULL)
        {
            if(i == 0)
            {
                fprintf(stderr, "       |---[ERR:%3s]", branch_current->id);
            }
            else
            {
                fprintf(stderr, "       |       |\n");
                fprintf(stderr, "       |   [ERR:%3s]", branch_current->id);
            }
            i++;

            node_sub_current = branch_current->node_list; 

            while(node_sub_current != KDK_NULL)
            {
                fprintf(stderr, "-[%3s]", node_sub_current->id);
                node_sub_current = node_sub_current->next;
            }
            fprintf(stderr, "\n");
            branch_current = branch_current->branch_next;
        }
        i = 0;

        node_current_main = node_current_main->next;
    }
    fprintf(stderr, "       |\n");
    fprintf(stderr, "     [end]\n");

    return ;
}



