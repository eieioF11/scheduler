#include <stdio.h>
#include "scheduler.h"

// 疑似的なシステムタイマー（実際の組み込みではハードウェアタイマーの割り込み等でインクリメントされる）
static uint32_t System_Tick_Counter = 0;

// デバイス固有の時刻取得関数
uint32_t My_GetTick_ms(void) {
    return System_Tick_Counter;
}

// テスト用タスクA（高優先度・10ms周期）
void Task_A(void) {
    printf("[%4dms] Task A (High, 10ms) executed.\n", My_GetTick_ms());
}

// テスト用タスクB（低優先度・50ms周期）
void Task_B(void) {
    printf("[%4dms] Task B (Low,  50ms) executed.\n", My_GetTick_ms());
}

int main(void) {
    // 1. スケジューラの初期化（時刻取得関数を登録）
    scheduler_init(My_GetTick_ms);

    // 2. タスクの登録（関数、周期ms、優先度）
    scheduler_add_task(Task_A, 10, PRIORITY_HIGH);
    scheduler_add_task(Task_B, 50, PRIORITY_LOW);

    printf("Scheduler Start!\n");

    // 3. メインループ（シミュレーションのために100回回す）
    for (int i = 0; i < 200; i++) {
        // 擬似的に時間を進める（実際のマイコンでは自動で進みます）
        System_Tick_Counter++; 

        // スケジューラを毎回更新
        scheduler_update();
    }

    return 0;
}