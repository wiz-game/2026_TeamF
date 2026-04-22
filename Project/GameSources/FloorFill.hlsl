cbuffer cb : register(b0)
{
    int width;
    int height;
}

StructuredBuffer<int> inputBuffer : register(t0);
RWStructuredBuffer<int> outputBuffer : register(u0);
RWStructuredBuffer<int> isConverted : register(u1);

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
    
    int newLabel = inputBuffer[index];
    
    
    //上下左右のデータと比較して、変換前のデータより小さい値があれば変換後のデータを更新する
    if(x < width - 1 && inputBuffer[index + 1] != -1)
    {
        newLabel = min(newLabel, inputBuffer[index + 1]);
    }
    if (x > 0 && inputBuffer[index - 1] != -1)
    {
        newLabel = min(newLabel, inputBuffer[index - 1]);
    }
    if (y < height - 1 && inputBuffer[index + width] != -1)
    {
        newLabel = min(newLabel, inputBuffer[index + width]);
    }
    if (y > 0 && inputBuffer[index - width] != -1)
    {
        newLabel = min(newLabel, inputBuffer[index - width]);
    }
    
    //変換前と変換後のデータを比較して、変換があったかどうかを判別する
    if (newLabel != inputBuffer[index])
    {
        int dummy;
        InterlockedOr(isConverted[0], 1);
    }
    outputBuffer[index] = newLabel;
}