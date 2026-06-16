/*!
@file PowerSupply.h
@brief 電源
*/

#pragma once
#include "stdafx.h"
#include "Texture2DrawComp.h"
#include "StageObjBase.h"
#include "Electrified.h"

namespace basecross {
	class Texture2DrawComp;

	class PowerSupply : public StageObjBase
	{
		std::shared_ptr<Transform> m_transform; // トランスフォームはよく使うのでメンバにしておく
		//std::shared_ptr<PNTDXModelDraw> m_draw; // ドローコンポーネント
		std::shared_ptr<Texture2DrawComp> m_staticDraw;

	public:
		// ステージを引数にしたコンストラクタ【必須】
		PowerSupply(const std::shared_ptr<Stage>& stage,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Pos
		) :
			StageObjBase(stage, Scale, Rot,Pos)
		{
		}

		void OnCreate() override; // 初期設定用の関数(UnityのStartメソッドに相当)
		void OnUpdate() override; // 毎フレーム実行される関数(UnityのUpdateメソッドに相当)

	};

}
//end basecross

