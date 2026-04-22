/*!
@file Port.cpp
@brief ポート実体
*/

#include "stdafx.h"
#include "Project.h"
#include "game_controller.h"

namespace basecross {
	// 初期設定
	void Port::OnCreate()
	{
		GetStage()->SetSharedGameObject(L"Port", GetThis<Port>());
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetDiffuse(Col4(1, 0, 0, 1));

		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);

		m_powerSupply = GetStage()->GetSharedGameObject<PowerSupply>(L"PowerSupply");
	}

	// 更新処理
	void Port::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

	}

	void Port::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto ink = std::dynamic_pointer_cast<InkCloud>(obj))
		{
			auto connect = m_powerSupply->GetConnect();
			auto power = ink->GetisPower();
			if (connect && power /*&& m_elect >= 50.0f*/)
			{
				isConnect = true;
				m_staticDraw->SetDiffuse(Col4(1, 1, 0, 1));
			}
			else
			{
				isConnect = false;
				m_staticDraw->SetDiffuse(Col4(1, 0, 0, 1));
			}
		}
	}

	void Port::OnCollisionExcute(std::shared_ptr<GameObject>& obj)
	{
		//if (auto ink = std::dynamic_pointer_cast<InkDraw>(obj))
		//{
		//	if (ink->isPower)
		//	{
		//		isConnect = true;
		//		m_staticDraw->SetDiffuse(Col4(1, 1, 0, 1));
		//	}
		//	else
		//	{
		//		isConnect = false;
		//		m_staticDraw->SetDiffuse(Col4(1, 0, 0, 1));
		//	}
		//}
	}
}
//end basecross

