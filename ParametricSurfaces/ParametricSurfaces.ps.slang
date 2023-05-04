struct ModelSettings
{
    float3 ambient;
    float3 diffuse;
    float3 specular;

    float4x4 transform;

    bool hasTexture;
    Texture2D tex;
};

struct PSIn
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
    uint modelIndex : SV_RenderTargetArrayIndex;
};

cbuffer PSCBuffer
{
    // light properties: ambient, diffuse, specular
    float3 lightAmbient;
    float3 lightDiffuse;
    float3 lightSpecular;

    float3 lightDir;
    float3 cameraPosition;

    ModelSettings modelSettings[32];

    // textures
    Texture2D objTexture[32];
    SamplerState texSampler;
};

float4 main(PSIn input) : SV_TARGET
{
    float3 ambient = lightAmbient * modelSettings[input.modelIndex].ambient;

    // just to make sure its normalized
    float3 normal = normalize(input.normal);
    float3 nLightDir = normalize(lightDir);
    
    float3 toLight = -nLightDir;
    float di = clamp(dot(toLight, normal), 0.0f, 1.0f);
    float3 diffuse = lightDiffuse * modelSettings[input.modelIndex].diffuse * di;

    float3 e = normalize(cameraPosition - input.pos.xyz);
    float3 r = reflect(-toLight, normal);
    float si = pow(clamp(dot(e, r), 0.0f, 1.0f), 20);
    float3 specular = lightSpecular * modelSettings[input.modelIndex].specular * si;
    
    if (modelSettings[input.modelIndex].hasTexture)
        return float4(ambient + diffuse + specular, 1) * modelSettings[input.modelIndex].tex.Sample(texSampler, input.texCoord);
    else
        return float4(ambient + diffuse + specular, 1);

}
