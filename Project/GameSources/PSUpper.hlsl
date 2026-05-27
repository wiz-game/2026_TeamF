//--------------------------------------------------------------------------------------
// File: PSPVTStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "../../Libs/BaseLib/DxShaders/INCStructs.hlsli"
#include "../../Libs/BaseLib/DxShaders/INCParameters.hlsli"

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

float4 main(PSPNTInput input) : SV_TARGET
{
	//法線ライティング
    float3 lightdir = normalize(LightDir.xyz);
    float3 N1 = normalize(input.norm);//法線
    float4 Light = (saturate(dot(N1, -lightdir)) * Diffuse) + Emissive;
    Light += input.specular;
    Light.a = Diffuse.a;
    if (Activeflags.x && dot(N1, float3(0, 1, 0)) > 0.99f)//法線と上方向のベクトルの差が0.99以上ならテクスチャを付ける
    {
        Light = g_texture.Sample(g_sampler, input.tex) * Light;
    }
    return Light;
}
