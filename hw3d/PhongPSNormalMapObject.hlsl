#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
    bool normalMapEnabled;
    float padding[1];
};

#include "Transform.hlsl"

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
    float2 tc : Texcoord) : SV_Target
{
    // sample normal from map if normal mapping enabled
    if (normalMapEnabled)
    {
        // sample and unpack normal data
        const float3 normalSample = nmap.Sample(splr, tc).xyz;
        const float3 objectNormal = normalSample * 2.0f - 1.0f;
        // bring normal from object space into view space
        worldnormal = normalize(mul(objectNormal, (float3x3) model));
    }
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(lightPos, worldPos);
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse intensity
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, worldnormal);
    // specular
    const float3 specular = Speculate(
    specularIntensity.rrr, 1.0f, worldnormal, lv.vToL,
    worldPos, CameraWPos, att, specularPower
    );
    // final color
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specular), 1.0f);
}