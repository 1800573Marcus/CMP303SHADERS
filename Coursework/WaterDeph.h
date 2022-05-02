
#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class WaterDeph : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};
	struct timerbufferType
	{
		float elapsedtime;
		XMFLOAT3 padding;

	};
	struct TessBufferType
	{
		XMFLOAT4 tessfactor;


	};

public:
	WaterDeph(ID3D11Device* device, HWND hwnd);
	~WaterDeph();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* Bumptexture, float time, XMFLOAT4 tess);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);


private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* timerbuffer;
	ID3D11Buffer* TessBuffer;

};
