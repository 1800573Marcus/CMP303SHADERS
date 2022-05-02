// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};
cbuffer TimerBuffer : register(b1)
{
    float time;
    float3 buffer;
};
struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 depthPosition : TEXCOORD0;

};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    float2 texCoord;
    OutputType output;
//interpolate between patch points for new vertex position and texture coords
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    texCoord = lerp(t1, t2, uvwCoord.x);
    // Store the texture coordinates for the pixel shader.


    float3  v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3  v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);

    vertexPosition.y = 5 * texture1.SampleLevel(sampler0, texCoord + float2 (time.x / 50, time.x / 50), 0);//sample texture for vertex manipulation




    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.depthPosition = output.position;//set the depth position to the same as the vertex new position



    return output;
}

