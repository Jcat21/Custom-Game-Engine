#include "Constants.hlsl"

struct VIn
{
    float3 position : POSITION0;
	float3 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

struct VOut
{
    float4 position : SV_POSITION;
	float4 worldPos : POSITION0;
	float4 normal : NORMAL0;
	float2 uv : TEXCOORD0;
};

VOut VS(VIn vIn)
{
    VOut output;

	// transform input position from model to world space
	output.worldPos = mul(float4(vIn.position, 1.0), c_modelToWorld);
	// transform position from world to projection space
	output.position = mul(output.worldPos, c_viewProj);

	// transform input normal from model to world space
	output.normal = mul(float4(vIn.normal, 0.0), c_modelToWorld);

	output.uv = vIn.uv;

    return output;
}

float4 PS(VOut pIn) : SV_TARGET
{
	float4 diffuse = DiffuseTexture.Sample(DefaultSampler, pIn.uv);
    return diffuse;
}
