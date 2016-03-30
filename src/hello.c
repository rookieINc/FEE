
/************************************************************
 * Copyright(C), 2015-2016, QTPAY
 * 
 * FileName    : hello.c
 * Author      : rookieINc
 * Version     : v0.1.0
 * Date        : 2016-01-14 03:34
 * Description : hello.c
 * History     : For test
 ***********************************************************/


#include <stdio.h>
#include "kdk.h"

int hello(void)
{
    fprintf(stderr, "Hello, world![%d]\n", getpid());
    return 0;
}

int main(int argc, char *argv[])
{
    kdk_uint32  ret_code;
    int  i;

    for(i = 0; i < argc; i++)
        fprintf(stderr, "[ARG:%s]\n", argv[i]);

    ret_code = kdk_log_init(0,0,".","test");
    if(ret_code)
    {
        fprintf(stderr, "kdk_log_init error![%d]\n", ret_code);
        return -1;
    }

    KLOG(ERROR, "%s", "hello");

    kdk_log_destroy();

    fprintf(stderr, "over!\n");

    ret_code = kdk_log_init(0,0,".","test");

    KLOG(ERROR, "%s", "test");

    kdk_log_destroy();

/*
    while(1)
    {
        hello();
        sleep(2);
    }
*/

    return 0;
}
