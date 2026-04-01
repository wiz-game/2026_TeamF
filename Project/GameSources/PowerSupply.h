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

		float m_elect = 100.0f;

	public:
		// ステージを引数にしたコンストラクタ【必須】
		PowerSupply(const std::shared_ptr<Stage>& stage) :
			GameObject(stage) // ステージをGameObjectに渡す【必須】
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)

		void Connected();//インクとつながった時の関数
	};

}
//end basecross

