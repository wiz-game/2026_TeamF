cbuffer cb : register(b0)
{
    int width;
    int height;
}

StructuredBuffer<int> inputBuffer : register(t0);
RWStructuredBuffer<int> outputBuffer : register(u0);

[numthreads(8, 8, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    if (DTid.x >= width || DTid.y >= height)
        return;

    int x = DTid.x;
    int y = DTid.y;
    int index = DTid.y * width + DTid.x;
    int maxIndex = width * height - 1;
    
    if (inputBuffer[index] == -1)
    {
        outputBuffer[index] = -1;
        return;
    }
    
    int newParent = inputBuffer[index];
    
    
    //上下左右のデータと比較して、変換前のデータより小さい値があれば変換後のデータを更新する
    if (x < width - 1 && inputBuffer[index + 1] != -1)
    {
        if (newParent > inputBuffer[index + 1])
        {
            newParent = inputBuffer[index + 1];
        }
    }
    if (x > 0 && inputBuffer[index - 1] != -1)
    {
        if (newParent > inputBuffer[index - 1])
        {
            newParent = inputBuffer[index - 1];
        }
    }
    if (y < height - 1 && inputBuffer[index + width] != -1)
    {
        if (newParent > inputBuffer[index + width])
        {
            newParent = inputBuffer[index + width];
        }
    }
    if (y > 0 && inputBuffer[index - width] != -1)
    {
        if (newParent > inputBuffer[index - width])
        {
            newParent = inputBuffer[index - width];
        }
    }
    
    outputBuffer[index] = newParent;

}