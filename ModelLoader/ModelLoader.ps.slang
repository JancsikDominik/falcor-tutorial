struct PSIn
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
};


cbuffer PSCBuffer
{
    // light properties: ambient, diffuse, specular
    float3 lightAmbient;
    float3 lightDiffuse;
    float3 lightSpecular;

    float3 lightDir;

    // material properties: ambient, diffuse, specular
    float3 materialAmbient;
    float3 materialDiffuse;
    float3 materialSpecular;

    float3 cameraPosition;
};


float4 main(PSIn input) : SV_TARGET
{
    float3 ambient = lightAmbient * materialAmbient;

    // just to make sure its normalized
    float3 normal = normalize(input.normal);
    float3 toLight = -lightDir;
    float di = clamp(dot(toLight, normal), 0.0f, 1.0f);
    float3 diffuse = lightDiffuse * materialDiffuse * di;

    float3 e = normalize(cameraPosition - input.pos.xyz);
    float3 r = reflect(-toLight, normal);
    float si = pow(clamp(dot(e, r), 0.0f, 1.0f), 20);
    float3 specular = lightSpecular * materialSpecular * si;
    
    return float4(ambient + diffuse + specular, 1);
}
