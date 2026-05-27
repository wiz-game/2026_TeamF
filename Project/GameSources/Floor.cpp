/*!
@file Player.cpp
@brief プレイヤーなど実体
*/

#include "stdafx.h"
#include "Project.h"
#include "InkDrawComponentTest.h"
namespace basecross
{
	Floor::Floor
	(
		const std::shared_ptr<Stage>& stage,
		const Vec3& Scale,
		const Vec3& rot,
		const Vec3& Pos
	) :
		StageObjBase(stage, Scale, rot, Pos),
		m_pos(Pos),
		m_scale(Scale)
	{
	}

	void Floor::OnCreate()
	{
		//Drawコンポーネント
		m_draw = AddComponent<PNTStaticDraw>(/*512,512*/);
		m_draw->SetMeshResource(L"DEFAULT_CUBE");
		m_draw->SetOwnShadowActive(true);

		auto shadowMap = AddComponent<Shadowmap>();
		shadowMap->SetMeshResource(L"DEFAULT_CUBE");

		//Transformコンポーネント
		m_trans = GetComponent<Transform>();

		//位置、スケール設定
		m_trans->SetPosition(m_pos);
		m_trans->SetScale(m_scale);
		m_trans->SetRotation(m_rot);


		auto coll = AddComponent<CollisionObb>();
		coll->SetFixed(true);
	}

	void Floor::OnUpdate()
	{

	}
}