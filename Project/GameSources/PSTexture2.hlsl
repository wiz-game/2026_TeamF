//--------------------------------------------------------------------------------------
// File: PSPVTStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "../../Libs/BaseLib/DxShaders/INCStructs.hlsli"
#include "../../Libs/BaseLib/DxShaders/INCParameters.hlsli"

Texture2D g_texture : register(t0);
Texture2D g_texture2 : register(t2);
SamplerState g_sampler : register(s0);

float4 main(PSPNTInput input) : SV_TARGET
{
	//法線ライティング
    float3 lightdir = normalize(LightDir.xyz);
    float3 N1 = normalize(input.norm);
    float4 Light = (saturate(dot(N1, -lightdir)) * Diffuse) + Emissive;
    Light += input.specular;
    Light.a = Diffuse.a;
    if (Activeflags.x && dot(N1, float3(0, 1, 0)) > 0.99f)
    {
        Light = g_texture.Sample(g_sampler, input.tex) * Light;
    }
    else
    {
        Light = g_texture2.Sample(g_sampler, input.tex) * Light;
    }
    return Light;
}
