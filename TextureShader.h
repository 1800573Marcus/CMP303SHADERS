#pragma once

#include "DXF.h"
using namespace std;
using namespace DirectX;

class TextureShader : public BaseShader
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
	struct LightBufferType2
	{
		XMFLOAT3 lightPosition;
		float padding1;

	};
	struct LightBufferAttType2
	{
		XMFLOAT3 lightPosition[6];
		float padding1[6];

	};
public:
	TextureShader(ID3D11Device* device, HWND hwnd);
	~TextureShader();
	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, Light* light, Light* lightatt[6], ID3D11ShaderResourceView* depthMapatt[6]);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11SamplerState* sampleStateShadow;
	ID3D11Buffer* lightBuffer;
	ID3D11Buffer* lightBuffer2;
	ID3D11Buffer* lightBufferatt;
	ID3D11Buffer* lightBufferatt2;

};

