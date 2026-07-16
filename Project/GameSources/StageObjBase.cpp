
#include "stdafx.h"
#include "Project.h"
#include "StageObjBase.h"

namespace basecross {
	StageObjBase::StageObjBase
	(
		const shared_ptr<Stage>& StagePtr,
		const Vec3& Scale, 
		const Vec3& Rot, 
		const Vec3& Position,
		const wstring& type
	) :
		GameObject(StagePtr),
		m_scale(Scale),
		m_rot(Rot),
		m_pos(Position),
		m_type(type)
	{
	}

	StageObjBase::~StageObjBase()
	{
		int checker = 0;
	}



}