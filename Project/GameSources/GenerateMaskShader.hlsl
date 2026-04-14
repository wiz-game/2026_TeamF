cbuffer cb : register(b0)
{
    int width;
}
struct Output
{
    int mask;
    bool visited;
};
Texture2D inputTexture : register(t0);
RWStructuredBuffer<Output> outputBuffer : register(u0);

[numthreads(256, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    int x = DTid.x % width;
    int y = DTid.x / width;
    
    float alpha = inputTexture.Load(int3(x, y, 0)).a;
    if (alpha > 0)
    {
        outputBuffer[DTid.x].mask = 1;
    }
    else
    {
        outputBuffer[DTid.x].mask = 0;
    }
    outputBuffer[DTid.x].visited = false;
}