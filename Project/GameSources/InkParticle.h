#pragma once
#include "stdafx.h"

namespace basecross 
{
	class InkParticle : public GameObject
	{
		const float m_Speed = 5.0f;	// 速度
		const float m_Size = 0.1f;	// サイズ
		const float m_Gravity = 9.8f;	// 重力
		float m_MaxLimitTime = 0.3f;	// パーティクルの寿命

		Vec3 m_Direction;	// パーティクルの方向
		Vec3 m_Velocity;	// パーティクルの速度
		Vec3 m_Position;	// パーティクルの位置

		float m_LimitTime = 0.3f;	// パーティクルの寿命

		std::shared_ptr<PNTStaticDraw> m_DrawComp;	// パーティクルのメッシュ
		std::shared_ptr<Transform> m_Transform;	// パーティクルのトランスフォーム

	public:
		InkParticle(std::shared_ptr<Stage> stage, Vec3& pos, Vec3& dir):
			GameObject(stage),
			m_Position(pos),
			m_Direction(dir)
		{
		}
		
		virtual ~InkParticle() {};
		void OnCreate();
		void OnUpdate();
		//void OnDraw();
	};
}