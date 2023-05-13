RWTexture2D<float4> result;
RWStructuredBuffer<float2> gradients;

cbuffer CSCBuffer
{
    float res;
    float seed;
}

float perlin(float x, float y);

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float4 v = float4(id.xyxy) / res;
    float h = 0.0;
    for (int i = 0; i < 6; i++)
    {
        h += (1.0 + noise(float4(id.xyxy) * pow(2.0, i) / res)) * pow(2.0, -(i + 2));
    }
    result[id.xy] = h;
}


float dropOff(float x)
{
    float v = 1 - abs(x);
    return 6 * pow(v, 5) - 15 * pow(v, 4) + 10 * pow(v, 3); // nice normals
}

float2 randomGradient(int ix, int iy) {
    // No precomputed gradients mean this works for any number of grid coordinates
    const uint w = 8 * 32;
    const uint s = w / 2; // rotation width
    uint a = ix, b = iy;
    a *= 3284157443; b ^= a << s | a >> w-s;
    b *= 1911520717; a ^= b << s | b >> w-s;
    a *= 2048419325;
    float random = a * (3.14159265 / ~(~0u >> 1)); // in [0, 2*Pi]
    float2 v;
    v.x = cos(random); v.y = sin(random);
    return v;
}

float noiseS(float2 g, float2 v)
{
    return dot(g, v.xy) * dropOff(v.x) * dropOff(v.y);
}

float noise(float4 v)
{
    v += float4(seed, 0.0, seed, 0.0);
    uint4 gi = floor(v.xyxy) + uint4(0, 0, 1, 1);
    v = frac(v.xyxy) - float4(0.0, 0.0, 1.0, 1.0);
    
    return noiseS(randomGradient(gi.x, gi.y), v.xy) + noiseS(randomGradient(gi.z, gi.y), v.zy) + noiseS(randomGradient(gi.x, gi.w), v.xw) + noiseS(randomGradient(gi.z, gi.w), v.zw);
}
