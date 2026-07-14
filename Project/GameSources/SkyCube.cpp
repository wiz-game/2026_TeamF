/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	void SkyCube::OnCreate() {
		auto draw = AddComponent<PTStaticDraw>();
		draw->SetMeshResource(L"DEFAULT_PT_CUBE");
		draw->SetTextureResource(m_TexKey);
		draw->SetRasterizerState(RasterizerState::CullFront);
		draw->SetDepthStencilState(DepthStencilState::Read);

		auto transform = GetComponent<Transform>();
		transform->SetScale(Vec3(150));

		Vec3 center;
		int objCount = 0;
		auto stage = GetStage();
		for (auto& obj : stage->GetGameObjectVec()) {
			auto stageObj = dynamic_pointer_cast<StageObjBase>(obj);
			if (!stageObj) continue;

			Vec3 position = stageObj->GetComponent<Transform>()->GetPosition();
			center += position;
			objCount++;
		}

		transform->SetPosition(center / objCount);
	}
}
//end basecross
