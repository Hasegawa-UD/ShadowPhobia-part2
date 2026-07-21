#pragma once
#include <Siv3D.hpp>

// 隠し（透明）ブロック構造体
struct HiddenBlock
{
	RectF rect;
	bool isRevealed = false;
};

class GameStage
{
public:
	static constexpr double MapWidth = 3200.0;
	static constexpr double GroundBaseY = 540.0;

	static constexpr double GoalX = 3000.0;
	static constexpr double GoalPoleHeight = 300.0;

	// 落とし穴の範囲
	const Array<std::pair<double, double>> pits = {
		{ 1500.0, 1680.0 }
	};

	// 通常ブロック
	const Array<RectF> blocks = {
		RectF{ 500, 360, 120, 40 },
		RectF{ 770, 280, 120, 40 },
		RectF{ 1040, 200, 160, 40 },
		RectF{ 1280, 360, 150, 40 },
		RectF{ 1850, 320, 120, 40 },
		RectF{ 2100, 240, 120, 40 },
		RectF{ 2550, 400, 80, 140 },
	};

	// 隠しブロック
	Array<HiddenBlock> hiddenBlocks;

	//  コインの座標リスト
	Array<Vec2> coins;

	void reset()
	{
		hiddenBlocks.clear();
		coins.clear();

		// 隠しブロック
		hiddenBlocks.push_back(HiddenBlock{ RectF{ 1450, 360, 50, 40 }, false });

		//  コイン配置（好きな場所に置けるyo）
		coins << Vec2{ 520, 320 };
		coins << Vec2{ 780, 240 };
		coins << Vec2{ 1060, 160 };

		//  ブロックの上に自動でコインを置いてくれるyo
		for (const auto& block : blocks)
		{
			coins << Vec2{ block.x + block.w / 2, block.y - 40 };
		}
	}

	bool isOverPit(double posX) const
	{
		for (const auto& pit : pits)
		{
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
