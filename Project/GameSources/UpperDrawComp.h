/*!
@file UpperDrawComp.h
@brief 上部にだけテクスチャを貼る
*/

#pragma once
#include "stdafx.h"

namespace basecross
{
	class UpperDrawComp : public PNTStaticDraw
	{
	public:
		UpperDrawComp(const shared_ptr<GameObject>& stage) :
			PNTStaticDraw(stage)
		{
		}

		virtual void OnDraw() override;
	};
	DECLARE_DX11_PIXEL_SHADER(UpperPixelSheder)
	DECLARE_DX11_PIXEL_SHADER(UpperShadowPixelSheder)

}
#pragma once
