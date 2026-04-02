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
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_statidDraw = AddComponent<PNTStaticDraw>();
		m_statidDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_statidDraw->SetDiffuse(Col4(1, 0, 0, 1));

		auto coll = AddComponent<CollisionObb>();
	}

	// 更新処理
	void Port::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();

		isConnect = false;

	}

	void Port::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto ink = std::dynamic_pointer_cast<InkDraw>(obj))
		{
			isConnect = true;
			m_statidDraw->SetDiffuse(Col4(1, 1, 0, 1));
		}
	}

	//通電したときの処理
	void Port::Energized()
	{

	}
}
//end basecross

