/*!
@file Port.h
@brief ポート
*/

#pragma once
#include "stdafx.h"
#include "PNTDXModelDraw.h"
#include "Electrified.h"
#include "StageObjBase.h"

namespace basecross {
	class Port : public StageObjBase
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		//std::shared_ptr<PNTDXModelDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<PNTStaticDraw> m_staticDraw;

		bool isConnect = false;
		bool isPower = false;

	public:
		// ステージを引数にしたコンストラクタ【必須】
		Port(const std::shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position) :
			StageObjBase(stage, Scale, Rot, Position) // ステージをGameObjectに渡す【必須】
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)
		void OnCollisionEnter(std::shared_ptr<GameObject>& obj)override;
		void OnCollisionExit(std::shared_ptr<GameObject>& obj)override;	//インクとの当たり判定

		bool GetConnect() const
		{
			return isConnect;
		}

		void SetisPower(bool power)
		{
			isPower = power;
		}
	};

}
//end basecross}
