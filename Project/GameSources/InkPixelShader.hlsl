struct PSInput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};

cbuffer Brush : register(b0)
{
    float4 brushCenters[4];
    float brushSize;
    int count;
}
float4 main(PSInput input) : SV_TARGET
{
    
    float dist = distance(input.uv, brushCenters[0].xy);
    if (dist <= brushSize)
    {
        if (brushCenters[0].z > 0.0f)
        {
            return float4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    discard;
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}