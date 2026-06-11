/*!
@file BaseEnemy.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "Floor.h"
namespace basecross {


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class BaseEnemy : public GameObject
	{

	public:
		// 構築と破棄
		BaseEnemy(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{
		}
		virtual ~BaseEnemy()
		{
		}

		enum class State {
			Idle,
			Patrol,
			Chase,
			Draw,
			Erase,
		};

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

	protected:

		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<Transform> m_transform; // トランスフォームコンポーネント

		float m_moveSpeed = 1.0f; // 移動速度
		float m_range = 5.0f;//徘徊範囲
		float m_srachRange; // 探索範囲
		float m_diatance;
		float m_groundY = 0.0f;
		float m_heightOffset = 0.5f;
		float m_rayLength = 5.0f;

		int m_groundCount = 0;

		bool m_isGround = true; // 地面にいるかどうか

		Vec3 m_origin;//UpdatePatrolのメンバ変数
		Vec3 m_targetPos;

		Vec3 m_pointA;//UpdateBetWeenのメンバ変数
		Vec3 m_pointB;

		State m_state; // 現在の状態

		virtual void UpdateIdle(); // 待機状態の更新
		virtual void UpdatePatrol(); // 徘徊移動の更新
		virtual void UpdateChase(); // 追跡の更新
		virtual void UpdateInkDrow(); // 描画状態の更新
		virtual void UpdateInkErase(); // 消去状態の更新

		void SpanInk(const Vec3& pos);// インクをスポーンする関数
		void OnCollisionEnter(shared_ptr<GameObject>& obj);
		void OnCollisionExit(shared_ptr<GameObject>& obj);
		void UpdateGroundRay(); // 地面との接触を更新する関数
	};

}
//end basecross
