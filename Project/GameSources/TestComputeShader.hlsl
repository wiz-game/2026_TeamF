StructuredBuffer<float> inputBuffer : register(t0);
RWStructuredBuffer<float> outputBuffer : register(u0);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float v = inputBuffer[DTid.x];

    for (int i = 0; i < 1024; i++)
    {
        uint idx = (DTid.x + i * 17) % 4000;
        float m = inputBuffer[idx];

        v += sin(m) * cos(v);
        v = sqrt(abs(v)) + 0.0001;
    }

    outputBuffer[DTid.x] = v;
}