/*!
@file PowerSupply.h
@brief 電源
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"
#include "Electrified.h"

namespace basecross {
	class PowerSupply : public GameObject
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<PNTDXModelDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		Vec3 m_pos = Vec3(0.0f, -0.3f, -4.0f);
		Vec3 m_scale = Vec3(1.0f, 0.1f, 1.0f);

		bool isConnect = false;
		bool isPower = true;

	public:
		// ステージを引数にしたコンストラクタ【必須】
		PowerSupply(const std::shared_ptr<Stage>& stage) :
			GameObject(stage)
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)
		void OnCollisionEnter(std::shared_ptr<GameObject>& obj);	//インクとの当たり判定

		bool GetConnect() const
		{
			return isConnect;
		}

		bool GetisPower() const
		{
			return isPower;
		}
	};

}
//end basecross

