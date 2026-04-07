/*!
@file PowerSupply.h
@brief 電源
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"

namespace basecross {
	class PowerSupply : public GameObject // GameObjectクラスの継承【必須】
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTDXModelDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		Vec3 m_pos = Vec3(0.0f, 0.0f, -4.0f);
		Vec3 m_scale = Vec3(1.0f, 0.1f, 1.0f);

		float m_elect = 100.0f;
		bool isConnect = false;

	public:
		// ステージを引数にしたコンストラクタ【必須】
		PowerSupply(const std::shared_ptr<Stage>& stage) :
			GameObject(stage) // ステージをGameObjectに渡す【必須】
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)
		void OnCollisionEnter(std::shared_ptr<GameObject>& obj);	//インクとの当たり判定

		float GetElect() const
		{
			return m_elect;
		}

		bool GetConnect() const
		{
			return isConnect;
		}
	};

}
//end basecross

