#include "../../Libs/BaseLib/DxShaders/INCStructs.hlsli"
#include "../../Libs/BaseLib/DxShaders/INCParameters.hlsli"

Texture2D g_texture : register(t0);
Texture2D g_inkTexture : register(t1);
SamplerState g_sampler : register(s0);

struct PSPNTInkInput
{
    float4 position : SV_POSITION;
    float3 norm : NORMAL;
    float4 specular : COLOR;
    float2 tex : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
};

float4 CalcInkColor(float4 defaultColor, PSPNTInkInput input)
{
    float2 uv = input.tex;
    float3 normal = normalize(input.norm);
    float4 color = g_inkTexture.Sample(g_sampler, uv);
    
    if (color.a > 0 && dot(normal, float3(0, 1, 0)) > 0.99f)
    {
        //float3 light = normalize(LightDir.xyz);
        //float3 cameraVec = EyePos.xyz - input.worldPosition;
        //float H = normalize(cameraVec + light);
        //float specular = pow(saturate(dot(normal, H)), 10.0f);
        
        //color += specular;
        return color;
    }
    
    return defaultColor;
}

float4 main(PSPNTInkInput input) : SV_TARGET
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
    color = CalcInkColor(color, input) * Light;
    
    return color;
}