/*!
@file Goal.cpp
@brief ゴール実体
*/

#include "stdafx.h"
#include "Project.h"
#include "Sprite.h"

namespace basecross {

	void Goal::RegisterResources() {
		auto& app = App::GetApp();
		wstring mediaPath = App::GetApp()->GetDataDirWString();
		app->RegisterTexture(L"BACKGROUND", mediaPath + L"Texture/background.jpg");
		app->RegisterTexture(L"Black", mediaPath + L"Texture/Black.png");
	}

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

		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"PRINTER_MODEL");

		m_draw->SetDrawActive(true);
		m_draw->AddAnimation(L"PRINTER_MODEL", 0, 120, false);

		m_resultSprite = GetStage()->AddGameObject<Sprite>(L"BACKGROUND", Vec3(0, 0, 0), Vec2(100, 100), Anchor::Center);
		m_resultSprite->SetDrawActive(false);
		m_resultSprite->SetDrawLayer(1);

		Vec3 spawnCenter = m_pos + Vec3(0.0f, 0.5f, 0.0f);
		float radius = 2.5f;//回転半径
		try
		{
			//エフェクトの生成
			GetStage()->AddGameObject<GoalEffect>(spawnCenter, Vec3(m_scale.x / 3.0f, m_scale.y / 5.0f, m_scale.z / 3.0f), radius, m_port);
		}
		catch (...) {
		}
	}

	void Goal::OnUpdate()
	{
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

				if (!m_MoveSound_1) {
					m_MoveSound_1 = SoundManager::Get().PlaySE(L"PRINTER_SE_1");
				}
			}
			break;

		case basecross::Goal::State::Vibrate:
			//振動アニメーション
			m_draw->UpdateAnimation(delta * 0.5f);

			if (!m_MoveSound_2) {
				m_MoveSound_2 = SoundManager::Get().PlayLoopSE(L"PRINTER_SE_2");
			}

			VibrateAnimation(delta);
			break;

		case basecross::Goal::State::SpriteScale:
			//スプライトアニメーション
			SoundManager::Get().StopLoopSE(m_MoveSound_2);
			m_resultSprite->SetDrawActive(true);//スプライトの表示
			SpriteAnimation(delta);
			break;

		case basecross::Goal::State::End:
			//scene->SetResultInk(m_player->GetInk(), m_player->GetMaxInk());
			//演出終了、画面遷移
			if (!m_fadeSprite)
			{
				m_fadeSprite = GetStage()->AddGameObject<Sprite>(L"Black", Vec3(0, 0, 0), Vec2(1280, 840), Anchor::Center);
				m_fadeSprite->SetDrawLayer(1);
				m_fadeComp = m_fadeSprite->AddComponent<SpriteFade>(1.5f);
				m_fadeComp->StartFade(FadeState::Out);
			}

			if (m_fadeComp->IsFinish())
			{
				PostEvent(0.0f, GetThis<Goal>(), scene, L"ToGoalStage");
			}
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
					m_spriteStartPos2D.y = screenSpacePos.y * halfHeight - 2.0f;
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

		float t_ease = 1.0f - pow(1.0f - t, 3.0f);

		Vec2 startPos = m_spriteStartPos2D;//元の位置
		Vec2 targetPos = Vec2(0.0f);//画面中央

		//中間地点
		Vec2 controlPos = (startPos + targetPos) * 0.5f;
		controlPos.x += 0.0f;
		controlPos.y += 200.0f;

		//二次ベジエ曲線：B(t) = (1-t)^2 * P0 + 2(1-t)t * P1 + t^2 * P2
		Vec2 m_spritePos2D =
			startPos * (1.0f - t_ease) * (1.0f - t_ease) +
			controlPos * 2.0f * (1.0f - t_ease) * t_ease +
			targetPos * t_ease * t_ease;

		//サイン波による揺らぎ
		float floatOffset = sin(t * XM_PI * 3.0f) * 30.0f * (1.0f - t);
		m_spritePos2D.y += floatOffset;//上下にふわふわさせる

		//サイズ補間計算
		Vec3 startSize = Vec3(100.0f, 100.0f, 100.0f);
		Vec3 endSize = Vec3(1280.0f, 840.0f, 1.0f);

		Vec3 currentSize = startSize * (1.0f - t_ease) + endSize * t_ease;

		m_resultSprite->SetPosition(Vec3(m_spritePos2D.x, m_spritePos2D.y, 0.0f));
		m_resultSprite->SetSize(Vec2(currentSize.x, currentSize.y));

		if (t >= 0.8f)
		{
			m_state = State::End;
		}
	}

	//------------------------------------------------------------------
	//		エフェクト
	//------------------------------------------------------------------
	void GoalParticle::OnCreate()
	{
		m_trans = GetComponent<Transform>();
		m_trans->SetScale(m_scale);

		m_draw = AddComponent<PNTStaticDraw>();
		m_draw->SetMeshResource(L"DEFAULT_SPHERE");
		SetAlphaActive(true);

		m_draw->SetBlendState(BlendState::AlphaBlend);
		m_draw->SetEmissive(Col4(1.0f, 1.0f, 0.0f, m_alpha));
		m_draw->SetDiffuse(Col4(1.0f, 1.0f, 0.0f, m_alpha));

	}

	void GoalParticle::OnUpdate()
	{
		float delta = App::GetApp()->GetElapsedTime();
		if (m_port && m_port->GetConnect())
		{
			m_speed = 15.0f;
		}
		else
		{
			m_speed = 5.0f;
		}
		m_angle += m_speed * delta;
		float x = m_centerPos.x + cosf(m_angle) * m_radius;
		float y = m_centerPos.y;
		float z = m_centerPos.z + sinf(m_angle) * m_radius;
		m_trans->SetPosition(Vec3(x, y, z));

	}

	void GoalEffect::OnCreate()
	{
		float interval = 0.05f;//球体同士の間隔

		for (int i = 0; i < m_total; i++)
		{
			float startAngle = i * interval;


			float baseRate = (m_total > 1) ? (float)i / (float)(m_total - 1) : 0.0f;
			float alpha = 0.1f + (baseRate * 0.9f);
			float scale = 0.05f + (baseRate * 0.35f);

			//球体の生成
			auto particle = GetStage()->AddGameObject<GoalParticle>(
				m_centerPos, m_scale, m_radius, startAngle, alpha, m_port);
		}
	}
}
//end basecross
