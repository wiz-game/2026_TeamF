/*!
@file OffsetObject.h
@brief トラップドアの初期の向きを決めるオブジェクト
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	//	class OffsetObject : public GameObject;
	//--------------------------------------------------------------------------------------
	class OffsetObject : public GameObject
	{
		std::shared_ptr<Transform> m_transform;
		std::shared_ptr<GameObject> m_owner = nullptr;//親になるオブジェクト

		Vec3 m_scale = Vec3(0.1f, 0.1f, 0.1f);

	public:
		// 構築と破棄
		OffsetObject(const shared_ptr<Stage>& stage,
			const shared_ptr<GameObject> owner) :
			GameObject(stage),
			m_owner(owner)
		{
		}
		virtual ~OffsetObject()
		{
		}

		virtual void OnCreate() override; // 初期化
		virtual void OnUpdate() override; // 更新
		//virtual void OnDraw() override; // 描画
	};

}
//end basecross
