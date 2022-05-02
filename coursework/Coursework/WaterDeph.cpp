#include "WaterDeph.h"

WaterDeph::WaterDeph(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
    initShader(L"WaterDephVS.cso", L"tessellation_hs.cso", L"DepthTeselationDS.cso",L"DephPS.cso");
}


WaterDeph::~WaterDeph()
{
    // Release the sampler state.
    if (sampleState)
    {
        sampleState->Release();
        sampleState = 0;
    }

    // Release the matrix constant buffer.
    if (matrixBuffer)
    {
        matrixBuffer->Release();
        matrixBuffer = 0;
    }

    // Release the layout.
    if (layout)
    {
        layout->Release();
        layout = 0;
    }

    
  

    //Release base shader components
    BaseShader::~BaseShader();
}
void WaterDeph::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
    // InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
    initShader(vsFilename, psFilename);

    // Load other required shaders.
    loadHullShader(hsFilename);
    loadDomainShader(dsFilename);
}

void WaterDeph::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    D3D11_BUFFER_DESC timeBufferDesc;

    // Load (+ compile) shader files
    loadVertexShader(vsFilename);
    loadPixelShader(psFilename);

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;
    renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
    // Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    renderer->CreateSamplerState(&samplerDesc, &sampleState);


    //create buffer for time
    timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    timeBufferDesc.ByteWidth = sizeof(timerbufferType);
    timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    timeBufferDesc.MiscFlags = 0;
    timeBufferDesc.StructureByteStride = 0;
    renderer->CreateBuffer(&timeBufferDesc, NULL, &timerbuffer);

    //create buffer for tessellation factor
    D3D11_BUFFER_DESC tessBufferDesc;
    tessBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    tessBufferDesc.ByteWidth = sizeof(TessBufferType);
    tessBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    tessBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    tessBufferDesc.MiscFlags = 0;
    tessBufferDesc.StructureByteStride = 0;

    renderer->CreateBuffer(&tessBufferDesc, NULL, &TessBuffer);
}
void WaterDeph::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* Bumptexture, float time, XMFLOAT4 tess)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    XMMATRIX tworld, tview, tproj;


    // Transpose the matrices to prepare them for the shader.
    tworld = XMMatrixTranspose(worldMatrix);
    tview = XMMatrixTranspose(viewMatrix);
    tproj = XMMatrixTranspose(projectionMatrix);
    result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    dataPtr = (MatrixBufferType*)mappedResource.pData;
    dataPtr->world = tworld;// worldMatrix;
    dataPtr->view = tview;
    dataPtr->projection = tproj;
    deviceContext->Unmap(matrixBuffer, 0);
    deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);


    // Set shader texture resource in the pixel shader.
    TessBufferType* Tessptr;
    deviceContext->Map(TessBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    Tessptr = (TessBufferType*)mappedResource.pData;
    Tessptr->tessfactor.x = tess.x;
    Tessptr->tessfactor.y = tess.y;
    Tessptr->tessfactor.z = tess.z;
    Tessptr->tessfactor.w = tess.w;

    deviceContext->Unmap(TessBuffer, 0);
    deviceContext->HSSetConstantBuffers(0, 1, &TessBuffer);


    //send time buffer to Domain shader
    timerbufferType* timeptr;
    deviceContext->Map(timerbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    timeptr = (timerbufferType*)mappedResource.pData;
    timeptr->elapsedtime = time;
    timeptr->padding.x = 0;
    timeptr->padding.y = 0;
    timeptr->padding.z = 0;
    deviceContext->Unmap(timerbuffer, 0);
    deviceContext->DSSetConstantBuffers(1, 1, &timerbuffer);

    // Set shader texture resource in the pixel shader.
    deviceContext->DSSetShaderResources(1, 1, &Bumptexture);
    deviceContext->DSSetSamplers(0, 1, &sampleState);
}