// Light vertex shader
Texture2D texture1 : register(t1);
SamplerState sampler0 : register(s0);

// Standard issue vertex shader, apply matrices, pass info to pixel shader
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
cbuffer LightBuffer2 : register(b2)
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


    //use hieght map data for y values
    input.position.y = texture1.SampleLevel(sampler0, input.tex, 0) * 10;//set vertex Y pos based on sampler value from height map. multiplied by 10 to mak the island higher up from the sea floor

    //Calculate normals
    float3 a = (0, 0, 0);
    float3 b = (0, 0, 0);
    float3 c = (0, 0, 0);
    float3 d = (0, 0, 0);

    a.x += 0.04f;
    a.y = texture1.SampleLevel(sampler0, input.tex + float2(0.02, 0), 0).r;

    b.x -= 0.04;
    b.y = texture1.SampleLevel(sampler0, input.tex - float2(0.02, 0), 0).r;

    c.z += 0.04;
    c.y = texture1.SampleLevel(sampler0, input.tex + float2(0, 0.02), 0).r;

    d.z -= 0.04;
    d.y = texture1.SampleLevel(sampler0, input.tex - float2(0, 0.02), 0).r;

    //create 2 vectors across the vertex being rendered 
    float3 AB = normalize(a - b);
    float3 CD = normalize(d - c);
    //normalized cross product of these vectors gives us our normals
    float3 vertexnormal = normalize(cross(AB, CD));

    input.normal = vertexnormal;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.position = mul(input.position, worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;

    // Calculate the normal vector against the world matrix only and normalise.
    output.normal = mul(input.normal, (float3x3)worldMatrix);
    output.normal = normalize(output.normal);


    // Calculate the position of the vertice as viewed by the light source.
    output.lightViewPos = mul(input.position, worldMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightViewMatrix);
    output.lightViewPos = mul(output.lightViewPos, lightProjectionMatrix);
    
    for (int i = 0; i < 6; i++) {


        output.lightViewPos2[i] = mul(input.position, worldMatrix);
        output.lightViewPos2[i] = mul(output.lightViewPos2[i], lightViewMatrix2[i]);
        output.lightViewPos2[i] = mul(output.lightViewPos2[i], lightProjectionMatrix2[i]);

    }


    // Calculate the position of the vertex in the world.
    worldposition = mul(input.position, worldMatrix);

    // Determine the light position based on the position of the light and the position of the vertex in the world.
    output.lightPos = lightPosition.xyz - worldposition.xyz;


    // Normalize the light position vector.
    output.lightPos = normalize(output.lightPos);



    

    output.worldPosition = worldposition;

    return output;
}