cbuffer cb : register(b0)
{
    int width;
    int height;
}
struct Output
{
    int mask;
    bool visited;
};
Texture2D inputTexture : register(t0);
RWStructuredBuffer<int> outputBuffer : register(u0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if(DTid.x >= width || DTid.y >= height) return;
    
    int x = DTid.x;
    int y = DTid.y;
    
    int index = y * width + x;
    float alpha = inputTexture.Load(int3(x, y, 0)).a;
    if (alpha > 0)
    {
        outputBuffer[index] = index;
    }
    else
    {
        outputBuffer[index] = -1;
    }
}