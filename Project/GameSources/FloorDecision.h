/*!
@file FloorDecision.h
@brief 上下する床の判定
*/

#pragma once
#include "stdafx.h"
#include "UpDownFloor.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	class FloorDecision : public GameObject;
	//--------------------------------------------------------------------------------------
	class FloorDecision : public GameObject
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<CollisionObb> m_coll;

		std::shared_ptr<UpDownFloor> m_updownFloor;

		Vec3 m_scale = Vec3(2.0f, 0.1f, 2.0f);

		bool m_getOn = false;

	public:
		// 構築と破棄
		FloorDecision(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{
		}
		virtual ~FloorDecision()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		void OnCollisionEnter(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExit(std::shared_ptr<GameObject>& obj)override;


		bool GetGetOn() const
		{
			return m_getOn;
		}
	};

}
//end basecross
