#pragma once
#include <Siv3D.hpp>

class GameTimer {
private:
	Timer m_timer;

public:
	// 初期化（例：制限時間 300秒）
	void start(double seconds);

	// 残り時間を整数（秒）で取得する（マリオのUI用）
	int32 getRemainingSeconds() const;

	// タイムアップしたかどうか
	bool isTimeUp() const;

	// 一時停止 / 再開（ポーズ画面用）
	void pause();
	void resume();
};
