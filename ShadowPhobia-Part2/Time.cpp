#include "Time.h"

// タイマーを開始する
void GameTimer::start(double seconds) {
	// 引数で指定された秒数でタイマーをセットし、即座に開始
	m_timer = Timer{ Duration{ seconds }, StartImmediately::Yes };
}

// 残り秒数を整数で返す
int32 GameTimer::getRemainingSeconds() const {
	// .s() を使うことで、自動的に端数を切り上げた残りの秒数（int32）が取得できます
	return m_timer.s();
}

// タイムアップ判定
bool GameTimer::isTimeUp() const {
	// 残り時間が 0 に達したかどうかを判定
	return m_timer.reachedZero();
}

// ポーズ処理
void GameTimer::pause() {
	m_timer.pause();
}

// 再開処理
void GameTimer::resume() {
	m_timer.resume();
}
