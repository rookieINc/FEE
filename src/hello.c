
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

int hello(void)
{
    fprintf(stderr, "Hello, world![%d]\n", getpid());
    return 0;
}

int main(int argc, char *argv[])
{
    int  i;

    for(i = 0; i < argc; i++)
        fprintf(stderr, "[ARG:%s]\n", argv[i]);

    while(1)
    {
        hello();
        sleep(2);
    }

    return 0;
}
