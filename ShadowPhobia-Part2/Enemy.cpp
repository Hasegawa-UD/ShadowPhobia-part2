#include "Enemy.h"

void EnemyManager::reset()
{
	const double enemyGroundY = 540.0 - (m_enemySize / 2.0);

	m_enemies.clear();

	// 1. 赤の敵（地上を左右移動）
	m_enemies.push_back(Enemy{ EnemyType::Walker, Vec2{ 600, enemyGroundY }, Vec2{ 600, enemyGroundY }, -100.0, 450.0, 750.0 });
	m_enemies.push_back(Enemy{ EnemyType::Walker, Vec2{ 1300, enemyGroundY }, Vec2{ 1300, enemyGroundY }, -120.0, 1150.0, 1450.0 });
	m_enemies.push_back(Enemy{ EnemyType::Walker, Vec2{ 2000, enemyGroundY }, Vec2{ 2000, enemyGroundY }, -150.0, 1850.0, 2200.0 });

	// 2. 緑の敵（空中を上下にゆらゆら浮遊・パタパタ風）
	m_enemies.push_back(Enemy{ EnemyType::Flyer, Vec2{ 950, 320 }, Vec2{ 950, 320 }, 0.0, 0.0, 0.0, 0.0 });
	m_enemies.push_back(Enemy{ EnemyType::Flyer, Vec2{ 1650, 280 }, Vec2{ 1650, 280 }, 0.0, 0.0, 0.0, 1.5 }); // 初期位相をずらす
	m_enemies.push_back(Enemy{ EnemyType::Flyer, Vec2{ 2400, 300 }, Vec2{ 2400, 300 }, 0.0, 0.0, 0.0, 3.0 });
}

void EnemyManager::update(double dt)
{
	for (auto& enemy : m_enemies)
	{
		if (!enemy.isAlive) continue;

		if (enemy.type == EnemyType::Walker)
		{
			// 地上歩行処理
			enemy.pos.x += enemy.velocityX * dt;

			if (enemy.pos.x <= enemy.minX)
			{
				enemy.pos.x = enemy.minX;
				enemy.velocityX = Abs(enemy.velocityX);
			}
			else if (enemy.pos.x >= enemy.maxX)
			{
				enemy.pos.x = enemy.maxX;
				enemy.velocityX = -Abs(enemy.velocityX);
			}
		}
		else if (enemy.type == EnemyType::Flyer)
		{
			// 空中浮遊処理（サイン波で上下運動）
			enemy.timer += dt * 3.0;
			enemy.pos.y = enemy.startPos.y + Math::Sin(enemy.timer) * 80.0;
		}
	}
}

void EnemyManager::draw(const Texture& texture) const
{
	for (const auto& enemy : m_enemies)
	{
		if (!enemy.isAlive) continue;

		// タイプによって色を変える（赤：地上敵、緑：飛行敵）
		const ColorF color = (enemy.type == EnemyType::Walker) ? ColorF{ 1.0, 0.3, 0.3 } : ColorF{ 0.3, 0.9, 0.3 };

		if (enemy.velocityX < 0)
		{
			texture.resized(m_enemySize).mirrored().drawAt(enemy.pos, color);
		}
		else
		{
			texture.resized(m_enemySize).drawAt(enemy.pos, color);
		}
	}
}
