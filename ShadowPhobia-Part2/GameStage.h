#pragma once
#include <Siv3D.hpp>

// 隠し（透明）ブロック構造体
struct HiddenBlock
{
	RectF rect;
	bool isRevealed = false; // たたかれて実体化したか
};

class GameStage
{
public:
	static constexpr double MapWidth = 3200.0;
	static constexpr double GroundBaseY = 540.0;

	static constexpr double GoalX = 3000.0;
	static constexpr double GoalPoleHeight = 300.0;

	// 地面の穴（落とし穴）の範囲 X座標: [始点, 終点]
	const Array<std::pair<double, double>> pits = {
		{ 1500.0, 1680.0 } // 意地悪トラップ配置エリアの落とし穴
	};

	// 通常の障害物
	const Array<RectF> blocks = {
		// --- エリア1: 登り階段 ＆ 高所足場 ---
		RectF{ 500, 360, 120, 40 },
		RectF{ 770, 280, 120, 40 },
		RectF{ 1040, 200, 160, 40 },

		// --- エリア2: 中空の島 ＆ トンネル構造 ---
		RectF{ 1280, 360, 150, 40 },

		// --- エリア3: 連なる空中アスレチック ---
		RectF{ 1850, 320, 120, 40 },
		RectF{ 2100, 240, 120, 40 },

		// --- エリア4: ゴール前の壁 ---
		RectF{ 2550, 400, 80, 140 },
	};

	// 隠し（透明）ブロック一覧
	Array<HiddenBlock> hiddenBlocks;

	void reset()
	{
		hiddenBlocks.clear();
		// 1500pxの落とし穴の少し手前(1450px)・頭上(Y=360)に透明ブロックを配置
		hiddenBlocks.push_back(HiddenBlock{ RectF{ 1450, 360, 50, 40 }, false });
	}

	// プレイヤーが穴の上にいるか判定（マージンを設けて判定をより確実に）
	bool isOverPit(double posX) const
	{
		for (const auto& pit : pits)
		{
			// 穴の左端から右端の範囲内か判定
			if (posX >= pit.first && posX <= pit.second)
			{
				return true;
			}
		}
		return false;
	}

	RectF getGoalRect() const
	{
		return RectF{ GoalX - 10.0, GroundBaseY - GoalPoleHeight, 20.0, GoalPoleHeight };
	}

	void drawWorld() const;
};
