#include "scheduler.h"
#include <string.h>

#define MAX_TASKS 10
#define MAX_NAME_LEN 16

typedef struct {
    char name[MAX_NAME_LEN];
    task_function func;
    void *arg;               // タスク専用のデータポインタ
    uint32_t period_ms;
    uint32_t last_run_tick;
    task_priority_t priority;
    bool is_used;
    volatile bool notified;
} task_t;

static task_t task_list[MAX_TASKS];
static get_tick_function get_current_tick = NULL;

void scheduler_init(get_tick_function tick_func) {
    get_current_tick = tick_func;
    for (int i = 0; i < MAX_TASKS; i++) {
        task_list[i].is_used = false;
        task_list[i].notified = false;
        task_list[i].arg = NULL;
    }
}

bool scheduler_add_task(const char *name, task_function func, void *arg, uint32_t period_ms, task_priority_t priority) {
    if (func == NULL || name == NULL) return false;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (!task_list[i].is_used) {
            strncpy(task_list[i].name, name, MAX_NAME_LEN - 1);
            task_list[i].name[MAX_NAME_LEN - 1] = '\0';
            
            task_list[i].func = func;
            task_list[i].arg = arg; // 共有データ領域のポインタを保持
            task_list[i].period_ms = period_ms;
            task_list[i].priority = priority;
            task_list[i].last_run_tick = (get_current_tick != NULL) ? get_current_tick() : 0;
            task_list[i].notified = false;
            task_list[i].is_used = true;
            return true;
        }
    }
    return false;
}

// 【新機能】名前からデータ領域のポインタを引っ張ってくる関数
void* scheduler_get_task_arg(const char *name) {
    if (name == NULL) return NULL;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].is_used && strcmp(task_list[i].name, name) == 0) {
            return task_list[i].arg; // 登録されているポインタを返す
        }
    }
    return NULL;
}

void scheduler_notify_task(const char *name) {
    if (name == NULL) return;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_list[i].is_used && strcmp(task_list[i].name, name) == 0) {
            task_list[i].notified = true;
            break;
        }
    }
}

void scheduler_update(void) {
    if (get_current_tick == NULL) return;

    uint32_t current_tick = get_current_tick();

    for (int p = 0; p < NUM_PRIORITIES; p++) {
        for (int i = 0; i < MAX_TASKS; i++) {
            if (!task_list[i].is_used || task_list[i].priority != (task_priority_t)p) {
                continue;
            }

            bool should_run = false;

            if (task_list[i].notified) {
                task_list[i].notified = false;
                should_run = true;
                task_list[i].last_run_tick = current_tick;
            }
            else if (task_list[i].period_ms > 0) {
                uint32_t elapsed = current_tick - task_list[i].last_run_tick;
                if (elapsed >= task_list[i].period_ms) {
                    should_run = true;
                    task_list[i].last_run_tick += task_list[i].period_ms;
                }
            }

            if (should_run) {
                // 実行時に毎回、登録されているデータポインタを渡す
                task_list[i].func(task_list[i].arg);
            }
        }
    }
}