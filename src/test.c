#include "fee_engine.h"

int main(int argc, char *argv[])
{
    engine_t   *engine;
    kdk_char32  node_id[7] = {0};
    kdk_uint32  ret_code, i, is_succ;
    module_t    module;
    kdk_dl_handle_t dl_handle;   
    kdk_char32  path_file[124];

    if(argc != 4)
    {
        fprintf(stderr, "wrong!\n");
        return KDK_INARG;
    }

    engine = engine_create(KDK_NULL, 14096);
    if(engine == KDK_NULL)
    {
        fprintf(stderr, "引擎[创建]失败[%d]...\n", ret_code);
        return KDK_NULLPTR;
    }
    fprintf(stderr, "...\n");
    fprintf(stderr, "引擎[创建]成功...\n");

    fprintf(stderr, "配置文件[初始化]...\n");
    fprintf(stderr, "流程配置文件[FLOW]   : %s\n", argv[1]);
    fprintf(stderr, "模块配置文件[MODULE] : %s\n", argv[2]);
    fprintf(stderr, "日志配置文件[LOG]    : %s\n", argv[3]);

    ret_code = engine_config_init(engine->config, argv[1], argv[2], argv[3]);
    if(ret_code)
    {
        fprintf(stderr, "配置文件[初始化]失败[%d]...\n", ret_code);
        return KDK_NULLPTR;
    }
    fprintf(stderr, "配置文件[初始化]成功...\n");

    KLOG(INFO, "日志[初始化]成功...");
    ret_code = kdk_log_init(0, engine->config->log->level, engine->config->log->file_path, engine->config->log->file_name);
    if(ret_code)
    {
        fprintf(stderr, "日志[初始化]失败[%d]...", ret_code);
        return KDK_NULLPTR;
    }
    KLOG(INFO, "日志[初始化]成功...");

    //daemonize(DAEMON_NO_CHDIR | DAEMON_NO_CLOSE_FILES | DAEMON_NO_REOPEN_STD_FDS);

    while(1)
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
            //  fprintf(stderr, "flow_runtime_next finish!\n");
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

            KLOG(INFO, "node:%s", node_id);

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

            KLOG(INFO, "[ID:%s]", module.id);
            KLOG(INFO, "[PATH:%s]", module.path);
            KLOG(INFO, "[FILE:%s]", module.file_name);
            KLOG(INFO, "[FUNC:%s]", module.func_name);

            memset(path_file, 0, sizeof(path_file));
            sprintf(path_file, "%s/%s", module.path, module.file_name);

            memset(&dl_handle, 0, sizeof(kdk_dl_handle_t));
            ret_code = kdk_dl_handle_coll_get(engine->config->dl_handle_coll, module.func_name, &dl_handle);
            if(ret_code)
            {
                fprintf(stderr, "kdk_dl_handle_coll_get error!\n");
                break;
            }

            KLOG(INFO, "dl_handle:%s\n", dl_handle.func_name);
        }

        engine_runtime_clear(engine->runtime);

        usleep(100);
    }

    engine_destroy(engine);
    engine = KDK_NULL;

    return KDK_SUCCESS;
}
