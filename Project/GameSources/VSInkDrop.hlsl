
struct VS_Input
{
    float3 position : SV_Position;
    float2 uv : TEXCOORD0;
};

struct VS_Output
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};
//ピクセルシェーダ―にUV座標を渡すための頂点シェーダー
VS_Output main( VS_Input input )
{   
    VS_Output output;
    output.position = float4(input.position, 1.0f);
    output.uv = input.uv;
    return output;
}
