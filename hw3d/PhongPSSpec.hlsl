#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"

#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float specularPowerConst;
    bool hasGloss;
    float specularMapWeight;
};

Texture2D tex;
Texture2D spec;

SamplerState splr;

cbuffer CameraCBuf:register(b3)
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
    // normalize the mesh normal
    worldnormal = normalize(worldnormal);
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(lightPos, worldPos);
    // specular parameters
    float specularPower = specularPowerConst;
    const float4 specularSample = spec.Sample(splr, tc);
    const float3 specularReflectionColor = specularSample.rgb * specularMapWeight;
    if (hasGloss)
    {
        specularPower = pow(2.0f, specularSample.a * 13.0f);
    }
    // attenuation
    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);
    // diffuse light
    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, worldnormal);
    // specular reflected
    const float3 specularReflected = Speculate(
        specularReflectionColor, 1.0f, worldnormal,
        lv.vToL, worldPos, CameraWPos, att, specularPower);
    // final color = attenuate diffuse & ambient by diffuse texture color and add specular reflected
    return float4(saturate((diffuse + ambient) * tex.Sample(splr, tc).rgb + specularReflected), 1.0f);
}