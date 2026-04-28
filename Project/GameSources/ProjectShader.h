/*!
@file ProjectShader.h
@brief プロジェクトで使用するシェーダー
*/

#pragma once
#include "stdafx.h"

namespace basecross{
	struct TextureSizeConstantData {
		int width;
		int height;
		int padding[2];
	};


	DECLARE_DX11_COMPUTE_SHADER(GenerateMaskShader)
	DECLARE_DX11_COMPUTE_SHADER(UnionFindFirst)
	DECLARE_DX11_COMPUTE_SHADER(UnionFindSecond)
	DECLARE_DX11_CONSTANT_BUFFER(TextureSizeConstantBuffer, TextureSizeConstantData)
}
//end basecross

