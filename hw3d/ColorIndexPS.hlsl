cbuffer CBuf
{
	float4 face_colors[8];
};

float4 main(float4 pos : SV_Position,uint tid: SV_PrimitiveID ) : SV_Target
{
	return face_colors[(tid / 2)%6];
}