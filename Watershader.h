#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;

class Watershader : public BaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 diffuse;
		XMFLOAT4 direction;
		XMFLOAT4 ambient;

	};
	struct LightAttBufferType
	{
		XMFLOAT4 diffuse[6];
		XMFLOAT4 direction[6];
		XMFLOAT4 ambient[6];
	};
	struct MatrixBufferType
	{
		XMMATRIX world;
		XMMATRIX view;
		XMMATRIX projection;
		XMMATRIX lightView;
		XMMATRIX lightProjection;
		XMMATRIX lightViewatt[6];
		XMMATRIX lightProjectionatt[6];
	};
	struct LightBufferAttType2
	{
		XMFLOAT3 lightPosition[6];
		float padding1[6];

	};

	struct timerbufferType
	{
		float elapsedtime;
		XMFLOAT3 padding;

	};
	struct LightBufferType2
	{
		XMFLOAT3 lightPosition;
		float padding1;

	};
	struct TessBufferType
	{
		XMFLOAT4 tessfactor;


	};
public:
	Watershader(ID3D11Device* device, HWND hwnd);
	~Watershader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2,ID3D11ShaderResourceView* depthMap, Light* light, Light* lightatt[6], ID3D11ShaderResourceView* depthMapatt[6], float time, XMFLOAT4 tess);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);
	void initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename);


private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* timerbuffer;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer2;
	ID3D11Buffer* lightBufferatt;
	ID3D11Buffer* lightBufferatt2;
	ID3D11Buffer* TessBuffer;
};
