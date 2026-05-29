//--------------------------------------------------------------------------------------
// File: PSPVTStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "../../Libs/BaseLib/DxShaders/INCStructs.hlsli"
#include "../../Libs/BaseLib/DxShaders/INCParameters.hlsli"

Texture2D g_texture : register(t0);
Texture2D g_texture2 : register(t2);
SamplerState g_sampler : register(s0);

//cbuffer VectorBuffer : register(b0)
//{
//    float3 up;
//    float3 front;
//    float3 right;
//}

float4 main(PSPNTInput input) : SV_TARGET
{
	//法線ライティング
    float3 lightdir = normalize(LightDir.xyz);
    float3 N1 = normalize(input.norm);
    float4 Light = (saturate(dot(N1, -lightdir)) * Diffuse) + Emissive;
    Light += input.specular;
    Light.a = Diffuse.a;
    
    //判定用フラグ
    bool isFirstTexture = false;
    
    if (Activeflags.x)
    {
        // Activeflags.y が 1.0 ＝ Z軸判定モード（ゴールの扉など）
        if (Activeflags.y > 0.5f)
        {
            // Z軸方向（前後両方）を向いているか判定
            if (abs(dot(N1, float3(0, 0, 1))) > 0.99f)
            {
                isFirstTexture = true;
            }
        }
        // Activeflags.y が 0.0 ＝ 従来のY軸上向き判定モード
        else
        {
            // Y軸上向きを向いているか判定
            if (dot(N1, float3(0, 1, 0)) > 0.99f)
            {
                isFirstTexture = true;
            }
        }
    }
    
    //まとめてテクスチャをサンプリング
    if(isFirstTexture)
    {
        Light = g_texture.Sample(g_sampler, input.tex) * Light;
    }
    else
    {
        Light = g_texture2.Sample(g_sampler, input.tex) * Light;
    }
    return Light;
}
