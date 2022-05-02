// texture vertex shader
// Basic shader for rendering textured geometry
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;
	matrix lightViewMatrix2[6];
	matrix lightProjectionMatrix2[6];
};
cbuffer LightBuffer : register(b1)
{
	float3 lightPosition;
	float padding1;

};
cbuffer LightBuffer : register(b2)
{
	float3 lightPosition2[6];
	float padding2[6];


};
struct InputType
{
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct OutputType
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPos : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
	float4 lightViewPos2[6] : TEXCOORD3;
	float3 lightPos2[6] : TEXCOORD9;
	float4 worldPosition : TEXCOORD15;

};

OutputType main(InputType input)
{
	OutputType output;
	float4 worldposition;




	// Calculate the position of the vertex against the world, view, and projection matrices.
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	// Store the texture coordinatesand normals  for the pixel shader.

	output.tex = input.tex;

	output.normal = mul(input.normal, (float3x3)worldMatrix);
	output.normal = normalize(output.normal);


	// Calculate the position of the light against the world, view, and projection matrices.


	output.lightViewPos = mul(input.position, worldMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
	output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);



	for (int i = 0; i < 6; i++) {//loop through point light data


		output.lightViewPos2[i] = mul(input.position, worldMatrix);
		output.lightViewPos2[i] = mul(output.lightViewPos2[i], lightViewMatrix2[i]);
		output.lightViewPos2[i] = mul(output.lightViewPos2[i], lightProjectionMatrix2[i]);

	}
	// Calculate the position of the vertex in the world.
	worldposition = mul(input.position, worldMatrix);




	
	output.worldPosition = worldposition;


	return output;
}