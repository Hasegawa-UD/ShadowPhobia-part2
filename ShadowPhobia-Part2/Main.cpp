#include <Siv3D.hpp>
#include "GameTimer.h"
#include "Enemy.h"
#include "GameStage.h"

void Main()
{
	Window::Resize(800, 600);
	Scene::SetBackground(ColorF{ 0.4, 0.7, 1.0 });

	const Font font{ 30, Typeface::Bold };
	const Font largeFont{ 60, Typeface::Bold };
	const Texture playerTexture{ U"player.png" };

	if (not playerTexture)
	{
		throw Error{ U"画像ファイルの読み込みに失敗しました。" };
	}

	GameStage stage;
	EnemyManager enemyManager;
	GameTimer gameTimer;
	Camera2D camera{ Vec2{ 400, 300 }, 1.0 };

	const double playerSize = 120.0;
	const double groundY = GameStage::GroundBaseY - (playerSize / 2.0);

	const double moveAccel = 1800.0;
	const double airAccel = 1200.0;
	const double maxSpeed = 320.0;
	const double friction = 9.0;

	const double gravity = 1100.0;
	const double jumpForce = -620.0;
	const double stompJumpForce = -400.0;
	const double gameTimeLimit = 300.0;

	Vec2 playerPos;
	double velocityX = 0.0;
	double velocityY = 0.0;
	bool isGrounded = true;
	bool isFacingLeft = false;
	bool isGameOver = false;
	bool isGameClear = false;

	double coyoteTimer = 0.0;
	double jumpBufferTimer = 0.0;

	auto getPlayerRect = [&](const Vec2& pos) {
		return RectF{ Arg::center = pos, playerSize * 0.6, playerSize * 0.8 };
		};

	auto resetGame = [&]()
		{
			playerPos = Vec2{ 200, groundY };
			velocityX = 0.0;
			velocityY = 0.0;
			isGrounded = true;
			isFacingLeft = false;
			isGameOver = false;
			isGameClear = false;

			coyoteTimer = 0.0;
			jumpBufferTimer = 0.0;

			stage.reset();
			enemyManager.reset();

			const double cameraX = Math::Clamp(playerPos.x, 400.0, GameStage::MapWidth - 400.0);
			camera.setTargetCenter(Vec2{ cameraX, 300.0 });
			camera.update();

			gameTimer.start(gameTimeLimit);
		};

	resetGame();

	while (System::Update())
	{
		const double dt = Scene::DeltaTime();

		// 1. カメラ更新
		const double cameraX = Math::Clamp(playerPos.x, 400.0, GameStage::MapWidth - 400.0);
		camera.setTargetCenter(Vec2{ cameraX, 300.0 });
		camera.update();

		// 2. リトライ/リザルト画面
		if (isGameOver || isGameClear)
		{
			{
				const auto transformer = camera.createTransformer();
				stage.drawWorld();
				enemyManager.draw(playerTexture);

				if (isFacingLeft) playerTexture.resized(playerSize).mirrored().drawAt(playerPos);
				else playerTexture.resized(playerSize).drawAt(playerPos);
			}

			Scene::Rect().draw(ColorF{ 0.0, 0.0, 0.0, 0.5 });

			if (isGameClear)
			{
				largeFont(U"STAGE CLEAR!").drawAt(Vec2{ 400, 230 }, Palette::Yellow);
			}
			else
			{
				largeFont(U"GameOver!").drawAt(Vec2{ 400, 230 }, Palette::Red);
			}

			font(U"Rボタンでリトライ").drawAt(Vec2{ 400, 330 }, Palette::White);

			const int32 remainingTime = gameTimer.getRemainingSeconds();
			font(U"TIME: {:03d}"_fmt(remainingTime)).draw(Arg::topRight = Vec2{ 770, 20 }, Palette::Yellow);

			if (KeyR.down()) resetGame();
			continue;
		}

		// 3. タイムアップ ＆ 画面外落下（落とし穴）判定
		if (gameTimer.isTimeUp() || playerPos.y > 750.0)
		{
			isGameOver = true;
			gameTimer.pause();
		}

		// 4. 敵の更新
		enemyManager.update(dt);

		// 5. タイマーの更新
		if (isGrounded) coyoteTimer = 0.12;
		else coyoteTimer -= dt;

		if (KeySpace.down()) jumpBufferTimer = 0.15;
		else jumpBufferTimer -= dt;

		// 6. 移動 ＆ 物理演算
		double inputX = 0.0;
		if (KeyLeft.pressed() || KeyA.pressed())  inputX -= 1.0;
		if (KeyRight.pressed() || KeyD.pressed()) inputX += 1.0;

		if (inputX != 0.0)
		{
			const double accel = (coyoteTimer > 0) ? moveAccel : airAccel;
			velocityX += inputX * accel * dt;
			velocityX = Math::Clamp(velocityX, -maxSpeed, maxSpeed);
		}
		else
		{
			velocityX -= velocityX * friction * dt;
			if (Abs(velocityX) < 1.0) velocityX = 0.0;
		}

		// --- X軸移動 ---
		playerPos.x += velocityX * dt;

		// ブロック横押し出し（通常の空中・地面ブロックのみ）
		auto checkHorizontalBlock = [&](const RectF& block) {
			const RectF pRect = getPlayerRect(playerPos);
			if (pRect.intersects(block))
			{
				if (pRect.y + pRect.h > block.y + 10.0 && pRect.y < block.y + block.h - 10.0)
				{
					if (velocityX > 0)
					{
						playerPos.x = block.x - pRect.w / 2.0;
						velocityX = 0.0;
					}
					else if (velocityX < 0)
					{
						playerPos.x = block.x + block.w + pRect.w / 2.0;
						velocityX = 0.0;
					}
				}
			}
			};

		for (const auto& block : stage.blocks) checkHorizontalBlock(block);
		for (const auto& hBlock : stage.hiddenBlocks)
		{
			if (hBlock.isRevealed) checkHorizontalBlock(hBlock.rect);
		}

		// --- ジャンプ判定 ---
		if (jumpBufferTimer > 0.0 && coyoteTimer > 0.0)
		{
			velocityY = jumpForce;
			coyoteTimer = 0.0;
			jumpBufferTimer = 0.0;
			isGrounded = false;
		}

		if (!isGrounded)
		{
			velocityY += gravity * dt;
		}
		playerPos.y += velocityY * dt;

		bool landedThisFrame = false;

		// ★ 落とし穴の範囲判定
		// プレイヤーの判定サイズ(playerSize * 0.6)の半分を考慮して、体の一部が穴にかかっているか確認
		
		const bool isInPit = stage.isOverPit(playerPos.x);
		const bool isNearPitEdge = stage.isOverPit(playerPos.x) || stage.isOverPit(playerPos.x);

		// 穴の上、または穴のフチに引っかかっている間は「地面への着地」を完全に禁止する
		if (!isInPit && !isNearPitEdge)
		{
			if (playerPos.y >= groundY)
			{
				playerPos.y = groundY;
				velocityY = 0.0;
				landedThisFrame = true;
			}
		}

		// ブロック縦判定処理（通常ブロック ＆ 実体化した透明ブロック）
		auto checkVerticalBlock = [&](const RectF& block) {
			const RectF pRect = getPlayerRect(playerPos);
			if (pRect.intersects(block))
			{
				if (velocityY >= 0.0 && (pRect.y + pRect.h - velocityY * dt) <= block.y + 18.0)
				{
					playerPos.y = block.y - pRect.h / 2.0;
					velocityY = 0.0;
					landedThisFrame = true;
				}
				else if (velocityY < 0.0 && (pRect.y - velocityY * dt) >= block.y + block.h - 18.0)
				{
					playerPos.y = block.y + block.h + pRect.h / 2.0;
					velocityY = 0.0;
				}
			}
			};

		for (const auto& block : stage.blocks) checkVerticalBlock(block);

		// ★ 透明（隠し）ブロックの判定・出現（叩き）処理
		for (auto& hBlock : stage.hiddenBlocks)
		{
			const RectF pRect = getPlayerRect(playerPos);

			if (hBlock.isRevealed)
			{
				checkVerticalBlock(hBlock.rect);
			}
			else
			{
				if (pRect.intersects(hBlock.rect) && velocityY < 0.0 && (pRect.y - velocityY * dt) >= hBlock.rect.y + hBlock.rect.h - 18.0)
				{
					hBlock.isRevealed = true;
					playerPos.y = hBlock.rect.y + hBlock.rect.h + pRect.h / 2.0;
					velocityY = 0.0;
				}
			}
		}

		// 落とし穴エリアに入っている場合は強制的に接地判定をOFF（空中扱い）
		if (isInPit)
		{
			isGrounded = false;
		}
		else
		{
			isGrounded = landedThisFrame;
		}

		playerPos.x = Math::Clamp(playerPos.x, playerSize / 2.0, GameStage::MapWidth - playerSize / 2.0);

		// 7. ゴール ＆ 敵判定
		const RectF playerRect = getPlayerRect(playerPos);

		if (playerRect.intersects(stage.getGoalRect()))
		{
			isGameClear = true;
			gameTimer.pause();
		}

		for (auto& enemy : enemyManager.getEnemies())
		{
			if (!enemy.isAlive) continue;

			if (playerRect.intersects(enemy.getCircle()))
			{
				const bool isStomping = (velocityY > 0.0) && (playerPos.y < enemy.pos.y - 10.0);

				if (isStomping)
				{
					enemy.isAlive = false;
					velocityY = stompJumpForce;
				}
				else
				{
					isGameOver = true;
					gameTimer.pause();
				}
			}
		}

		// 8. 描画
		{
			const auto transformer = camera.createTransformer();

			stage.drawWorld();
			enemyManager.draw(playerTexture);

			if (velocityX < 0)      isFacingLeft = true;
			else if (velocityX > 0) isFacingLeft = false;

			if (isFacingLeft) playerTexture.resized(playerSize).mirrored().drawAt(playerPos);
			else playerTexture.resized(playerSize).drawAt(playerPos);
		}

		const int32 remainingTime = gameTimer.getRemainingSeconds();
		font(U"TIME: {:03d}"_fmt(remainingTime)).draw(Arg::topRight = Vec2{ 770, 20 }, Palette::Yellow);
	}
}
