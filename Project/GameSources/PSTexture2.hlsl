//--------------------------------------------------------------------------------------
// File: PSPVTStatic.hlsl
//
//--------------------------------------------------------------------------------------

#include "../../Libs/BaseLib/DxShaders/INCStructs.hlsli"
#include "../../Libs/BaseLib/DxShaders/INCParameters.hlsli"

Texture2D g_texture : register(t0);
Texture2D g_texture2 : register(t2);
Texture2D g_texture3 : register(t3);
SamplerState g_sampler : register(s0);

cbuffer VectorBuffer : register(b1)
{
    float3 up;
}

float4 GetInkColor(PSPNTInput2 input)
{
    float4 color = g_texture3.Sample(g_sampler, input.tex);
    if (color.a > 0)
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


float4 main(PSPNTInput2 input) : SV_TARGET
{
    float4 color;
	//法線ライティング
    float3 lightdir = normalize(LightDir.xyz);
    float3 N1 = normalize(input.norm); //変換後の法線ベクトル
    float3 modelNorm = normalize(input.modelNorm); //変換前の法線ベクトル
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
            if (abs(dot(modelNorm, float3(1, 0, 0))) > 0.99f)
            {
                isFirstTexture = true;
            }
        }
        // Activeflags.y が 0.0 ＝ 従来のY軸上向き判定モード
        else
        {
            // Y軸上向きを向いているか判定
            if (dot(modelNorm, up) > 0.99f)
            {
                isFirstTexture = true;
            }
        }
    }
    
    //まとめてテクスチャをサンプリング
    if(isFirstTexture)
    {
        color = g_texture.Sample(g_sampler, input.tex) * Light;
    }
    else
    {
        color = g_texture2.Sample(g_sampler, input.tex) * Light;
    }
    
    float4 inkColor = GetInkColor(input);
    //インクの色が透明でない場合は、インクの色を乗算して反映させる
    if (inkColor.a != 0)
    {
        color = inkColor * Light;
    }

    return color;
}
