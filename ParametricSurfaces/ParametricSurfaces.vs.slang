struct ModelSettings
{
    float4x4 transform;
    float4x4 transformIT;

    Texture2D perlinNoise;
    SamplerState noiseSampler;
    bool hasPerlinNoise;
    float noiseIntensity;
    float texelWidth;
};

cbuffer VSCBuffer
{
    float4x4 viewProjection;
    ModelSettings settings[32];
}

struct VSOut
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    uint modelIndex : MODELINDEX;
};

struct VSIn
{
    float3 objSpacePos : POSOBJ;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    uint modelIndex : MODELINDEX;
};

float4 apply_perlin_noise(ModelSettings settings, float3 pos, float3 normal, float2 uv);

float3 calculate_normal_after_perlin(ModelSettings settings, float3 pos, float3 normal, float2 uv)
{
    const float delta = settings.texelWidth;
        
    float4 center = apply_perlin_noise(settings, pos, normal, uv);

    float3 p1Pos = float3(pos.x + 0.001, pos.y, pos.z);
    float2 p1Uv = float2(uv.x + delta, uv.y);
    float4 p1 = apply_perlin_noise(settings, p1Pos, normal, p1Uv);

    float3 p2Pos = float3(pos.x, pos.y, pos.z + 0.001);
    float2 p2Uv = float2(uv.x, uv.y + delta);
    float4 p2 = apply_perlin_noise(settings, p2Pos, normal, p2Uv);
    
    float3 deltax = normalize(center.xyz - p1.xyz);
    float3 deltaz = normalize(center.xyz - p2.xyz);

    float3 newNormal = cross(deltax, deltaz);
    
    return normalize(newNormal);
}

float4 apply_perlin_noise(ModelSettings settings, float3 pos, float3 normal, float2 uv)
{
    float4 displacement = 0;
    if (settings.hasPerlinNoise)
        displacement = settings.perlinNoise.SampleLevel(settings.noiseSampler, uv, 0);
    
    return float4(pos + (normal * displacement.xyz * settings.noiseIntensity), 1);
}

VSOut main(in VSIn input)
{
    VSOut output;
    ModelSettings obj = settings[input.modelIndex];
    
    float4x4 mvp = mul(viewProjection, obj.transform);

    if (obj.hasPerlinNoise)
    {
        float4 displacedObjSpacePos = apply_perlin_noise(obj, input.objSpacePos, input.normal, input.texCoord);
        float3 normalAfterPerlin = calculate_normal_after_perlin(obj, input.objSpacePos, input.normal, input.texCoord);
        output.normal = mul(obj.transformIT, float4(normalAfterPerlin, 0)).xyz;
        output.pos = mul(mvp, displacedObjSpacePos);
    }
    else
    {
        output.pos = mul(mvp, float4(input.objSpacePos, 1));
        output.normal = mul(obj.transformIT, float4(input.normal, 0)).xyz;
    }
    
    output.texCoord = input.texCoord;
    output.modelIndex = input.modelIndex;
    return output;
}
