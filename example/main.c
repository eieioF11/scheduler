#include <stdio.h>
#include "scheduler.h"

// ============================================================================
// 0. ハードウェア依存部分のシミュレーション
// ============================================================================
static uint32_t simulated_system_tick = 0;

// デバイス固有の時刻取得関数 (1msごとにカウントアップする変数を返す)
uint32_t Get_My_Device_Tick_ms(void) {
    return simulated_system_tick;
}

// ============================================================================
// 1. タスクが共有・管理するためのデータ構造
// ============================================================================
typedef struct {
    float temperature;    // センサーから読み取った温度
    uint32_t scan_count;  // スキャンした回数
} SensorContext_t;

// ============================================================================
// 2. タスク関数の定義
// ============================================================================

/**
 * @brief センサー値を定期的に読み取るタスク (周期: 100ms)
 * @param arg 登録時に渡された SensorContext_t へのポインタ
 */
void Task_Sensor_Scan(void *arg) {
    SensorContext_t *context = (SensorContext_t *)arg;
    if (context == NULL) return;
    printf("[%4dms] [Sensor] センサー値をスキャン中... (前回: %.1f℃)\n", Get_My_Device_Tick_ms(), context->temperature);
    // 擬似的にセンサーの値を更新
    context->temperature += 0.1f; // 時間とともに少しずつ温度が上がるシミュレーション
    context->scan_count++;
}

/**
 * @brief データを画面に表示するタスク (周期: 500ms または 通知時に実行)
 * @param arg 今回は使用しないため NULL が渡される
 */
void Task_Display_Output(void *arg) {
    (void)arg; // 未使用コンパイル警告よけ

    SensorContext_t *sensor_data = (SensorContext_t *)scheduler_get_task_arg("Sensor");

    if (sensor_data != NULL) {
        printf("[%4dms] [Display] 最新データ -> 温度: %.1f℃ (スキャン回数: %d)\n",
               Get_My_Device_Tick_ms(), 
               sensor_data->temperature, 
               sensor_data->scan_count);
    }
}

// ============================================================================
// 3. メイン関数
// ============================================================================
int main(void) {
    // 1. スケジューラの初期化（マイコン固有の時刻取得関数を登録）
    scheduler_init(Get_My_Device_Tick_ms);

    // 2. センサータスクが使う「データの記憶部屋（実体）」を用意
    static SensorContext_t sensor_storage = { .temperature = 20.0f, .scan_count = 0 };

    // 3. タスクの登録 (タスク名, 関数ポインタ, データポインタ, 周期ms, 優先度)
    scheduler_add_task("Sensor",  Task_Sensor_Scan,    &sensor_storage, 100, PRIORITY_MED);
    scheduler_add_task("Display", Task_Display_Output, NULL,            500, PRIORITY_LOW);

    printf("--- スケジューラ動作開始 ---\n");

    // 4. メインループのシミュレーション (1秒間 = 1000ms 分まわす)
    for (int i = 0; i < 1000; i++) {
        // 擬似的に時間を 1ms 進める（実際の組み込みではタイマー割り込み等で自動で進みます）
        simulated_system_tick++;

        // 周期（500ms）を無視して、今すぐ "Display" タスクを動かすよう通知を投げる！
        if (simulated_system_tick == 250) {
            printf("[%4dms] [外部イベント発生] Displayタスクへ即時実行通知を送信！\n", simulated_system_tick);
            scheduler_notify_by_name("Display");
        }

        // スケジューラの更新関数を毎回呼び出す
        scheduler_update();
    }

    printf("--- スケジューラ動作終了 ---\n");
    return 0;
}