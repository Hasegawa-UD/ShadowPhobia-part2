#include "GameTimer.h" // ダブルクォーテーションにする

// タイマーを開始する
void GameTimer::start(double seconds) {
	m_timer = Timer{ Duration{ seconds }, StartImmediately::Yes };
}

// 残り秒数を整数で返す
int32 GameTimer::getRemainingSeconds() const {
	return m_timer.s();
}

// タイムアップ判定
bool GameTimer::isTimeUp() const {
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
