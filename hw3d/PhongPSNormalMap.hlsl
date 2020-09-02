#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[2];
};

#include "transform.hlsl"

Texture2D tex;
Texture2D nmap : register(t2);
SamplerState splr;

cbuffer CameraCBuf: register(b3)
{
    float3 CameraWPos;
};

float4 main(float4 pos : SV_Position,
    float3 worldPos : WPosition,
    float3 worldnormal : WNormal,
    float3 viewPos : VPosition,
    float3 viewnormal : VNormal,
    float2 tc : Texcoord,
    float3 worldtangent : WTangent,
    float3 worldbitangent : WBiTangent) : SV_Target
{
    if (normalMapEnabled)
    {
        const float3x3 TransformToWorld = float3x3(
            normalize(worldtangent),
            normalize(worldbitangent),
            normalize(worldnormal));

        const float3 SampledNormal = nmap.Sample(splr, tc).xyz * 2.0f - 1.0f;

        worldnormal = NormalMapWorldSpace(worldtangent, worldbitangent, worldnormal, tc, nmap, splr);
    }

    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(lightPos, worldPos);
    
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);

    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, worldnormal);

    const float3 specular = Speculate(specularIntensity.rrr, 1.0f, worldnormal, lv.dirToL, worldPos, CameraWPos, att, specularPower);

    /*
    // diffuse attenuation
    const float att = 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
    //relected light vector
    const float3 w = worldnormal * dot(dirToL, worldnormal);
    const float3 r = w * 2.0f - dirToL;
    // cal the specular 
    const float3 specular = att * (diffuseColor * diffuseIntensity) * specularIntensity * pow(max(0.0f, dot(normalize(r), normalize(CameraWPos - worldPos))), specularPower);
    // diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, worldnormal));
    // final color
    //return float4(saturate(specular) * materialColor, 1.0f);
    //return float4(specular, 1.0f);
    */
    return float4(saturate(diffuse + ambient) * tex.Sample(splr, tc).rgb + specular, 1.0f);
}