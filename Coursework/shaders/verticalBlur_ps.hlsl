Texture2D shaderTexture : register(t0);
Texture2D depthmap : register(t1);

SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenHeight;
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
	weight0 = 0.382928f;
    weight1 = 0.241732;
    weight2 = 0.060598;
    weight3 = 0.241732;
    weight4 = 0.060598;
    // Initialize the colour to black.
    colour = float4(0.0f, 0.0f, 0.0f, 0.0f);

    float texelSize = 1.0f / screenHeight;


        float blurfactor;
        float2 centre;
        centre.x = 0.5;
        centre.y = 0.5;
        float centreblur = depthmap.Sample(SampleType, centre);//sample centre value
        blurfactor = depthmap.Sample(SampleType, input.tex);//sample depth value being rendered
    
            
                if (centreblur < blurfactor) {//if the vertex being rendered is further away than the centre of the screen blur it
                    blurfactor * 500;//increase blurriness

                    // Add the vertical pixels to the colour by the specific weight of each.
                    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -4.0f) * blurfactor) * weight4;//multiply by blur factor increase bluriness based on distance
                    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -3.0f) * blurfactor) * weight3;
                    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -2.0f) * blurfactor) * weight2;
                    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * -1.0f) * blurfactor) * weight1;
                    colour += shaderTexture.Sample(SampleType, input.tex) * weight0;
                    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 1.0f) * blurfactor) * weight1;
                    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 2.0f) * blurfactor) * weight2;
                    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 3.0f) * blurfactor) * weight3;
                    colour += shaderTexture.Sample(SampleType, input.tex + float2(0.0f, texelSize * 4.0f) * blurfactor) * weight4;

                    colour = colour / colour.w;
                    // Set the alpha channel to one.
                    colour.a = 1.0f;
                }
                else {
                    colour = shaderTexture.Sample(SampleType, input.tex);//render unblurred scene
                    colour.a = 1.0f;
                }
          
            

   


        return colour;
    }