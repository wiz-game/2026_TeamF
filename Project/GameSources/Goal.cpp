/*!
@file Goal.cpp
@brief ゴール実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//初期化
	void Goal::OnCreate()
	{
		m_transform = GetComponent<Transform>();
		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);

		// ドローコンポーネントを追加
		//m_draw = AddComponent<PNTDXModelDraw>();
		//m_draw->SetMeshResource(L"DEFAULT_CUBE");

		//auto draw = AddComponent<PNTStaticDraw>();
		//draw->SetMeshResource(L"DEFAULT_CUBE");
		//draw->SetDiffuse(Col4(0, 1, 1, 1));

		//コリジョン
		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);
		coll->SetFixed(true);//固定
		coll->SetDrawActive(true);
		SetAlphaActive(true);
	}

	void Goal::OnCollisionEnter(std::shared_ptr<GameObject>& target)
	{
		auto scene = App::GetApp()->GetScene<Scene>();
		std::wstringstream wss;

		if (auto player = std::dynamic_pointer_cast<Player>(target))
		{
			wss << L"ゴール";
			scene->SetDebugString(wss.str());
		}
	}
}
//end basecross
