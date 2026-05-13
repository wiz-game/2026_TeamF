/*!
@file FloorDecision.h
@brief 上下する床の判定
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	class FloorDecision : public GameObject;
	//--------------------------------------------------------------------------------------
	class FloorDecision : public GameObject
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<CollisionObb> m_coll;

		Vec3 m_scale = Vec3(1.5f, 0.1f, 1.5f);

		std::shared_ptr<GameObject> m_owner = nullptr;//親になるオブジェクト

		Vec3 m_currentMoveVec;
		bool m_isPlayerOn = false;//プレイヤーが乗っているかどうか

	public:
		// 構築と破棄
		FloorDecision(const shared_ptr<Stage>& stage,
			const shared_ptr<GameObject> owner) :
			GameObject(stage),
			m_owner(owner),
			m_currentMoveVec(0)
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

		//MoveFloorから移動量を取得する
		void SetCurrentMoveVec(const Vec3& v)
		{
			m_currentMoveVec = v;
		}

		bool GetIsPlayerOn() const
		{
			return m_isPlayerOn;
		}
	};

}
//end basecross
