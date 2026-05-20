#include <stdio.h>
#include <unistd.h> // usleep用（シミュレーション用）
#include "scheduler.h"

// 擬似的なシステムティックカウンタ（ミリ秒単位）
static uint32_t simulated_ticks = 0;

// 時刻取得関数の実装
uint32_t get_system_tick(void) {
    return simulated_ticks;
}

// サンプルタスクA：センサー値を読み取る（1000ms周期想定）
void task_counter_read(void *arg) {
    int *call_count = (int *)arg;
    (*call_count)++;
}

void task_print_task(void *arg) {
    int *call_count = (int *)arg;
    printf("[%4dms] [PrintTask] CounterTaskの実行回数: %d\n", simulated_ticks, *call_count);
}


// サンプルタスクB：通知でのみ動くタスク（周期は0）
void task_alert(void *arg) {
    printf("[%4dms] [AlertTask] 手動通知されました！\n", simulated_ticks);
}

int main(void) {
    int task_a_counter = 0;

    // 1. スケジューラの初期化
    scheduler_init(get_system_tick);

    // 2. タスクの登録
    // タスクAは1000ms周期、タスクBは周期0（通知専用）
    scheduler_add_task("CounterTask", task_counter_read, &task_a_counter, 10, PRIORITY_MED);
    scheduler_add_task("PrintTask", task_print_task, &task_a_counter, 100, PRIORITY_LOW);
    scheduler_add_task("AlertTask", task_alert, NULL, 0, PRIORITY_HIGH);

    printf("--- スケジューラ デモ開始 (初期状態：周期実行ON) ---\n");

    // シミュレーションループ（1msごとに進むイメージ）
    for (int i = 0; i <= 6000; i++) {
        simulated_ticks = i;

        // 定期的にスケジューラを更新
        scheduler_update();

        // 【デモのアクション1】3000ms時点で、タスクAの周期実行を「無効」にする
        if (simulated_ticks == 3000) {
            printf("\n--- [System] 3000ms到達: CounterTask の周期実行を【無効】にします ---\n\n");
            scheduler_disable_period("CounterTask");
        }

        // 【デモのアクション2】4500ms時点で、タスクB（通知専用）に手動通知を送る
        // （周期実行が無効化されていても、notify機能は生きていることの確認）
        if (simulated_ticks == 4500) {
            printf("\n--- [System] 4500ms到達: AlertTask に通知を送ります ---\n");
            scheduler_notify_task("AlertTask");
        }

        // 【デモのアクション3】5000ms時点で、タスクAの周期実行を「有効」に戻す
        if (simulated_ticks == 5000) {
            printf("\n--- [System] 5000ms到達: CounterTask の周期実行を【有効】に戻します ---\n\n");
            scheduler_enable_period("CounterTask");
        }

        // 実際のミリ秒を少し待つ（シミュレーションをゆっくり見せるため）
        usleep(1000);
    }

    printf("--- デモ終了 ---\n");
    return 0;
}