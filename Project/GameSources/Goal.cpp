/*!
@file Goal.cpp
@brief ゴール実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Sprite.h"

namespace basecross {
	//初期化
	void Goal::OnCreate()
	{
		//モデルのサイズ調整
		Mat4x4 spanMat;
		spanMat.affineTransformation(
			Vec3(1.0f, 1.0f, 1.0f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, 0.0f, 0.0f),
			Vec3(0.0f, 0.0f, 0.0f)
		);


		m_transform = GetComponent<Transform>();
		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);
		m_transform->SetRotation(m_rot);

		// ドローコンポーネントを追加
		//m_staticDraw = AddComponent<PNTStaticDraw>();
		//m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		//m_staticDraw->SetOwnShadowActive(true);

		m_draw = AddComponent<PNTBoneModelDraw>();
		m_draw->SetMeshResource(L"PRINTER_MODEL");
		m_draw->SetTextureResource(L"PRINTER_TEX");
		m_draw->SetMeshToTransformMatrix(spanMat);
		//m_draw->SetRasterizerState(RasterizerState::CullNone);

		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"PRINTER_MODEL");

		m_draw->SetDrawActive(true);
		m_draw->AddAnimation(L"PRINTER_MODEL", 0, 120, false);
		
		m_resultSprite = GetStage()->AddGameObject<Sprite>(L"PRINTER_TEX", Vec3(0,0,0), Vec2(100, 100),Anchor::Center);
		m_resultSprite->SetDrawActive(false);
		m_resultSprite->SetDrawLayer(1);

		try
		{	// objectの取得
			m_player = GetStage()->GetSharedGameObject<Player>(L"Player");
		}
		catch (...) {
			m_player.reset();
		}
	}

	void Goal::OnUpdate()
	{
		if (!m_player) return; // プレイヤーがいなければ何もしない
		if (m_port == nullptr) return;//ポートが指定されていなければ何もしない

		auto scene = App::GetApp()->GetScene<Scene>();
		auto delta = App::GetApp()->GetElapsedTime();

		switch (m_state)
		{
		case basecross::Goal::State::Wait:

			//通電していればアニメーションスタート
			if (m_port->GetConnect())
			{
				//通電したらアニメーションを動かす
				m_draw->UpdateAnimation(delta * 0.5f);
				m_state = State::Vibrate;
				m_animatimer = 0.0f;
			}
			break;

		case basecross::Goal::State::Vibrate:
			//振動アニメーション
			m_draw->UpdateAnimation(delta * 0.5f);
			VibrateAnimation(delta);
			break;

		case basecross::Goal::State::SpriteScale:
			//スプライトアニメーション
			m_resultSprite->SetDrawActive(true);//スプライトの表示
			SpriteAnimation(delta);
			break;

		case basecross::Goal::State::End:
			//演出終了、画面遷移
			//PostEvent(0.0f, GetThis<GoalStage>(), scene, L"ToGoalStage");

			break;
		}
	}

	//振動アニメーション
	void Goal::VibrateAnimation(float delta)
	{
		auto vidration = 0.01f;//振動する幅
		auto timerStop = 7.5f;
		m_animatimer += delta;

		if (m_animatimer >= timerStop)
		{
			//元の位置に戻す
			m_pos = m_startPos;
			m_transform->SetPosition(m_pos);

			//タイマーをリセット
			m_animatimer = 0.0f;

			auto view = GetStage()->GetView();
			if (view)
			{
				auto camera = view->GetTargetCamera();
				if (camera)
				{
					// 1. カメラのビュー・射影行列を取得
					Mat4x4 viewMatrix = camera->GetViewMatrix();
					Mat4x4 projMatrix = camera->GetProjMatrix();

					// 2. 先に行列同士を掛け合わせる（ビュー × プロジェクション）
					Mat4x4 viewProj = viewMatrix * projMatrix;

					// 3. 3D位置に合成行列を掛ける
					Vec3 screenSpacePos = m_pos * viewProj;

					float w = m_pos.x * viewProj._14 + m_pos.y * viewProj._24 + m_pos.z * viewProj._34 + viewProj._44;

					if (w != 0.0f)
					{
						screenSpacePos.x /= w;
						screenSpacePos.y /= w;
					}

					// 3. 画面中央(0,0)基準の2D座標へ変換する
					float halfWidth = App::GetApp()->GetGameWidth() * 0.5f;
					float halfHeight = App::GetApp()->GetGameHeight() * 0.5f;

					m_spriteStartPos2D.x = screenSpacePos.x * halfWidth;
					m_spriteStartPos2D.y = screenSpacePos.y * halfHeight;
				}
			}

			//スプライトアニメーションステートへ
			m_state = State::SpriteScale;
		}
		else
		{
			//振動スタート
			m_pos.x += m_speed * delta;

			auto diff = abs(m_pos.x - m_startPos.x);

			//反転
			if (diff >= vidration) //振動幅
			{
				m_speed *= -1;
			}

			m_transform->SetPosition(m_pos);
		}
	}

	void Goal::SpriteAnimation(float delta)
	{
		m_animatimer += delta;
		auto duration = 2.0f;	//演出は２秒間

		//0.0f ~ 0.1f の割合を計算
		float t = m_animatimer / duration;
		if (t > 1.0f) t = 1.0f;

		//サイズ補間計算
		Vec3 startSize = Vec3(100.0f, 100.0f,100.0f);
		Vec3 endSize = Vec3(1280.0f, 840.0f,1.0f);

		Vec3 currentSize = startSize * (1.0f - t) + endSize * t;

		Vec2 targetPos = Vec2(0.0f);//画面中央
		Vec2 m_spritePos2D = m_spriteStartPos2D + (targetPos - m_spriteStartPos2D) * t;

		m_resultSprite->SetPosition(Vec3(m_spritePos2D.x, m_spritePos2D.y,0.0f));
		m_resultSprite->SetSize(Vec2(currentSize.x, currentSize.y));

		if (t >= 1.0f)
		{
			m_state = State::End;
		}
	}
}
//end basecross
