#include "engine_exec.h"

int main(int argc, char *argv[])
{
    engine_t   *engine;
    kdk_char32  node_id[7] = {0};
    kdk_uint32  ret_code, i, is_succ;
    module_t    module;
    kdk_dl_handle_t dl_handle;   
    kdk_char32  path_file[124];

    engine = engine_create(KDK_NULL, 4096);
    if(engine == KDK_NULL)
    {
        fprintf(stderr, "engine_create error!\n");
        return KDK_NULLPTR;
    }
    fprintf(stderr, "engine_create success!\n");

    fprintf(stderr, "%s\n%s\n", argv[1], argv[2]);

    ret_code = engine_config_init(engine->config, argv[1], argv[2]);
    if(ret_code)
    {
        fprintf(stderr, "engine_config_init error![ERR:%p]\n", ret_code);
        return KDK_NULLPTR;
    }
    fprintf(stderr, "engine_config_init success!\n");

    daemonize(DAEMON_NO_CLOSE_FILES | DAEMON_NO_REOPEN_STD_FDS);

    for(i = 0; i < 1; i++)
    {
        ret_code = engine_runtime_init(engine->runtime, engine->config, "SERV01");
        if(ret_code)
        {
            fprintf(stderr, "engine_config_init error!\n");
            continue;
        }

        while(1)
        {
            is_succ = 0;
            ret_code = flow_runtime_next(engine->runtime->flow_runtime, is_succ, KDK_NULL, node_id);
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

            fprintf(stderr, "node:%s\n", node_id);

            memset(&module, 0, sizeof(module_t));
            ret_code = module_coll_get(engine->config->module_coll, node_id, &module);
            if(ret_code == KDK_NULLPTR && ret_code != KDK_NOTFOUND)
            {
                fprintf(stderr, "module_get error!\n");
                break;
            }
            else if(ret_code == KDK_NOTFOUND)
            {
                fprintf(stderr, "no module!\n");
                engine_runtime_clear(engine->runtime);
                continue;
            }

            fprintf(stderr, "[ID:%s]\n", module.id);
            fprintf(stderr, "[PATH:%s]\n", module.path);
            fprintf(stderr, "[FILE:%s]\n", module.file_name);
            fprintf(stderr, "[FUNC:%s]\n", module.func_name);

            memset(path_file, 0, sizeof(path_file));
            sprintf(path_file, "%s/%s.so", module.path, module.file_name);

            memset(&dl_handle, 0, sizeof(kdk_dl_handle_t));
            ret_code = kdk_dl_handle_coll_get(engine->config->dl_handle_coll, module.func_name, &dl_handle);
            if(ret_code)
            {
                fprintf(stderr, "kdk_dl_handle_coll_get error!\n");
                break;
            }

            fprintf(stderr, "dl_handle:%s\n", dl_handle.func_name);
        }

        engine_runtime_clear(engine->runtime);

        usleep(100);
    }

    engine_destroy(engine);
    engine = KDK_NULL;

    return KDK_SUCCESS;
}
