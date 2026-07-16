/*!
@file Port.h
@brief ポート
*/

#pragma once
#include "stdafx.h"
#include "Texture2DrawComp.h"
#include "Electrified.h"
#include "StageObjBase.h"

namespace basecross {
	class Port : public StageObjBase
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		std::shared_ptr<Texture2DrawComp> m_staticDraw;

		bool isConnect = false;
		bool isPower = false;

	public:
		// ステージを引数にしたコンストラクタ【必須】
		Port(const std::shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position) :
			StageObjBase(stage, Scale, Rot, Position,L"Port") // ステージをGameObjectに渡す【必須】
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)

		bool GetConnect() const
		{
			return isConnect;
		}

		void SetConnect(bool connect)
		{
			isConnect = connect;
		}

		void SetisPower(bool power)
		{
			isPower = power;
		}
	};
}
//end basecross}
