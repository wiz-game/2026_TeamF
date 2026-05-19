/*!
@file PowerSupply.cpp
@brief 電源実体
*/

#include "stdafx.h"
#include "PowerSupply.h"
#include "Project.h"
#include "game_controller.h"
#include "InkDraw.h"
#include "TextureCollision.h"
namespace basecross {
	// 初期設定
	void PowerSupply::OnCreate()
	{
		// トランスフォームコンポーネントを取得しておく
		m_transform = GetComponent<Transform>();

		m_transform->SetPosition(m_pos);
		m_transform->SetScale(m_scale);
		m_transform->SetRotation(m_rot);

		// ドローコンポーネントを追加
		m_draw = AddComponent<PNTDXModelDraw>();
		m_draw->SetMeshResource(L"DEFAULT_CUBE");

		m_staticDraw = AddComponent<PNTStaticDraw>();
		m_staticDraw->SetMeshResource(L"DEFAULT_CUBE");
		m_staticDraw->SetDiffuse(Col4(0, 1, 0, 1));

		auto coll = AddComponent<CollisionObb>();
		coll->SetAfterCollision(AfterCollision::None);

		auto elec = AddComponent<Electrified>();
		elec->SetAsSource(true);// 電源であることを設定
		//isPower = true;
		//coll->SetFixed(true);
		isPower = true;// 電源であることを設定

		InkConnectChecker::Get().AddPowerSupply(GetThis<PowerSupply>());
	}

	// 更新処理
	void PowerSupply::OnUpdate()
	{
		auto elec = GetComponent<Electrified>();
		elec->UpdateElectrified();
		auto isPower = elec->IsPowered();//電流が流れているかどうかを更新
		//if (isPower)
		//{
		//	//isConnect = true;
		//	m_staticDraw->SetDiffuse(Col4(1, 1, 0, 1));
		//}
		//else
		//{
		//	//isConnect = false;
		//	m_staticDraw->SetDiffuse(Col4(0, 1, 0, 1));
		//}
	}

	void PowerSupply::OnCollisionEnter(std::shared_ptr<GameObject>& obj)
	{
		if (auto elec = GetComponent<Electrified>(false))
		{
			//リストに追加
			elec->OnElectrifiedEnter(obj);
		}
	}
	
	void PowerSupply::OnCollisionExit(std::shared_ptr<GameObject>& obj)
	{
		if (auto elec = GetComponent<Electrified>(false))
		{
			elec->OnElectrifiedExit(obj);
		}
	}
}
//end basecross

