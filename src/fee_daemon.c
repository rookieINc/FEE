
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : daemon.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2016-01-18 18:16
 * Description : daemon.c
 * History     : 
 ***********************************************************/


#include "fee_daemon.h"

daemon_config_t *
daemon_config_create(kdk_mem_pool_t *mem_pool, kdk_uint32 mem_pool_size)
{
    kdk_uint32          mem_pool_type = OTHER_MEM_POOL;
    daemon_config_t    *daemon_config;

    if(mem_pool == KDK_NULL)
    {
        mem_pool = kdk_mem_pool_create(mem_pool_size, mem_pool_size);
        if(mem_pool == KDK_NULL)
            return KDK_NULL;
        mem_pool_type = OWN_MEM_POOL;
    }

    daemon_config = (daemon_config_t *)kdk_mem_pool_malloc(mem_pool, sizeof(daemon_config_t));
    if(daemon_config == KDK_NULL)
    {
        if(mem_pool_type == OWN_MEM_POOL)
        {
            kdk_mem_pool_destroy(mem_pool);
            mem_pool = KDK_NULL;
        }
        return KDK_NULL;
    }

    memset(daemon_config, 0, sizeof(daemon_config_t));
    daemon_config->mem_pool_type = mem_pool_type;
    daemon_config->mem_pool      = mem_pool;

    return daemon_config;
}

kdk_uint32  
daemon_config_init(daemon_config_t *daemon_config, kdk_char32 *daemon_config_file)
{
    kdk_uint32      ret_code, i;
    kdk_char32      value[PATH_LEN + 1];
    kdk_char32      key[PATH_LEN + 1];
    kdk_config_t   *config;

    if(daemon_config == KDK_NULL || daemon_config_file == KDK_NULL)
        return KDK_INARG;

    config = kdk_config_create(KDK_NULL, 1024, daemon_config_file);
    if(config == KDK_NULL)
        return KDK_NULLPTR;

    ret_code = kdk_config_init(config);
    if(ret_code)
    {
        kdk_config_destroy(config);
        config = KDK_NULL;
        return ret_code;
    }

    //memset(config, 0, sizeof(config_t));

    // PART [DAEMON]
    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(config, DAEMON, SLEEP_TIME, value);
    if(ret_code)
    {
        kdk_config_destroy(config);
        config = KDK_NULL;
        return ret_code;
    }
    daemon_config->sleep_time = atoi(value);

    // PART [LOG]
    memset(value, 0, sizeof(value));
    kdk_config_get_value(config, LOG, LOG_PATH, value);
    strncpy(daemon_config->log_path, value, PATH_LEN);

    memset(value, 0, sizeof(value));
    kdk_config_get_value(config, LOG, LOG_NAME, value);
    strncpy(daemon_config->log_name, value, NAME_LEN);

    memset(value, 0, sizeof(value));
    kdk_config_get_value(config, LOG, LOG_LEVEL, value);

    if(strcmp(value, "DEBUG") == 0)
        daemon_config->log_level = KDK_LOG_LEVEL_DEBUG;
    else if(strcmp(value, "INFO") == 0)
        daemon_config->log_level = KDK_LOG_LEVEL_INFO;
    else if(strcmp(value, "WARN") == 0)
        daemon_config->log_level = KDK_LOG_LEVEL_WARN;
    else if(strcmp(value, "ERROR") == 0)
        daemon_config->log_level = KDK_LOG_LEVEL_ERROR;
    else
        daemon_config->log_level = KDK_LOG_LEVEL_DEBUG;

    // PART [PROCESS]
    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(config, PROCESS, PROC_COUNT, value);
    if(ret_code)
    {
        kdk_config_destroy(config);
        config = KDK_NULL;
        return ret_code;
    }
    daemon_config->proc_count = atoi(value);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(config, PROCESS, PROC_PATH, value);
    if(ret_code)    
    {
        kdk_config_destroy(config);
        config = KDK_NULL;
        return ret_code;
    }
    strncpy(daemon_config->proc_path, value, PATH_LEN);

    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(config, PROCESS, PROC_NAME, value);
    if(ret_code)    
    {
        kdk_config_destroy(config);
        config = KDK_NULL;
        return ret_code;
    }
    strncpy(daemon_config->proc_name, value, NAME_LEN);

    // PART [ARGUMENT]
    memset(value, 0, sizeof(value));
    ret_code = kdk_config_get_value(config, ARGUMENT, ARG_COUNT, value);
    if(ret_code)    
    {
        kdk_config_destroy(config);
        config = KDK_NULL;
        return ret_code;
    }
    daemon_config->argc = atoi(value);

    if(daemon_config->argc < 0 || daemon_config->argc > ARG_LEN)
    {
        kdk_config_destroy(config);
        config = KDK_NULL;
        return KDK_OVERFLOW;
    }

    daemon_config->argv[0] = (kdk_char32 *)kdk_mem_pool_malloc(daemon_config->mem_pool, PATH_LEN + 1);
    if(daemon_config->argv[0] == KDK_NULL)
    {
        kdk_config_destroy(config);
        config = KDK_NULL;
        return KDK_NULLPTR;
    }
    strncpy(daemon_config->argv[0], daemon_config->proc_name, PATH_LEN);

    kdk_uint32 j;
    for(i = 1, j = 1; i <= daemon_config->argc; i++)
    {
        memset(key, 0, sizeof(key));
        snprintf(key, PATH_LEN + 1, "argument_%02d", i);

        daemon_config->argv[j] = (kdk_char32 *)kdk_mem_pool_malloc(daemon_config->mem_pool, PATH_LEN + 1);
        ret_code = kdk_config_get_value(config, ARGUMENT, key, daemon_config->argv[j]);        
        if(ret_code == KDK_NOTFOUND)
        {
            continue;
        }
        else if(ret_code)
        {
            kdk_config_destroy(config);
            config = KDK_NULL;
            return ret_code;
        }

        j++;
    }

    daemon_config->argv[j] = KDK_NULL;

    kdk_config_destroy(config);
    config = KDK_NULL;

    return KDK_SUCCESS;
}


