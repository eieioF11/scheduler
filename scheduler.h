/**
 * @file scheduler.h
 * @author eieiof11
 * @brief タスクスケジュール管理ライブラリ
 * @version 2.1
 * @date 2026-05-19
 *
 * @copyright Copyright (c) 2026
 *
 */
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    PRIORITY_HIGH = 0,
    PRIORITY_MED,
    PRIORITY_LOW,
    NUM_PRIORITIES
} task_priority_t;

/**
 * @brief タスク関数の型定義
 */
typedef void (*task_function)(void *arg);

/**
 * @brief 時刻取得関数の型定義（例：1msごとにインクリメントされるカウンタを返す関数）
 */
typedef uint32_t (*get_tick_function)(void);

/**
 * @brief スケジューラの初期化関数
 * @param tick_func: 時刻取得関数を登録する
 */
void scheduler_init(get_tick_function tick_func);

/**
 * @brief タスクを登録する
 * @param name: タスクを一意に識別する名前
 * @param func: 実行する関数
 * @param arg: タスクが管理・使用するデータ領域へのポインタ（静的変数や構造体など）
 * @param period_ms: 周期（0 の場合は通知専用）
 * @param priority: 優先度
 */
bool scheduler_add_task(const char *name, task_function func, void *arg, uint32_t period_ms, task_priority_t priority);

/**
 * @brief タスク名から、そのタスクに紐づいているデータポインタ（arg）を取得する
 * @param name: タスク名
 * @return void*: 登録されているデータポインタ（見つからない場合は NULL）
 */
void* scheduler_get_task_arg(const char *name);

/**
 * @brief 指定したタスクに通知を送る
 * @param name: タスク名
 * 通知されたタスクは、次回のスケジューラ更新時に優先的に実行される
 */
void scheduler_notify_task(const char *name);

/**
 * @brief スケジューラの更新関数
 * これを定期的に呼び出すことで、タスクの実行が管理される
 */
void scheduler_update(void);

#endif // SCHEDULER_H