#include <stdio.h>   
#include <stdlib.h>   
#include <string.h>   
#include <time.h>
#include <unistd.h>
#include <assert.h>   
#include <ctype.h>

#include "task.h"
#include "shell.h"

#undef TASK_DEBUG
#define TASK_DEBUG(args...) //printf(args)

#define CK_TIME 1   

#define LINUX_KERNEL_TASK_STAT	"%d (%[0-9A-Za-z: _,-]) %c %d %d %d %d %d %u %lu "\
								"%lu %lu %lu %lu %lu %ld %ld %ld %ld %d "\
								"%llu %lu %ld %lu %lu %lu %lu %lu %lu %lu"\
								"%lu %lu %lu %lu %lu %lu %d %lu %u %u "\
								"%llu %lu %lu"

typedef struct task_stat_info_s {
	int task_id;
	char tcomm[32];
	char state;
	int ppid;
	int pgid;
	int sid;
	int tty_nr;
	int tty_pgrp;
	unsigned long task_flags;
	unsigned long min_flt;
	//
	unsigned long cmin_flt;
	unsigned long maj_flt;
	unsigned long cmaj_flt;
	unsigned long utime;
	unsigned long stime;
	long cutime;
	long cstime;
	long priority;
	long nice;
	int num_threads;
	//
	unsigned long long start_time;
	unsigned long vsize;
	unsigned long mm_rss;
	unsigned long rsslim;
	unsigned long mm_start_code;
	unsigned long mm_end_code;
	unsigned long mm_start_stack;
	unsigned long esp;
	unsigned long eip;
	unsigned long pending_signal_sig;
	//
	unsigned long blocked_sig;
	unsigned long sigign_sig;
	unsigned long sigcatch_sig;
	unsigned long wchan;
	unsigned long unknow_1;
	unsigned long unknow_2;
	int exit_signal;
	unsigned int task_cpu;
	unsigned int rt_priority;
	unsigned int policy;
	//
	unsigned long long delayacct_blkio_ticks;
	unsigned long cgtime;
	unsigned long gtime;
}task_stat_info_t;

static int __task_stat_get(const char *str,task_stat_info_t *taskInfo)
{
	int ret;
	memset(taskInfo,0,sizeof(task_stat_info_t));
	assert(str && taskInfo);
	
	ret = sscanf(str,LINUX_KERNEL_TASK_STAT,
	&taskInfo->task_id,
	taskInfo->tcomm,
	&taskInfo->state,
	&taskInfo->ppid,
	&taskInfo->pgid,
	&taskInfo->sid,
	&taskInfo->tty_nr,
	&taskInfo->tty_pgrp,
	&taskInfo->task_flags,
	&taskInfo->min_flt,
	//
	&taskInfo->cmin_flt,
	&taskInfo->maj_flt,
	&taskInfo->cmaj_flt,
	&taskInfo->utime,
	&taskInfo->stime,
	&taskInfo->cutime,
	&taskInfo->cstime,
	&taskInfo->priority,
	&taskInfo->nice,
	&taskInfo->num_threads,
	//
	&taskInfo->start_time,
	&taskInfo->vsize,
	&taskInfo->mm_rss,
	&taskInfo->rsslim,
	&taskInfo->mm_start_code,
	&taskInfo->mm_end_code,
	&taskInfo->mm_start_stack,
	&taskInfo->esp,
	&taskInfo->eip,
	&taskInfo->pending_signal_sig,
	//
	&taskInfo->blocked_sig,
	&taskInfo->sigign_sig,
	&taskInfo->sigcatch_sig,
	&taskInfo->wchan,
	&taskInfo->unknow_1,
	&taskInfo->unknow_2,
	&taskInfo->exit_signal,
	&taskInfo->unknow_2, 	
	&taskInfo->task_cpu, 		
	&taskInfo->unknow_2, 
	//
	&taskInfo->unknow_2, 	
	&taskInfo->gtime,
	&taskInfo->cgtime);
	//printf("ret=%d\n",ret);
	assert(43 == ret);
	return 0;
}


void task_stat_parse(const char *str)
{
	int ret;
	task_stat_info_t taskInfo;
	assert(str);
	ret = __task_stat_get(str,&taskInfo);
	assert(0 == ret);
	printf("%-20s %-8d %-8c %-12d %-12lu %-12lu %-12lu %-12lu %-12lu %-8lu %-4u %-8u\n",
	taskInfo.tcomm,taskInfo.task_id,taskInfo.state,taskInfo.num_threads
	,taskInfo.utime,taskInfo.stime,taskInfo.cutime,taskInfo.cstime
	,taskInfo.priority,taskInfo.nice,taskInfo.task_cpu,taskInfo.mm_rss);
}

