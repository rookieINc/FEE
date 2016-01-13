#include "flow_parse.h"
#include "module_parse.h"
#include "kdk_config.h"
#include "kdk_dl.h"

int main(int argc, char *argv[])
{
    kdk_uint32                   ret_code, i;
    flow_branch_collection_t    *collection;
    module_collection_t         *module_collection;
    flow_runtime_t              *runtime;
    module_t                     module;
    kdk_void (*func_test)(kdk_void);
        kdk_char32                   path_file[48] = {0};
        kdk_dl_handle_collection_t  *dl_handle_collection;
        kdk_dl_handle_t              dl_handle;

    collection = flow_branch_collection_create(NULL, 1024, 101);
    if(collection == KDK_NULL)
    {
        fprintf(stderr, "flow_branch_collection_create error!\n");
        return -1;
    }
    fprintf(stderr, "flow_branch_collection_create success!\n");

    ret_code = flow_config_to_flow_branch_collection(argv[1], collection);
    if(ret_code)
    {
        fprintf(stderr, "flow_config_to_flow_branch_collection error!\n");
        return -1;
    }
    fprintf(stderr, "flow_config_to_flow_branch_collection success!\n");

    module_collection = module_collection_create(NULL, 1024, 101);
    if(module_collection == KDK_NULL)
    {
        fprintf(stderr, "module_collection_create error!\n");
        return -1;
    }
    fprintf(stderr, "module_collection_create success!\n");

    ret_code = module_config_to_module_collection(argv[2], module_collection);
    if(ret_code)
    {
        fprintf(stderr, "module_config_to_module_collection error!\n");
        return -1;
    }
    fprintf(stderr, "module_config_to_module_collection success!\n");

    runtime = flow_runtime_create(collection->mem_pool, 0);
    if(runtime == KDK_NULL)
    {
        fprintf(stderr, "flow_runtime_create error!\n");
        return -1;
    }
    fprintf(stderr, "flow_runtime_create success!\n");

    ret_code = flow_runtime_init(collection, runtime, "SERV01");
    if(ret_code)
    {
        fprintf(stderr, "flow_runtime_init error!\n");
        return -1;
    }
    fprintf(stderr, "flow_runtime_init success!\n");

    dl_handle_collection = kdk_dl_handle_collection_create(KDK_NULL, 4096);
    if(dl_handle_collection == KDK_NULL)
    {
        fprintf(stderr, "kdk_dl_handle_collection_create error!\n");
        return -1;
    }

    kdk_char32  node_id[7] = {0};
    kdk_uint32  isSuccess = 0;
    kdk_char32  err_code[5] = {0};
    while(1)
    {
        fprintf(stderr, "Success:%d\n", isSuccess);
        fprintf(stderr, "err_code:%s\n", err_code);
        ret_code = flow_runtime_next(runtime, isSuccess, err_code, node_id);
        if(ret_code == KDK_NOTFOUND)
        {
            fprintf(stderr, "flow_runtime_next finish!\n");
            break;
        }
        else if(ret_code == KDK_FAILURE)
        {
            fprintf(stderr, "flow_runtime_next no branch to exec!\n");
            break;
        }
        else if(ret_code != KDK_SUCCESS)
        {
            fprintf(stderr, "flow_runtime_next failure!\n");
            break;
        }

        ret_code = module_get(module_collection, node_id, &module);
        if(ret_code == KDK_NULLPTR && ret_code != KDK_NOTFOUND)
        {
            fprintf(stderr, "module_get error!\n");
            break;
        }
        else if(ret_code == KDK_NOTFOUND)
        {
            fprintf(stderr, "no module!\n");
            continue;
        }

/*
        fprintf(stderr, "[ID:%s]\n", module->id);
        fprintf(stderr, "[PATH:%s]\n", module->path);
        fprintf(stderr, "[FILE:%s]\n", module->file_name);
        fprintf(stderr, "[FUNC:%s]\n", module->func_name);
*/

        memset(path_file, 0, sizeof(path_file));
        sprintf(path_file, "%s/%s.so", module.path, module.file_name);

        ret_code = kdk_dl_handle_collection_set(path_file, module.func_name, dl_handle_collection);
        if(ret_code)
        {
            fprintf(stderr, "kdk_dl_handle_collection_set error!\n");
            continue;
        }

        memset(&dl_handle, 0, sizeof(kdk_dl_handle_t));
        ret_code = kdk_dl_handle_collection_get(dl_handle_collection, module.func_name, &dl_handle);
        if(ret_code)
        {
            fprintf(stderr, "kdk_dl_handle_collection_get error!\n");
            break;
        }

        fprintf(stderr, "dl_handle:%s\n", dl_handle.func_name);
/*
        func_test = kdk_dl_open(path_file, module.func_name);
        if(func_test == KDK_NULL)
           fprintf(stderr, "kdk_dl_open error!\n"); 

        kdk_dl_close();
*/

        memset(node_id, 0, sizeof(node_id));
        memset(err_code, 0, sizeof(err_code));

        fprintf(stderr, "Enter success flag:\n");
        scanf("%d", &isSuccess);
        fprintf(stderr, "Enter errCode flag:\n");
        scanf("%s", err_code);
    }

    kdk_dl_handle_collection_destroy(dl_handle_collection);

/*
    kdk_char32  flow_stream1[1024] = "001-002[DEF,00E-00F-00G]-003[DEF,00E-00F-00G][092,00G-00H][DEF,00E-00G]-004[DEF,00E-00F-00G]-005[DEF,00E-00F-00G][DEF,00E-00F-00G]-006";
    kdk_char32  flow_stream2[1024] = "001-002[DEF,00E-00F-00G]-003[DEF,00E-00F-00G-004]-004[DEF,00E-00F-00G]-005[DEF,00E-00F-00G][092,003-004]-006";
    kdk_char32  flow_stream3[1024] = "001-002-003-004-005-006";
    kdk_char32  flow_stream4[1024] = "001-002-003[DEF,00E-00F-00G][092,00G-00H][DEF,00E-00F-00G]-004-005-006";
*/

    return 0;
}
