#pragma once
#include <Siv3D.hpp> // これが必要！

class GameTimer {
private:
	Timer m_timer;

public:
	// 初期化（制限時間設定してスタート）
	void start(double seconds);

	// 残り時間を整数（秒）で取得
	int32 getRemainingSeconds() const;

	// タイムアップしたかどうか
	bool isTimeUp() const;

	// 一時停止 / 再開
	void pause();
	void resume();
};
