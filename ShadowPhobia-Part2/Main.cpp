#include <Siv3D.hpp>

void Main()
{
	Window::Resize(800, 600);

	// --- 画像の読み込み ---
	const Texture playerTexture{ U"player.png" };

	if (not playerTexture)
	{
		throw Error{ U"画像ファイルの読み込みに失敗しました。Appフォルダに画像があるか確認してください。" };
	}

	// ★★★ プレイヤーの描画サイズを大きく変更（60.0 -> 120.0） ★★★
	// ここを 150.0 や 200.0 にするとさらに大きくなります
	const double playerSize = 120.0;

	// 地面のベースとなるY座標
	const double groundBaseY = 540.0;
	// プレイヤーの中心が位置すべき地面のY座標（サイズに合わせて自動計算）
	const double groundY = groundBaseY - (playerSize / 2.0);

	// プレイヤーの初期位置（画面中央、地面の上）
	Vec2 playerPos{ 400, groundY };

	// 移動・慣性関連のパラメータ
	double velocityX = 0.0;
	const double moveAccel = 1500.0;
	const double airAccel = 800.0;
	const double maxSpeed = 300.0;
	const double friction = 8.0;

	// ジャンプ・物理関連のパラメータ
	double velocityY = 0.0;
	const double gravity = 980.0;
	const double jumpForce = -500.0;  // ※画像が重そうに感じたら -600.0 などにすると高く飛びます
	bool isGrounded = true;

	while (System::Update())
	{
		const double dt = Scene::DeltaTime();

		// 1. 左右の入力方向を判定
		double inputX = 0.0;
		if (KeyLeft.pressed() || KeyA.pressed())  inputX -= 1.0;
		if (KeyRight.pressed() || KeyD.pressed()) inputX += 1.0;

		// 2. 慣性処理
		if (inputX != 0.0)
		{
			const double accel = isGrounded ? moveAccel : airAccel;
			velocityX += inputX * accel * dt;
			velocityX = Math::Clamp(velocityX, -maxSpeed, maxSpeed);
		}
		else
		{
			velocityX -= velocityX * friction * dt;
			if (Abs(velocityX) < 1.0) velocityX = 0.0;
		}

		playerPos.x += velocityX * dt;

		// 3. ジャンプ
		if (KeySpace.down() && isGrounded)
		{
			velocityY = jumpForce;
			isGrounded = false;
		}

		// 4. 縦方向の物理演算
		if (!isGrounded)
		{
			velocityY += gravity * dt;
		}
		playerPos.y += velocityY * dt;

		// 5. 着地判定
		if (playerPos.y >= groundY)
		{
			playerPos.y = groundY;
			velocityY = 0.0;
			isGrounded = true;
		}

		// 画面外への飛び出し制限（サイズに合わせて壁の衝突位置も自動調整）
		playerPos.x = Math::Clamp(playerPos.x, playerSize / 2.0, 800.0 - playerSize / 2.0);

		// --- 描画処理 ---
		// 地面の描画
		Line{ 0, groundBaseY, 800, groundBaseY }.draw(4, Palette::White);

		// 6. 画像の向き（反転）処理と描画
		if (velocityX < 0)
		{
			// 左に動いているときは画像を左右反転
			playerTexture.resized(playerSize).mirrored().drawAt(playerPos);
		}
		else if (velocityX > 0)
		{
			// 右に動いているときはそのまま
			playerTexture.resized(playerSize).drawAt(playerPos);
		}
		else
		{
			// 止まっているときは最後に動いていた向き（今回はデフォルト）で描画
			playerTexture.resized(playerSize).drawAt(playerPos);
		}
	}
}
