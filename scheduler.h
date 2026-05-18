/**
 * @file scheduler.h
 * @author eieiof11
 * @brief タスクスケジュール管理ライブラリ
 * @version 0.1
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

/**
 * @brief タスクの優先度
 * 低い数値ほど高優先度（例：0が最も高い）
 */
typedef enum {
    PRIORITY_HIGH = 0,
    PRIORITY_MED,
    PRIORITY_LOW,
    NUM_PRIORITIES
} TaskPriority;

/**
 * @brief タスク関数の型定義
 */
typedef void (*task_function)(void);

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
 * @brief タスクをスケジューラに追加する
 * @param func: 実行するタスク関数
 * @param period_ms: タスクの実行周期（ms）
 * @param priority: タスクの優先度
 * @return true: 成功, false: 失敗
 */
bool scheduler_add_task(task_function func, uint32_t period_ms, TaskPriority priority);

/**
 * @brief スケジューラの更新関数
 * これを定期的に呼び出すことで、タスクの実行が管理される
 */
void scheduler_update(void);

#endif // SCHEDULER_H