#include "GameStage.h"

void GameStage::drawWorld() const
{
	// 背景描画（山・雲）
	for (int32 i = 0; i < 5; ++i)
	{
		const double baseX = i * 800;
		Triangle{ Vec2{ baseX + 200, GroundBaseY }, Vec2{ baseX + 350, GroundBaseY - 180 }, Vec2{ baseX + 500, GroundBaseY } }.draw(ColorF{ 0.2, 0.7, 0.3 });
		Circle{ baseX + 100, 150, 40 }.draw(Palette::White);
		Circle{ baseX + 130, 140, 50 }.draw(Palette::White);
		Circle{ baseX + 160, 150, 40 }.draw(Palette::White);
	}

	// 通常の障害物（ブロック）
	for (const auto& block : blocks)
	{
		block.draw(ColorF{ 0.8, 0.5, 0.2 }).drawFrame(2, Palette::Saddlebrown);
	}

	// 叩かれた隠しブロック（実体化したら描画）
	for (const auto& hBlock : hiddenBlocks)
	{
		if (hBlock.isRevealed)
		{
			// 出現したブロック（少し黄色っぽいハテナブロック風の色）
			hBlock.rect.draw(ColorF{ 0.9, 0.7, 0.2 }).drawFrame(2, Palette::Orange);
		}
	}

	// ゴールポール
	Line{ GoalX, GroundBaseY, GoalX, GroundBaseY - GoalPoleHeight }.draw(8, Palette::Lightgrey);
	Circle{ GoalX, GroundBaseY - GoalPoleHeight, 15 }.draw(Palette::Gold);
	Triangle{ Vec2{ GoalX, GroundBaseY - GoalPoleHeight + 10 }, Vec2{ GoalX + 60, GroundBaseY - GoalPoleHeight + 30 }, Vec2{ GoalX, GroundBaseY - GoalPoleHeight + 50 } }.draw(Palette::Red);

	// 地面の描画（落とし穴の部分を避けて分割描画）
	double currentX = 0.0;
	for (const auto& pit : pits)
	{
		if (pit.first > currentX)
		{
			const double width = pit.first - currentX;
			RectF{ currentX, GroundBaseY, width, 60.0 }.draw(ColorF{ 0.7, 0.4, 0.2 });
			Line{ currentX, GroundBaseY, currentX + width, GroundBaseY }.draw(4, Palette::White);
		}
		currentX = pit.second;
	}

	if (currentX < MapWidth)
	{
		const double width = MapWidth - currentX;
		RectF{ currentX, GroundBaseY, width, 60.0 }.draw(ColorF{ 0.7, 0.4, 0.2 });
		Line{ currentX, GroundBaseY, currentX + width, GroundBaseY }.draw(4, Palette::White);
	}
}
