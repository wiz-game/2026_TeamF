#include "../../Libs/BaseLib/DxShaders/INCStructs.hlsli"
#include "../../Libs/BaseLib/DxShaders/INCParameters.hlsli"

struct PSPNTInkInput
{
    float4 position : SV_POSITION;
    float3 norm : NORMAL;
    float4 specular : COLOR;
    float2 tex : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
};
PSPNTInkInput main(VSPNTInput input)
{
    PSPNTInkInput result;
	//頂点の位置を変換
    float4 pos = float4(input.position.xyz, 1.0f);
	//ワールド変換
    pos = mul(pos, World);
    result.worldPosition = pos.xyz;
	//ビュー変換
    pos = mul(pos, View);
	//射影変換
    pos = mul(pos, Projection);
	//ピクセルシェーダに渡す変数に設定
    result.position = pos;
	//ライティング
    result.norm = mul(input.norm, (float3x3) World);
    result.norm = normalize(result.norm);
	//スペキュラー
    float3 H = normalize(normalize(-LightDir.xyz) + normalize(EyePos.xyz - pos.xyz));
    result.specular = Specular * dot(result.norm, H);
	//テクスチャUV
    result.tex = input.tex;
    return result;
}
