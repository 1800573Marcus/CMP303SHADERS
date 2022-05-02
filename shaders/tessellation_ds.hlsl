// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
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
cbuffer TimerBuffer : register(b1)
{
    float time;
    float3 buffer;
};
cbuffer LightBuffer : register(b2)
{
    float3 lightPosition;
    float padding1;

};
cbuffer LightBuffer2 : register(b3)
{
    float3 lightPosition2[6];
    float padding2[6];

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
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
    float4 lightViewPos2[6]: TEXCOORD3;
    float3 lightPos2[6] : TEXCOORD9;
    float4 worldPosition : TEXCOORD15;
};

[domain("quad")]
OutputType main(ConstantOutputType input, float2 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{
    float3 vertexPosition;
    float2 texCoord;
    OutputType output;
    float4 worldposition;

 
    // Determine the position of the new vertex and tex coords
    //interpolate between the points of the patches to finms the new point or texture coord
    float2 t1 = lerp(patch[0].tex, patch[1].tex, uvwCoord.y);
    float2 t2 = lerp(patch[3].tex, patch[2].tex, uvwCoord.y);
    texCoord = lerp(t1, t2, uvwCoord.x);
    // Store the texture coordinates for the pixel shader.
    output.tex = texCoord;


    float3  v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3  v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    vertexPosition = lerp(v1, v2, uvwCoord.x);

    vertexPosition.y = 5 * texture1.SampleLevel(sampler0, texCoord + float2 (time.x / 50, time.x / 50), 0);//sample height map for y position

    //Calculate normals
    float3 a = (0, 0, 0);
    float3 b = (0, 0, 0);
    float3 c = (0, 0, 0);
    float3 d = (0, 0, 0);
    //create 2 vecctors acroos the point being rendered
    a.x += 0.04f;
    a.y = texture1.SampleLevel(sampler0, texCoord + float2 (time.x / 50, time.x / 50) + float2(0.02, 0), 0).r;//time must be factored in because of the movement of the waves

    b.x -= 0.04;
    b.y = texture1.SampleLevel(sampler0, texCoord + float2 (time.x / 50, time.x / 50) - float2(0.02, 0), 0).r;

    c.z += 0.04;
    c.y = texture1.SampleLevel(sampler0, texCoord + float2 (time.x / 50, time.x / 50) + float2(0, 0.02), 0).r;

    d.z -= 0.04;
    d.y = texture1.SampleLevel(sampler0, texCoord + float2 (time.x / 50, time.x / 50) - float2(0, 0.02), 0).r;

    float3 AB = normalize(a - b);
    float3 CD = normalize(d - c);
    //normalized cross product gets us the normal
    float3 vertexnormal = normalize(cross(AB, CD));

    // Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(vertexnormal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);




    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Calculate the position of the new lights against the world, view, and projection matrices.

    output.lightViewPos = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);

    for (int i = 0; i < 6; i++) {

        output.lightViewPos2[i] = mul(float4(vertexPosition, 1.0f), worldMatrix);
        output.lightViewPos2[i] = mul(output.lightViewPos2[i], lightViewMatrix2[i]);
        output.lightViewPos2[i] = mul(output.lightViewPos2[i], lightProjectionMatrix2[i]);

    }
    
    // Calculate the position of the vertex in the world.
    worldposition = mul(float4(vertexPosition, 1.0f), worldMatrix);

    // Determine the light position based on the position of the light and the position of the vertex in the world.
    output.lightPos = lightPosition.xyz - worldposition.xyz;

    // Normalize the light position vector.
    output.lightPos = normalize(output.lightPos);


 

    return output;
}

