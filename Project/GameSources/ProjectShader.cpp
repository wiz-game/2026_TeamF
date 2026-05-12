/*!
@file ProjectShader.cpp
@brief プロジェクトで使用するシェーダー実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross{

	IMPLEMENT_DX11_COMPUTE_SHADER(GenerateMaskShader, App::GetApp()->GetShadersPath() + L"GenerateMaskShader.cso")
	IMPLEMENT_DX11_COMPUTE_SHADER(UnionFindFirst, App::GetApp()->GetShadersPath() + L"UnionFindFirst.cso")
	IMPLEMENT_DX11_COMPUTE_SHADER(UnionFindSecond, App::GetApp()->GetShadersPath() + L"UnionFindSecond.cso")
	IMPLEMENT_DX11_CONSTANT_BUFFER(TextureSizeConstantBuffer)
}
//end basecross


