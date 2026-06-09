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
cbuffer TimeBuffer : register(b1)
{
    float time;
}


float random(float2 p)
{
    return frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453);
}

float noise(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);

    float a = random(i);
    float b = random(i + float2(1, 0));
    float c = random(i + float2(0, 1));
    float d = random(i + float2(1, 1));

    float2 u = f * f * (3.0 - 2.0 * f);

    return lerp(a, b, u.x) +
           (c - a) * u.y * (1.0 - u.x) +
           (d - b) * u.x * u.y;
}

float4 main(PSInput input) : SV_TARGET
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    for (int i = 0; i < count; i++)
    {
        float dist = distance(input.uv, brushCenters[i].xy);
        if (dist <= brushSize)
        {
            if (brushCenters[0].z > 0.0f)
            {
                return float4(0.0f, 0.0f, 0.0f, 0.0f);
            }
        
            float3 color = float3(0.0f, 0.0f, 0.0f);
            float alpha = 1.0f;
            
            float fadeStart = brushSize * 0.8f;
            float fadeEnd = brushSize;

            float fade = 1.0 - smoothstep(fadeStart, fadeEnd, dist);
            alpha *= fade;
            
            return float4(color, fade);
        }
    }
       
    discard;
    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}