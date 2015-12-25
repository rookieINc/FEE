
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


module_collection_t *
module_collection_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size, kdk_uint32 prime)
{
    kdk_uint32           mem_pool_type = OTHER_MEM_POOL;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = OWN_MEM_POOL;
    }

    module_collection_t *collection = (module_collection_t *)kdk_mem_pool_malloc(mem_pool, sizeof(module_collection_t));
    if(collection == KDK_NULL)
    {
        if(mem_pool_type == OWN_MEM_POOL)
            kdk_mem_pool_destroy(mem_pool);
        return KDK_NULL;
    }

    kdk_hash_table_t *module_collection = kdk_hash_table_create(mem_pool, 0, prime);
    if(module_collection == KDK_NULL)
    {
        module_collection_destroy(collection);
        return KDK_NULL;
    }

    memset(collection, 0, sizeof(module_collection_t));
    collection->mem_pool_type     = mem_pool_type;
    collection->mem_pool          = mem_pool;
    collection->module_collection = module_collection;

    return collection;
}


kdk_void 
module_collection_destroy(module_collection_t *collection)
{
    if(collection == KDK_NULL)
        return ;

    if(collection->mem_pool != KDK_NULL && collection->mem_pool_type == OWN_MEM_POOL)
    {
        kdk_mem_pool_destroy(collection->mem_pool);
    }

    return ;
}
