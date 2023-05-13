struct ModelSettings
{
    float3 ambient;
    float3 diffuse;
    float3 specular;

    float4x4 transform;

    Texture2D tex;

    Texture2D perlinNoise;
    SamplerState noiseSampler;
    bool hasPerlinNoise;
    float noiseIntensity;
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

//float3 calculate_normal_after_perlin(ModelSettings settings, float3 pos, float3 normal, float2 uv)
//{
//    const double delta = 0.0005;

//    float4 center = apply_perlin_noise(settings, pos, normal, uv);

//    pos = float3(pos.x + delta, pos.y, pos.z);
//    uv = float2(uv.x + delta, uv.y);
//    float4 p1 = apply_perlin_noise(settings, pos, normal, uv);
    
//    pos = float3(pos.x - delta, pos.y, pos.z + delta);
//    uv = float2(uv.x - delta, uv.y + delta);
//    float4 p2 = apply_perlin_noise(settings, pos, normal, uv);
    
//    float3 deltax = normalize(p1.xyz - center.xyz);
//    float3 deltaz = normalize(p2.xyz - center.xyz);

//    return normalize(cross(deltax, deltaz));
//}

float4 apply_perlin_noise(ModelSettings settings, float3 pos, float3 normal, float2 uv)
{
    float4 displacement = 0;
    if (settings.hasPerlinNoise)
        displacement = settings.perlinNoise.SampleLevel(settings.noiseSampler, uv, 0);
    
    return float4(pos + (normal * displacement.xyz * settings.noiseIntensity), 1);
}

VSOut main(in VSIn input)
{
    const float delta = 0.0001f;
    VSOut output;
    ModelSettings obj = settings[input.modelIndex];
    
    float4x4 mvp = mul(viewProjection, obj.transform);

    float4 displacedObjSpacePos = apply_perlin_noise(obj, input.objSpacePos, input.normal, input.texCoord);
    displacedObjSpacePos.y -= obj.noiseIntensity * 0.5f;

    //if (obj.hasPerlinNoise)
    //    output.normal = mul(obj.transform, float4(calculate_normal_after_perlin(obj, input.objSpacePos, input.normal, input.texCoord), 1)).xyz;
    //else
    output.normal = mul(obj.transform, float4(input.normal, 1)).xyz;
    
    output.pos = mul(mvp, displacedObjSpacePos);
    output.texCoord = input.texCoord;
    output.modelIndex = input.modelIndex;
    return output;
}
