/*
 * Copyright (C) 2025 pdnguyen of HCMC University of Technology VNU-HCM
 */

/* Sierra release
 * Source Code License Grant: The authors hereby grant to Licensee
 * personal permission to use and modify the Licensed Source Code
 * for the sole purpose of studying while attending the course CO2018.
 */

 /*
    Modified by Nguyen Thien Tri:
    implement killall syscall
    queue.h and string.h have been added
 */

#include "../include/common.h"
#include "../include/syscall.h"
#include "stdio.h"
#include "../include/libmem.h"
#include <pthread.h>

//for traversing the proclist
#include "../include/queue.h" 
//for checking if the process matches proc_name
#include <string.h>

pthread_mutex_t sys_kill_lock;


int __sys_killall(struct pcb_t *caller, struct sc_regs* regs)
{

    pthread_mutex_init(&sys_kill_lock, NULL);

    char proc_name[100];
    uint32_t data;

    //hardcode for demo only
    uint32_t memrg = regs->a1;
    
    /* TODO: Get name of the target proc */
    //proc_name = libread..
    int i = 0;
    data = 0;
    while(data != -1){
        libread(caller, memrg, i, &data);
        proc_name[i]= data;
        if(data == -1) proc_name[i]='\0';
        i++;
    }
    printf("The procname retrieved from memregionid %d is \"%s\"\n", memrg, proc_name);

    /* TODO: Traverse proclist to terminate the proc
     *       stcmp to check the process match proc_name
     */
    //caller->running_list
    //caller->mlq_ready_queu

    /* TODO Maching and terminating 
     *       all processes with given
     *        name in var proc_name
     */

    //define a macro to traverse the proclist and terminate if match
    #define TRAVERSE_AND_KILL(q)\
    { \
        pthread_mutex_lock(&sys_kill_lock);\
        for (int i = 0; i < (q)->size; ++i) { \
            struct pcb_t *proc = (q)->proc[i]; \
            if (proc != NULL && strcmp(proc->path, proc_name) == 0) { \
                /*terminate*/\
                free_pcb_memph(proc); \
                /*shift the remanining ones to the left*/ \
                for (int j = i; j < (q)->size - 1; ++j) { \
                    (q)->proc[j] = (q)->proc[j+1]; \
                } \
                (q)->proc[(q)->size - 1] = NULL; \
                --(q)->size; \
                --i;  \
            } \
        } \
        pthread_mutex_unlock(&sys_kill_lock);\
    }

    
    if (caller->ready_queue) TRAVERSE_AND_KILL(caller->ready_queue);
    if (caller->running_list) TRAVERSE_AND_KILL(caller->running_list);
    if (caller->mlq_ready_queue) TRAVERSE_AND_KILL(caller->mlq_ready_queue);

    return 0; 
}
