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
		//m_draw->SetRasterizerState(RasterizerState::CullNone);

		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"PRINTER_MODEL");

		m_draw->SetDrawActive(true);
		m_draw->AddAnimation(L"PRINTER_MODEL", 0, 120, false);

		m_resultSprite = GetStage()->AddGameObject<Sprite>(L"BACKGROUND", Vec3(0, 0, 0), Vec2(100, 100), Anchor::Center);
		m_resultSprite->SetDrawActive(false);
		m_resultSprite->SetDrawLayer(1);

		try
		{	// objectの取得
			//m_player = GetStage()->GetSharedGameObject<Player>(L"Player");

			GoalEreaEffect::InitParams params; // オーラエフェクトに渡すパラメータをまとめた構造体（BaseCrossはAddGameObjectの際、引数が分かりづらいのでまとめると良い）
			params.textureKey = L"line";//テクスチャ
			params.sides = 30;			//面の数
			params.height = 0.0f;		//筒の高さ
			params.topRadius = 10.0f;	//筒の上のわっかの半径
			params.bottomRadius = 1.0f; //下のわっかの半径
			params.topColor = Col4(1.0f, 1.0f, 0.0f, 0.2f);		//上の方の色
			params.bottomColor = Col4(1.0f, 1.0f, 0.0f, 1.0f);	//下の方の色
			params.uvOffsetSpeed = Vec2(0.0f, 2.0f);			//アニメーションの速さ(テクスチャをずらす)
			params.textureLoops = 5.0f;							//テクスチャのループ

			auto light = GetStage()->AddGameObject<GoalEreaEffect>(params);
			light->GetComponent<Transform>()->SetPosition(m_pos.x,m_pos.y + 1.0f,m_pos.z);

		}
		catch (...) {
			m_player.reset();
		}
	}

	void Goal::OnUpdate()
	{
		//if (!m_player) return; // プレイヤーがいなければ何もしない
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
			//演出終了、画面遷移
			PostEvent(0.0f, GetThis<Goal>(), scene, L"ToGoalStage");

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

		if (t >= 1.0f)
		{
			m_state = State::End;
		}
	}

	//初期化
	void GoalEreaEffect::OnCreate()
	{
		// 頂点データの作成する
		for (int i = 0; i < params.sides + 1; i++) // 1周して戻ってきて、出発点にも頂点が必要なため、分割数より1回多くループする
		{
			float fSides = static_cast<float>(params.sides); // 円の分割数をfloat型に変換する
			float radian = XMConvertToRadians(i * 360.0f / fSides); // 頂点を配置する角度を算出する
			vertices.push_back({ Vec3(params.topRadius * cosf(radian), params.height, params.topRadius * sinf(radian)), params.topColor, Vec2(i / fSides, 0.0f) }); // 上の頂点
			vertices.push_back({ Vec3(params.bottomRadius * cosf(radian), 0.0f, params.bottomRadius * sinf(radian)), params.bottomColor, Vec2(i / fSides, params.textureLoops) }); // 下の頂点
		}

		// ループ処理のベースにするインデックス
		uint16_t baseIndices[] = {
			1, 0, 2,
			1, 2, 3,
		};

		// ベースインデックスをもとに頂点インデックスを作成する
		std::vector<uint16_t> indices;
		//面の数だけループ
		for (int i = 0; i < params.sides; i++)
		{
			//一枚の板ポリ（四角）を形成
			for (auto baseIndex : baseIndices)
			{
				indices.push_back(baseIndex + 2 * i); // ベースインデックスを２ずつずらして設定していく
			}
		}

		// ドローコンポーネントを追加する
		auto drawComp = AddComponent<BcPCTStaticDraw>(); // Bc系のコンポーネントだと、頂点の色でアルファブレンドしてくれる
		drawComp->CreateOriginalMesh(vertices, indices); // 上記の設定をもとにメッシュを生成する
		drawComp->SetOriginalMeshUse(true); // 独自メッシュ(プログラム内で作成)を使用することを宣言する
		if (params.textureKey != L"") // テクスチャを使用する場合の設定
		{
			drawComp->SetTextureResource(params.textureKey); // 使用するテクスチャを設定する
		}
		drawComp->SetBlendState(BlendState::Additive); // 加算合成されるように設定する
		drawComp->SetDepthStencilState(DepthStencilState::Read); // 他の板ポリと干渉しないようにする]
		if (params.textureLoops > 1.0f || params.uvOffsetSpeed.length() != 0.0f) // ループアニメーションや拡大UVを使用する場合はテクスチャが繰り返しに対応できるように設定する
		{
			drawComp->SetSamplerState(SamplerState::AnisotropicWrap); // テクスチャアニメーション用にループ貼り付けできるようにする
		}

		SetAlphaActive(true); // 透過処理を有効にする
		SetDrawLayer(1);
	}

	void GoalEreaEffect::OnUpdate()
	{
		auto& app = App::GetApp();
		float delta = app->GetElapsedTime();

		for (auto& vertex : vertices) // テクスチャアニメーションさせる
		{
			vertex.textureCoordinate += delta * params.uvOffsetSpeed; // すべての頂点のUVを秒速でずらす
		}

		auto drawComp = GetComponent<BcPCTStaticDraw>();
		drawComp->UpdateVertices(vertices); // 更新された頂点データを再適用する
	}

}
//end basecross
