//
// Created by baijiajun on 25-11-14.
//
#include "kernel/types.h"
#include "user/user.h"
#include "kernel/riscv.h"
#include "kernel/sysinfo.h"

#include <stdio.h>
#include <stdlib.h>

#include "../kernel/riscv.h"

void sinfo(struct sysinfo *info)
{
    if (sysinfo(info) < 0)
    {
        printf("FATAL:sysinfo failed \n");
        exit(1);
    }
}

void testcall()
{
    struct sysinfo info;
    if (sysinfo(&info) <0)
    {
        printf("FATAL:sysinfo failed \n");
        exit(1);
    }
   if (sysinfo((struct sysinfo*)0xeaeb0b0102030406) != -1)
   {
       printf("FATAL : sysinfo succeeded with bad argument \n");
       exit(1);
   }
}

uint64 freecount()
{
    struct sysinfo info;
    uint64 sz0 = (uint64)sbrk(0);
    uint64 n =0;
    while (1)
    {
        if ((uint64)sbrk(PGSIZE) == 0xffffffffffffffff)
        {
            break;
        }
        n += PGSIZE;
    }
    sinfo(&info);
    if (info.freemem != 0)    //判断是否为内存全被占用
    {
        printf("FATAL: there is no free mem ,but info.freemem %d \n" , info.freemem);
        exit(1);
    }
    sbrk(-((uint64)sbrk(0) - sz0));
    return n;
}

void testmem()
{
    struct sysinfo info;
    uint64 n = freecount();

    sinfo(&info);
    if (info.freemem != n)
    {
        printf("FATAL: there is no free mem ,but info.freemem %d \n" , info.freemem);
        exit(1);
    }

    if ((uint64)sbrk(PGSIZE) == 0xffffffffffffffff)
    {
        printf("brsk failed \n");
        exit(1);
    }

    sinfo(&info);
    if (info.freemem != n - PGSIZE)
    {
        printf("FATAL: free men %d (bytes) instead of %d" , n - PGSIZE , info.freemem);
        exit(1);
    }

    if ((uint64)sbrk(-PGSIZE) == 0xffffffffffffffff)
    {
        printf("brsk failed \n");
        exit(1);
    }

    sinfo(&info);
    if (info.freemem != n)
    {
        printf("FATAL: free mem %d (bytes) instead od %d" , n , info.freemem);
        exit(1);
    }
}

void testproc()
{
    struct sysinfo info;
    uint64 nproc;
    int status;
    int pid;
    sinfo(&info);
    nproc = info.nproc;

    pid = fork();
    if (pid < 0)
    {
        printf("FATAL:fork failed \n");
        exit(1);
    }
    if (pid == 0)
    {
        sinfo(&info);
        if (info.nproc != nproc +1 )
        {
            printf("sysinfotest : FATAL nproc id %d instead of %d" ,info.nproc ,nproc);
            exit(1);
        }
        exit(0);
    }
    sinfo(&info);
    if (info.nproc != nproc)
    {
        printf("sysinfotest : FATAL nproc id %d instead of %d" ,info.nproc ,nproc);
        exit(1);
    }
}

int main(int argc , char **argv)
{
    printf("sysinfo test start \n");
    testcall();
    testmem();
    testproc();
    printf("sysinfo test ok \n");
    exit(0);
}
