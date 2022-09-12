#ifndef __TASK_H
#define __TASK_H


typedef struct task_info_s {
	int id;
	char name[32];
	char state;
	unsigned long utime;
	unsigned long stime;
	long cutime;
	long cstime;
	long priority;
	long nice;
	unsigned int cpu;
	int num_threads;
}task_info_t;


typedef struct task_arr_s {
	task_info_t *array;
	unsigned int num;
}task_arr_t;


//
void task_show(const char *process_name);
unsigned int task_id_get(const char *process_name);
int task_info_new(const char *process_name,task_arr_t *taskArrOutput);
void task_info_free(task_arr_t *taskArr);
void task_info_show(task_arr_t *taskArr);

#endif  //__TASK_H


