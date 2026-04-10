/*!
@file Electrified.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross{


	//--------------------------------------------------------------------------------------
	//	class GenericSprite : public GameObject;
	//--------------------------------------------------------------------------------------
	class Electrified : public GameObject
	{
	public:
		// 構築と破棄
		Electrified(const shared_ptr<Stage>& stage) :
			GameObject(stage)
		{
		}
		virtual ~Electrified()
		{
		}

		virtual void OnCreate() override; // 初期化
		//virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画

		bool isPower;//電源かどうか
		float e = 0.0f;//電気の強さ
		vector<shared_ptr<GameObject>> targets;//電気の影響を受けるオブジェクト

		void OnCollisionEnter(shared_ptr<GameObject>& info) override;
		void OnCollisionExit(shared_ptr<GameObject>& info) override;
		void Electrify(shared_ptr<GameObject>& target);
		void OnUpdate() override;
	};

}
//end basecross
