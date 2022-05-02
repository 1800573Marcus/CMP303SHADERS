Texture2D shaderTexture : register(t0);
Texture2D depthmap : register(t1);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenWidth;
    float centrex;
    float centrey;
    float padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float weight0, weight1, weight2, weight3, weight4;
    float4 colour;



	// Create the weights that each neighbor pixel will contribute to the blur.
    weight0 = 0.382928;
    weight1 = 0.241732;
    weight2 = 0.060598;
    weight3 = 0.005977;
    weight4 = 0.000229;

	// Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

   
        float texelSize = 1.0f / screenWidth;
        float blurfactor;
        float2 centre;//set centre coords of screen
        centre.x = 0.5;
        centre.y = 0.5;
        float centreblur = depthmap.Sample(SampleType,centre);//sample depth value at centre of screen
        blurfactor = depthmap.Sample(SampleType, input.tex);//sample depth value of vertex
       
            
                if (centreblur < blurfactor) {//if the vertex being rendered is further away than the vertex at the centre blur
                    blurfactor * 500;//increase blurriness
                // Add the horizontal pixels to the colour by the specific weight of each.
                colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -4.0f, 0.0f) * blurfactor) * weight4; //multiply by blur factor so objects blur more based on distance
                colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -3.0f, 0.0f) * blurfactor) * weight3;
                colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -2.0f, 0.0f) * blurfactor) * weight2;
                colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * -1.0f, 0.0f * blurfactor)) * weight1;
                colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
                colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 1.0f, 0.0f) * blurfactor) * weight1;
                colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 2.0f, 0.0f) * blurfactor) * weight2;
                colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 3.0f, 0.0f) * blurfactor) * weight3;
                colour += shaderTexture.Sample(SampleType, input.tex + float2(texelSize * 4.0f, 0.0f) * blurfactor) * weight4;
                // Set the alpha channel to one.
                colour = colour / colour.w;

                colour.a = 1.0f;
            }
                else {//render scene unblurred
                    colour = shaderTexture.Sample(SampleType, input.tex);
                }
     

    
   
    return colour;
}
