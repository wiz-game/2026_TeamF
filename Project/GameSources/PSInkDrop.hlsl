struct VS_Output
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

cbuffer brushBuffer : register(b0)
{
    float4 brushCenters[4];//同時に描画できる数
    float brushSize; //ブラシのサイズ
    int count;//新しく描画されるブラシの数
};

float4 main(VS_Output input) : SV_TARGET
{
    float3 center = brushCenters[0].xyz;
    float dist = length(input.uv - center.xy);
    
    if(dist < brushSize)
    {
        return float4(0, 0, 0, 1); //ブラシの内側は黒で描画
    }
    discard; //ブラシの外側は描画しない
}