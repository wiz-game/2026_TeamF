#include "stdafx.h"
#include "InkParticle.h"

namespace basecross
{
	void InkParticle::OnCreate()
	{
		m_DrawComp = AddComponent<PNTStaticDraw>();
		m_DrawComp->SetMeshResource(L"DEFAULT_SPHERE");// デフォルトの球体メッシュ
		m_DrawComp->SetDiffuse(Col4(0.0f, 0.0f, 0.0f, 1.0f));// 黒色に設定
		m_DrawComp->SetEmissive(Col4(0.0f, 0.0f, 0.0f, 1.0f));// 黒色に設定

		m_Transform = AddComponent<Transform>();
		m_Transform->SetPosition(m_Position);
		m_Transform->SetScale(Vec3(m_Size, m_Size, m_Size));

		SetAlphaActive(true);

		//ランダムなぶれを足して扇状に広げる
		m_Direction.x += ((float)rand() / RAND_MAX - 0.5f);
		m_Direction.z += ((float)rand() / RAND_MAX - 0.5f);
		m_Direction.y += 0.2;	//少し上向きに飛ばす
		m_Direction.normalize();

		m_Velocity = m_Direction * m_Speed;
		m_LimitTime = m_MaxLimitTime;
	}

	void InkParticle::OnUpdate()
	{
		float delta = App::GetApp()->GetElapsedTime();

		m_LimitTime -= delta;

		//パーティクルの寿命が尽きたら、ゲームオブジェクトを削除
		if (m_LimitTime <= 0.0f)
		{
			GetStage()->RemoveGameObject<InkParticle>(GetThis<InkParticle>());
			return;
		}

		m_Velocity.y -= m_Gravity * delta;// 重力の影響を受ける
		m_Position += m_Velocity * delta;// 位置を更新

		float alpha = m_LimitTime / m_MaxLimitTime;
		m_DrawComp->SetDiffuse(Col4(0.0f, 0.0f, 0.0f, alpha));// 透明度を寿命に応じて変化))
		m_DrawComp->SetEmissive(Col4(0.0f, 0.0f, 0.0f, alpha));// 黒色に設定

		m_Transform->SetPosition(m_Position);
	}
}