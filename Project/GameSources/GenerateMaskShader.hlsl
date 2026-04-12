struct Output
{
    int mask;
    bool visited;
};
StructuredBuffer<float> inputBuffer : register(t0);
RWStructuredBuffer<Output> outputBuffer : register(u0);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (inputBuffer[DTid.x] > 0)
    {
        outputBuffer[DTid.x].mask = 1;
    }
    else
    {
        outputBuffer[DTid.x].mask = 0;
    }
    outputBuffer[DTid.x].visited = false;
}