void task_show(const char *process_name)  
{
    char buf[1024*4] = {0,};
    char buf2[1024*4] = {0,};
    int pid = -1;
    //
	pid = task_id_get(process_name);
	TASK_DEBUG("pid: %d\n",pid);
	//
	while(1)
	{
		//
		system("clear");
		printf("%-20s %-8s %-8s %-12s %-12s %-12s %-12s %-12s %-12s %-8s %-4s %-4s\n",
		"task_name",
		"task_id",
		"state",
		"threads",
		"cpu_utime",
		"cpu_stime",
		"cpu_cutime",
		"cpu_cstime",
		"priority",
		"nice",
		"cpu",
		"mem"
		);
		printf("===================================================================="
		"===========================================================================\n");
		//
	    shell_cmd_parse(buf,sizeof(buf),"ls /proc/%d/task/ | xargs",pid);
		TASK_DEBUG("%s,%d: buf: %s\n",__func__,__LINE__,buf);

		char *sub = NULL;
		int i = 0;
		sub = strtok(buf," ");
		while(NULL != sub)
		{
			i = 0;
			while(0 != sub[i])
			{
				if(0 == isdigit(sub[i]))
				{
					sub[i] = 0;
					break;
				}
				i++;
			}
		    shell_cmd_parse(buf2,sizeof(buf2),"cat /proc/%d/task/%s/stat",pid,sub);
			TASK_DEBUG("%s,%d: buf2: %s\n",__func__,__LINE__,buf2);
			//
			task_stat_parse(buf2);
			//
			sub = strtok(NULL, " ");
		}	
		sleep(1);
	}
	
}

unsigned int task_id_get(const char *process_name)
{
    unsigned int pid = 0;
    char buf[1024*4] = {0,};
    assert(process_name);
    shell_cmd_parse(buf,sizeof(buf),"ps -ef | grep %s | grep -v grep | awk '{print $2}' ",process_name);
	sscanf(buf,"%d",&pid);
	return pid;
}


int task_info_new(const char *process_name,task_arr_t *taskArrOutput)
{
	assert(process_name);
	assert(taskArrOutput);
    int ppid = -1;
    char buf[1024*4] = {0,};
    unsigned long pids[1024*2] = {0,};
    unsigned long pid_num = 0;
    char *sub_str = NULL;
    int cnt = 0,i = 0;
    int ret = -1;
    task_info_t *pTaskInfo = NULL;
    task_stat_info_t taskStat;
    //pid
	ppid = task_id_get(process_name);
	TASK_DEBUG("ppid: %d\n",ppid);
	//
	bzero(buf,sizeof(buf));
	shell_cmd_parse(buf,sizeof(buf),"ls /proc/%d/task/ | xargs",ppid);
	TASK_DEBUG("%s,%d: buf: %s\n",__func__,__LINE__,buf);
	//分割字符串.
	cnt = 0;
	const char *delim = " ";
	sub_str = strtok(buf,delim);
	ret = sscanf(sub_str,"%lu",&pids[cnt++]);
	assert(1 == ret);
	while((sub_str = strtok(NULL, delim)))
    {
    	if(cnt >= sizeof(pids)/sizeof(pids[0]))
    	{
    		break;
    	}
		sscanf(sub_str,"%lu",&pids[cnt++]);
		assert(1 == ret);
    }
    pid_num = cnt;
    //
    pTaskInfo = (task_info_t *)malloc(sizeof(task_info_t)*pid_num);
	assert(pTaskInfo);
    //
    for(i = 0;i < pid_num;i++)
    {
		bzero(buf,sizeof(buf));
    	shell_cmd_parse(buf,sizeof(buf),"cat /proc/%d/task/%d/stat",ppid,pids[i]);
		bzero(&taskStat,sizeof(taskStat));
		ret = __task_stat_get(buf,&taskStat);
		assert(0 == ret);
		pTaskInfo[i].id = taskStat.task_id;
		strncpy(pTaskInfo[i].name,taskStat.tcomm,sizeof(pTaskInfo[i].name));
		pTaskInfo[i].state = taskStat.state;
		pTaskInfo[i].utime = taskStat.utime;
		pTaskInfo[i].stime = taskStat.stime;
		pTaskInfo[i].cutime = taskStat.cutime;
		pTaskInfo[i].cstime = taskStat.cstime;
		pTaskInfo[i].priority = taskStat.priority;
		pTaskInfo[i].nice = taskStat.nice;
		pTaskInfo[i].cpu = taskStat.task_cpu;
		pTaskInfo[i].num_threads = taskStat.num_threads;
    }	

    taskArrOutput->array = pTaskInfo;
    taskArrOutput->num = pid_num;
    
	return 0;
}


void task_info_free(task_arr_t *taskArr)
{
	if(taskArr) 
	{
		if(taskArr->array) free(taskArr->array);
	}
}

void task_info_show(task_arr_t *taskArr)
{
	int i;
	task_info_t *taskInfo;
	assert(taskArr);
	for(i = 0;i < taskArr->num;i++)
	{
		taskInfo = &taskArr->array[i];
		printf("%-20s %-8d %-8c %-12d %-12lu %-12lu %-12lu %-12lu %-12lu %-8lu %-4u\n",
		taskInfo->name,taskInfo->id,taskInfo->state,taskInfo->num_threads
		,taskInfo->utime,taskInfo->stime,taskInfo->cutime,taskInfo->cstime
		,taskInfo->priority,taskInfo->nice,taskInfo->cpu);
	}
}

