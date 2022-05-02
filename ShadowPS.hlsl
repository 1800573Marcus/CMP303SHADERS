
Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
Texture2D depthMapTexture2[6] : register(t2);

SamplerState diffuseSampler  : register(s0);
SamplerState shadowSampler : register(s1);

cbuffer LightBuffer : register(b0)
{
    float4 diffuse;
    float4 direction;
    float4 ambient;
    
    

};

cbuffer LightBufferAtt : register(b1)
{
    float4 diffuseatt[6];
    float4 directionatt[6];
    float4 ambientatt[6];

};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float4 lightViewPos : TEXCOORD1;
    float3 lightPos : TEXCOORD2;
    float4 lightViewPos2[6] : TEXCOORD3;
    float3 lightPos2[6] : TEXCOORD9;
    float3 worldPosition : TEXCOORD15;

};

// Calculate lighting intensity based on direction and normal. Combine with light colour.
float4 calculateLighting(float3 lightDirection, float3 normal, float4 diffuse)
{
    float intensity = saturate(dot(normal, lightDirection));
    float4 colour = (0, 0, 0, 0);
    colour = colour + (diffuse * intensity);
    return colour;
}

// Is the gemoetry in our shadow map
bool hasDepthData(float2 uv)
{
    if (uv.x < 0.f || uv.x > 1.f || uv.y < 0.f || uv.y > 1.f)
    {
        return false;
    }
    return true;
}

bool isInShadow(Texture2D sMap, float2 uv, float4 lightViewPosition, float bias)
{
    // Sample the shadow map (get depth of geometry)
    float depthValue = sMap.Sample(shadowSampler, uv).r;
    // Calculate the depth from the light.
    float lightDepthValue = lightViewPosition.z / lightViewPosition.w;
    lightDepthValue -= bias;

    // Compare the depth of the shadow map value and the depth of the light to determine whether to shadow or to light this pixel.
    if (lightDepthValue < depthValue)
    {
        return false;
    }
    return true;
}

float2 getProjectiveCoords(float4 lightViewPosition)
{

    // Calculate the projected texture coordinates.
    float2 projTex = lightViewPosition.xy / lightViewPosition.w;
    projTex *= float2(0.5, -0.5);
    projTex += float2(0.5f, 0.5f);
    return projTex;
}

float4 main(InputType input) : SV_TARGET
{
        float3 lightVector = input.lightViewPos - input.worldPosition;

        float dist = length(lightVector);

        float constantf = 0;
        float linearf = 0;
        float quadraticf = 0.5;
        float pow = 1;
        float attenuation = 1 / (constantf + (linearf * dist) + (quadraticf * pow * (dist, 2)));//calculation for sttenuation


    float shadowMapBias = 0.005;
    float4 colour = float4(0.f, 0.f, 0.f, 1.f);
    float4 textureColour = shaderTexture.Sample(diffuseSampler, input.tex);
    float4 ambientcalc = ambient;
   

    // Calculate the projected texture coordinates.
    float2 pTexCoord = getProjectiveCoords(input.lightViewPos);

    // Shadow test. Is or isn't in shadow
    if (hasDepthData(pTexCoord))
    {
        // Has depth map data
        if (!isInShadow(depthMapTexture, pTexCoord, input.lightViewPos, shadowMapBias))
        {
            // is NOT in shadow, therefore light
            colour += calculateLighting(-direction, input.normal, diffuse);
        }
    }

    for (int i = 0; i < 6; i++) {


        lightVector = input.lightViewPos2[i] - input.worldPosition;

        dist = length(lightVector);

        pTexCoord = getProjectiveCoords(input.lightViewPos2[i]);

        // Shadow test. Is or isn't in shadow
        if (hasDepthData(pTexCoord))
        {
            // Has depth map data
            if (!isInShadow(depthMapTexture2[i], pTexCoord, input.lightViewPos2[i], shadowMapBias))
            {
                // is NOT in shadow, therefore light
                colour += calculateLighting(-directionatt[i], input.normal, diffuseatt[i]) * attenuation;
                //ambientcalc += ambientatt;
            }
        }



    }
     colour = saturate(colour + ambientcalc);
     return saturate(colour)* textureColour;
}