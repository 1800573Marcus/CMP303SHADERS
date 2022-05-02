// Tessellation Hull Shader
// Prepares control points for tessellation
cbuffer TesellationFactorBuffer : register(b0)
{
    float4 TessFactor;
    
};


struct InputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;

};


struct ConstantOutputType
{
    float edges[4] : SV_TessFactor;
    float inside[2] : SV_InsideTessFactor;
};

struct OutputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};




ConstantOutputType PatchConstantFunction(InputPatch<InputType, 4> inputPatch, uint patchId : SV_PrimitiveID)
{    
    ConstantOutputType output;
 
   float distance =  1;
   
    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] =  2/distance;
    output.edges[1] =  2/distance;
    output.edges[2] =  2/distance;
    output.edges[3] = 2 / distance;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside[0] = 2/ distance;
    output.inside[1] = 2 / distance;
    return output;
}


[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunction")]
OutputType main(InputPatch<InputType, 4> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    OutputType output;

    
    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

  

    // Set the input colour as the output colour.
    output.tex = patch[pointId].tex;
    output.normal = patch[pointId].normal;

    return output;
}