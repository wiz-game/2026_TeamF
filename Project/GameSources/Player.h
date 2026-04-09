/*!
@file Player.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "MainCamera.h"
#include "PNTDXModelDraw.h"

namespace basecross {
	// GameObjectクラスを継承した「Player」クラスを定義
	class Player : public GameObject // GameObjectクラスの継承【必須】
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント

		std::weak_ptr<MainCamera> m_camera;

		float m_height;
		float m_radius;

		float m_moveSpeed;
		float m_maxSpeed;	//最大速度
		float m_accel;	//加速度
		Vec3 m_pos;
		Vec3 m_forward;
		Vec3 m_velocity;

		float m_ink;	//インク残量
		float m_inkMax;	//インク最大残量
		float m_inkDecrease;	//インク減少量
		bool m_isDraw;	//インクを塗れるかどうか
		
	public :
		// ステージを引数にしたコンストラクタ【必須】
		Player(const std::shared_ptr<Stage>& stage) :
			GameObject(stage), // ステージをGameObjectに渡す【必須】
			m_height(1.0f),
			m_radius(0.49f),
			m_moveSpeed(0.0f),
			m_maxSpeed(4.0f),
			m_accel(0.99f),
			m_pos(Vec3(0)),
			m_forward(Vec3(0)),
			m_velocity(Vec3(0)),
			m_ink(0.0f),
			m_inkMax(10.0f),
			m_inkDecrease(1.0f),
			m_isDraw(true)
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)

		void OnMove();
		void DropInk();
		void OnDied();

	};

}
//end basecross

