#include "ShaderOps.hlsl"
#include "LightVectorData.hlsl"
#include "PointLight.hlsl"

cbuffer ObjectCBuf
{
    float4 materialcolor;
    float4 specularColor;
    float specularPower;
};

/*
cbuffer TransformBuffer
{
    matrix model;// model local to world
    matrix modelView; // model local to view local
    matrix modelViewProj; // model local to screen
};
*/


cbuffer CameraCBuf: register(b3)
{
    float3 CameraWPos;
};

float4 main(float4 pos : SV_Position,
    float3 worldPos : WPosition,
    float3 worldnormal : WNormal,
    float3 viewPos : VPosition,
    float3 viewnormal : VNormal) : SV_Target
{
    worldnormal = normalize(worldnormal);
    viewnormal = normalize(viewnormal);
    // fragment to light vector data
    const LightVectorData lv = CalculateLightVectorData(lightPos, worldPos);

    const float att = Attenuate(attConst, attLin, attQuad, lv.distToL);

    const float3 diffuse = Diffuse(diffuseColor, diffuseIntensity, att, lv.dirToL, worldnormal);

    const float3 specular = Speculate(specularColor.rgb, 1.0f, worldnormal, lv.dirToL, worldPos, CameraWPos, att, specularPower);

    return float4(saturate((diffuse + ambient) * materialcolor.rgb + specular), 1.0f);
}