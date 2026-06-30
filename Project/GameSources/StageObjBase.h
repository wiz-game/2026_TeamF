#pragma once
#include "stdafx.h"

namespace basecross {

	class StageObjBase :public GameObject
	{
	public:
		Vec3 m_scale;
		Vec3 m_rot;
		Vec3 m_pos;

	protected:
		StageObjBase
		(
			const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position
		);

		virtual ~StageObjBase();

	};

}