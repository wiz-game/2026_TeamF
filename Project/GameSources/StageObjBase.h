#pragma once
#include "stdafx.h"

namespace basecross {

	class StageObjBase :public GameObject
	{
	public:
		Vec3 m_scale;
		Vec3 m_rot;
		Vec3 m_pos;

		wstring m_type;
		UINT m_id;

		inline static UINT TOTAL_ID = 0;
	protected:
		StageObjBase
		(
			const shared_ptr<Stage>& StagePtr,
			const Vec3& Scale,
			const Vec3& Rot,
			const Vec3& Position,
			const wstring& type
		);

		virtual ~StageObjBase();

	};

}