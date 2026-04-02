/*!
@file Player.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"

namespace basecross {
	// GameObjectクラスを継承した「Player」クラスを定義
	class Player : public GameObject // GameObjectクラスの継承【必須】
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTDXModelDraw> m_draw; // ドローコンポーネント
		float m_height;
		float m_radius;

		float m_moveSpeed;
		Vec3 m_moveDir;
		Vec3 m_pos;

		float m_ink;	//インク残量
		float m_inkMax;	//インク最大残量
		
	public :
		// ステージを引数にしたコンストラクタ【必須】
		Player(const std::shared_ptr<Stage>& stage) :
			GameObject(stage), // ステージをGameObjectに渡す【必須】
			m_height(1.5f),
			m_radius(0.70f),
			m_moveSpeed(2.0f),
			m_moveDir(Vec3(0)),
			m_pos(Vec3(0)),
			m_ink(0.0f),
			m_inkMax(10.0f)
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)

		void OnMove();
		void DropInk();
	};

}
//end basecross