kdk_void 
daemon_config_destroy(daemon_config_t *daemon_config)
{
    if(daemon_config == KDK_NULL)
        return ;
    
    if(daemon_config->mem_pool != KDK_NULL && daemon_config->mem_pool_type == OWN_MEM_POOL)
    {
        kdk_mem_pool_destroy(daemon_config->mem_pool);
        daemon_config->mem_pool = KDK_NULL;
    }

    return ;
}


kdk_void 
proc_start(daemon_config_t *daemon_config)
{
    switch(fork())
    {
        case KDK_SUCCESS:
            execvp(daemon_config->proc_name, daemon_config->argv);
            break;
        case KDK_LINUXERR:
            return ;
        default:
            break;
    }

    return ;
}

kdk_int32
main(kdk_int32 argc, kdk_char32 *argv[])
{
    kdk_pid             pid;
    kdk_uint32          proc_count, ret_code;
    daemon_config_t    *daemon_config;

    if(argc != 2)
    {
        fprintf(stderr, "USAGE:\n\tdaemon [CONFIG_FILE]\n");
        return KDK_INARG;
    }

    daemon_config = daemon_config_create(KDK_NULL, 1024);
    if(daemon_config == KDK_NULL)
    {
        fprintf(stderr, "守护进程配置[创建]失败...\n"); 
        return KDK_INARG;
    }
    fprintf(stderr, "守护进程配置[创建]成功...\n"); 

    ret_code = daemon_config_init(daemon_config, argv[1]);
    if(ret_code)
    {
        fprintf(stderr, "守护进程配置[初始化]失败...\n"); 
        return KDK_INARG;
    }
    fprintf(stderr, "守护进程配置[初始化]成功...\n"); 

    for(proc_count = 0; proc_count < daemon_config->proc_count; proc_count++)
    {
        fprintf(stderr, "进程[%s][%d]启动中...\n", daemon_config->proc_name, proc_count + 1);
        proc_start(daemon_config);
    }
    fprintf(stderr, "进程已全部启动...\n");

    fprintf(stderr, "...\n");
    fprintf(stderr, "日志配置\n");
    fprintf(stderr, "日志等级[LEVEL] : %d\n", daemon_config->log_level);
    fprintf(stderr, "日志路径[PATH]  : %s\n", daemon_config->log_path);
    fprintf(stderr, "日志前缀[NAME]  : %s\n", daemon_config->log_name);

    daemonize(DAEMON_NO_CHDIR);// | DAEMON_NO_CLOSE_FILES | DAEMON_NO_REOPEN_STD_FDS);

    if(strcmp(daemon_config->log_path, "") != 0 && strcmp(daemon_config->log_name, "") != 0)
    {
        ret_code = kdk_log_init(0, daemon_config->log_level, daemon_config->log_path, daemon_config->log_name);
        if(ret_code)
            return KDK_INARG;
        KLOG(INFO, "日志[初始化]成功...\n");
    }

    while(1)
    {
        pid = waitpid(-1, KDK_NULL, WNOHANG);    
        if(pid > 0)        
        {
            KLOG(WARN, "进程[%s]重启中...", daemon_config->proc_name);
            proc_start(daemon_config);
        }

        sleep(daemon_config->sleep_time);
    }

    kdk_log_destroy();

    daemon_config_destroy(daemon_config);
    daemon_config = KDK_NULL;

    return KDK_SUCCESS;
}
