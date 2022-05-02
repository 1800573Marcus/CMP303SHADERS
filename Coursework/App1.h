// Application.h
#ifndef _APP1_H
#define _APP1_H

// Includes
#include "DXF.h"
#include "ManipulationShader.h"
#include "Watershader.h"
#include "TextureShader.h"
#include "ManipulationDepth.h"
#include "WaterDeph.h"
#include "TextureDepth.h"
#include "VerticalBlurShader.h"
#include "HorizontalBlurShader.h"
#include "TextureShaderOrtho.h"
#include "TessPlaneMesh.h"

class App1 : public BaseApplication
{
public:

	App1();
	~App1();
	void init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN);

	bool frame();

protected:
	bool render();
	void depthPass();
	void depthPassatt();
	void depthPassspot();
	void finalPass();
	void depthPassblur();
	void verticalBlur();
	void horizontalBlur();
	void upsample();
	void finalblurpass();
	void gui();
	
private:
	Timer time;
	float times;
	AModel* model[6];
	WCHAR* modeltex[6];
	
	ManipulationShader* shader;
	ManipulationDepth* depthshader;
	Watershader* watershader;
	WaterDeph* waterdeph;
	TextureShader* texshader;
	TextureDepthShader* texdepth;
	TextureShaderOrtho* orthoshader;

	PlaneMesh* mesh;
	TessPlaneMesh* Tessmesh;
	Light* light;
	Light* lightatt[7];
	Light* lightspot;
	ShadowMap* shadowMap;
	ShadowMap* shadowMapatt[7];
	ShadowMap* shadowMapspot;
	ShadowMap* shadowMapblur;
	OrthoMesh* orthoMesh;
	OrthoMesh* DownSampleOrtho;
	RenderTexture* renderTexture;
	RenderTexture* horizontalBlurTexture;
	RenderTexture* verticalBlurTexture;
	RenderTexture* downsampleTexture;
	RenderTexture* upSampleTexture;
	VerticalBlurShader* verticalBlurShader;
	HorizontalBlurShader* horizontalBlurShader;
	XMFLOAT2 centrecoords;
	XMFLOAT4 TessellationFactor = XMFLOAT4(5, 5, 5, 5);

	
};

#endif