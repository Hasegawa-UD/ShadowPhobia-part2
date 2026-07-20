#pragma once
#include <Siv3D.hpp>

// 敵の種類
enum class EnemyType
{
	Walker, // 赤：地上を左右移動（クリボー風）
	Flyer   // 緑：空中を上下浮遊（パタパタ風）
};

struct Enemy
{
	EnemyType type = EnemyType::Walker;
	Vec2 pos;
	Vec2 startPos;      // 初期位置（浮遊計算用）
	double velocityX = 0.0;
	double minX = 0.0;  // 左右移動範囲
	double maxX = 0.0;
	double timer = 0.0; // 浮遊用タイマー
	bool isAlive = true;

	Circle getCircle() const
	{
		return Circle{ pos, 30.0 };
	}
};

class EnemyManager
{
private:
	Array<Enemy> m_enemies;
	double m_enemySize = 80.0;

public:
	void reset();
	void update(double dt);
	void draw(const Texture& texture) const;

	Array<Enemy>& getEnemies() { return m_enemies; }
	const Array<Enemy>& getEnemies() const { return m_enemies; }
};
