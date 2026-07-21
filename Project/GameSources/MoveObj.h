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
	class MoveObj : public BaseEnemy
	{
	private:
		Vec3 m_scale;
		Vec3 m_pos;
		Vec3 m_rot;
		Vec3 m_moveDirection;
		Vec3 m_startPos;
		
		float m_moveSpeed = 1.0f;
		float m_moveAmount;

		static const float PUSH_POWER;
	public:
		// 構築と破棄
		MoveObj(
			const shared_ptr<Stage>& stage,
			const Vec3& Scale,//サイズ
			const Vec3& Rot,//回転
			const Vec3& Position,//位置
			const Vec3& MoveDirection//移動の向き
			):
			BaseEnemy(stage),
			m_scale(Scale),
			m_rot(Rot),
			m_pos(Position),
			m_moveDirection(MoveDirection)
		{}

		virtual ~MoveObj()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		void OnCollisionEnter(std::shared_ptr<GameObject>& obj);
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj);
		//void OnCollisionExit(std::shared_ptr<GameObject>& obj);

	private:
		void PushPlayer(std::shared_ptr<Player> player);
	};

}
//end basecross
