/*!
@file Player.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"

namespace basecross {

class UpDownFloor;

	// GameObjectクラスを継承した「Player」クラスを定義
	class Player : public GameObject // GameObjectクラスの継承【必須】
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTStaticDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<UpDownFloor> m_currentFloor;//現在乗っている床
		float m_height;
		float m_radius;

		float m_moveSpeed;
		float m_maxSpeed;
		Vec3 m_moveDir;
		Vec3 m_pos;

		float m_ink;	//インク残量
		float m_inkMax;	//インク最大残量

		Vec3 m_externalVelocity; // 外部からの速度
		
	public :
		// ステージを引数にしたコンストラクタ【必須】
		Player(const std::shared_ptr<Stage>& stage) :
			GameObject(stage), // ステージをGameObjectに渡す【必須】
			m_height(1.5f),
			m_radius(0.70f),
			m_moveSpeed(0.0f),
			m_maxSpeed(4.0f),
			m_moveDir(Vec3(0)),
			m_pos(Vec3(0)),
			m_ink(0.0f),
			m_inkMax(10.0f),
			m_externalVelocity(Vec3(0))
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)

		void OnMove();
		void DropInk();

		void OnCollisionEnter(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExcute(std::shared_ptr<GameObject>& obj) override;
		void OnCollisionExit(std::shared_ptr<GameObject>& obj)override;

		void UpdateMoveFloor();

		// 外部からの移動を加算する関数
		//void AddExternalMove(const Vec3& move);
		//void SetExternalVelocity(const Vec3& v) { m_externalVelocity = v; }
	};

}
//end basecross

