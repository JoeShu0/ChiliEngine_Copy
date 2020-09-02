
float3 NormalMapWorldSpace(const float3 tanWS, const float3 bitanWS, const float3 normalWS, const float2 tc, Texture2D nmap, SamplerState splr)
{
    const float3x3 TransformToWorld = float3x3(
        tanWS,
        bitanWS,
        normalWS);

    const float3 TangentNormal = nmap.Sample(splr, tc).xyz * 2.0f - 1.0f;

    return normalize(mul(TangentNormal, TransformToWorld));
}

float Attenuate(uniform float attConst, uniform float attLin, uniform float attQuad, const in float distToL)
{
    return 1.0f / (attConst + attLin * distToL + attQuad * (distToL * distToL));
}

float3 Diffuse(uniform float3 diffColor, uniform float diffuseIntensity, const in float att, const in float3 DirToLWS, const in float3 normalWS)
{
    return diffColor * diffuseIntensity * att * max(0.0f, dot(DirToLWS, normalWS));
}

float3 Speculate(
    const in float3 specularColor,
    uniform float specularIntensity,
    const in float3 normalWS,
    const in float3 dirToLWS,
    const in float3 posWS,
    const in float3 camPosWS,
    const in float att,
    const in float specularPower)
{
    const float3 w = normalWS * dot(dirToLWS, normalWS);
    const float3 r = normalize(w * 2.0f - dirToLWS);

    const float3 DirToCam = normalize(camPosWS - posWS);

    return att * specularColor * specularIntensity * pow(max(0.0f, dot(r, DirToCam)), specularPower);
}