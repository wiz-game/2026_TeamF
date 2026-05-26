struct VSInput
{
    float2 position : SV_Position;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_Position;
    float2 uv : TEXCOORD;
};

PSInput main(VSInput input)
{
    PSInput output;
    output.position = float4(input.position, 0, 1);
    output.uv = input.uv;
	return output;
}