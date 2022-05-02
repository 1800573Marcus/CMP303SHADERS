#pragma once

#include "DXF.h"

using namespace std;
using namespace DirectX;


class TextureDepthShader : public BaseShader
{

public:

	TextureDepthShader(ID3D11Device* device, HWND hwnd);
	~TextureDepthShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& world, const XMMATRIX& view, const XMMATRIX& projection);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);

private:
	ID3D11Buffer* matrixBuffer;
};
