cbuffer LightCBuf
{
	float3 lightPos;
	float3 ambient;
	float3 diffuseColor;
	float diffuseIntensity;
	float attConst;
	float attLin;
	float attQuad;
};

cbuffer ObjectCBuf
{
    float specularIntensity;
    float specularPower;
	float padding[2];
};

Texture2D tex;
SamplerState splr;

cbuffer CameraCBuf
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
    //Normalize normal
    worldnormal = normalize(worldnormal);
    viewnormal = normalize(viewnormal);

    // fragment to light vector data
	const float3 vToL = lightPos - worldPos;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;
    
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
	return float4(saturate(diffuse + ambient ) * tex.Sample(splr, tc).rgb + specular, 1.0f);
}