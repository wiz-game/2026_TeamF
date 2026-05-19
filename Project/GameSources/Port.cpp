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

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetDiffuse(Col4(1, 0, 0, 1));

		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);

		AddComponent<Electrified>();
		InkConnectChecker::Get().AddPort(GetThis<Port>());
	}

	// 更新処理
	void Port::OnUpdate()
	{
		// アプリケーションオブジェクトを取得
		auto& app = App::GetApp();
		auto elec = GetComponent<Electrified>();
		//電流の更新
		elec->UpdateElectrified();
		auto power = elec->IsPowered();//電流が流れているかどうかを更新

		// 電流の状態に応じて接続状態と色を更新
		if (power)
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

	void Port::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto elec = GetComponent<Electrified>(false))
		{
			//リストに追加
			elec->OnElectrifiedEnter(obj);
		}
	}

	void Port::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		if (auto elec = GetComponent<Electrified>(false))
		{
			elec->OnElectrifiedExit(obj);
		}
	}
}
//end basecross

