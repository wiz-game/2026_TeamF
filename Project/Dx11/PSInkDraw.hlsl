//--------------------------------------------------------------------------------------
// File: PSPVTStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "../../Libs/BaseLib/DxShaders/INCStructs.hlsli"
#include "../../Libs/BaseLib/DxShaders/INCParameters.hlsli"

Texture2D g_texture : register(t0);
Texture2D g_texture2 : register(t1);
SamplerState g_sampler : register(s0);
cbuffer VectorBuffer : register(b1)
{
    float4 up;
}

float4 GetInkColor(PSPNTInput input)
{
    float4 color = g_texture2.Sample(g_sampler, input.tex);
    if(color.a > 0)
    {
        //上方向のみ
        if (dot(input.norm, up) > 0.99f)
        {
            //インクの色をそのまま返す
            return color;
        }
    }
    
    //条件に合わない場合は透明な色を返す
    return float4(0, 0, 0, 0);
}

float4 main(PSPNTInput input) : SV_TARGET
{
    float4 color;
	//法線ライティング
    float3 lightdir = normalize(LightDir.xyz);
    float3 N1 = normalize(input.norm);
    float4 Light = (saturate(dot(N1, -lightdir)) * Diffuse) + Emissive;
    Light += input.specular;
    Light.a = Diffuse.a;
    if (Activeflags.x)
    {
        color = g_texture.Sample(g_sampler, input.tex) * Light;
    }
    
    float4 inkColor = GetInkColor(input);
    //インクの色が透明でない場合は、インクの色を乗算して反映させる
    if(inkColor.a != 0)
    {
        color = inkColor * Light;
    }
    return color;
}
