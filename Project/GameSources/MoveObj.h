/*!
@file MoveObj.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class MoveObj : public StageObjBase
	{
		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<Transform> m_transform; // トランスフォームコンポーネント

	private:
		Vec3 m_scale;
		Vec3 m_pos;
		Vec3 m_rot;
		Vec3 m_startPos;
		Vec3 m_moveFloorVec;
		Vec3 m_currentMoveVec;

		float m_moveAmount;
		float m_gravity = -9.8f;
		float m_velocity = 0.0f;

		bool m_isGround = false;

		static const float PUSH_POWER;
		static const float GRAVITY;

	public:
		// 構築と破棄
		MoveObj(
			const shared_ptr<Stage>& stage,
			const Vec3& Scale,//サイズ
			const Vec3& Rot,//回転
			const Vec3& Position//位置
			):
			StageObjBase(stage, Scale,Rot,Position,L"MoveObj"),
			m_scale(Scale),
			m_rot(Rot),
			m_pos(Position)
		{}

		virtual ~MoveObj()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		void OnCollisionEnter(std::shared_ptr<GameObject>& obj);
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj);
		void OnCollisionExit(std::shared_ptr<GameObject>& obj);

		void UpdateMoveFloor(const Vec3& movePos);

	private:
		void PushPlayer(std::shared_ptr<Player> player);
		void UpdateGravity(float delta);
	};

}
//end basecross
