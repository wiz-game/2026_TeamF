/*!
@file PowerSupply.cpp
@brief 電源実体
*/

#include "stdafx.h"
#include "PowerSupply.h"
#include "game_controller.h"
#include "InkDraw.h"

namespace basecross {
	// 初期設定
	void PowerSupply::OnCreate()
	{
		this->isPower = true;	// 電源であることを設定
		GetStage()->SetSharedGameObject(L"PowerSupply", GetThis<PowerSupply>());
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetDiffuse(Col4(0, 1, 0, 1));

		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);

		//coll->SetFixed(true);
		isPower = true;// 電源であることを設定
	}

	// 更新処理
	void PowerSupply::OnUpdate()
	{
	}

	void PowerSupply::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto ink = std::dynamic_pointer_cast<InkCloud>(obj))
		{
			if (isPower)
			{
				isConnect = true;
				m_staticDraw->SetDiffuse(Col4(1, 1, 0, 1));
			}
			else
			{
				isConnect = false;
				m_staticDraw->SetDiffuse(Col4(0, 1, 0, 1));
			}
		}
	}
}
//end basecross

