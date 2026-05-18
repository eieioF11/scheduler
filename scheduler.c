/**
 * @file scheduler.c
 * @author eieiof11
 * @brief タスクスケジュール管理ライブラリ
 * @version 0.1
 * @date 2026-05-19
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "scheduler.h"

#define MAX_TASKS 10

// タスクの情報を管理する構造体
typedef struct {
    task_function func;      // 実行する関数
    uint32_t period_ms;     // 周期 (ms)
    uint32_t last_run_tick; // 前回実行した時刻
    TaskPriority priority;  // 優先度
    bool is_used;          // このスロットが使用中か
} task_t;

// スケジューラの内部状態
static task_t task_list[MAX_TASKS];
static get_tick_function get_current_tick = NULL;

void scheduler_init(get_tick_function tick_func) {
    get_current_tick = tick_func;
    for (int i = 0; i < MAX_TASKS; i++) {
        task_list[i].is_used = false;
    }
}

bool scheduler_add_task(task_function func, uint32_t period_ms, TaskPriority priority) {
    if (func == NULL) return false;

    for (int i = 0; i < MAX_TASKS; i++) {
        if (!task_list[i].is_used) {
            task_list[i].func = func;
            task_list[i].period_ms = period_ms;
            task_list[i].priority = priority;
            // 初期状態では、登録時の時刻をセット（または0）
            task_list[i].last_run_tick = (get_current_tick != NULL) ? get_current_tick() : 0;
            task_list[i].is_used = true;
            return true;
        }
    }
    return false; // 空きスロットなし
}

void scheduler_update(void) {
    if (get_current_tick == NULL) return;

    uint32_t current_tick = get_current_tick();

    // 優先度（PRIORITY_HIGH = 0）の高い順にループを回す
    for (int p = 0; p < NUM_PRIORITIES; p++) {
        for (int i = 0; i < MAX_TASKS; i++) {
            if (!task_list[i].is_used || task_list[i].priority != (TaskPriority)p) {
                continue;
            }

            // 時間が経過したかどうかの判定（オーバーフロー対策を含む）
            uint32_t elapsed = current_tick - task_list[i].last_run_tick;
            if (elapsed >= task_list[i].period_ms) {
                // タスク実行
                task_list[i].func();
                
                // 次回計算用に前回の実行時刻を更新
                // (+= period_ms にすることで、処理遅れによる周期のズレを蓄積させない)
                task_list[i].last_run_tick += task_list[i].period_ms;
            }
        }
    }
}