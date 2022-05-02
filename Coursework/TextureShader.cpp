#include "TextureShader.h"

TextureShader::TextureShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
    initShader(L"texture_vs.cso", L"ShadowPS.cso");
}


TextureShader::~TextureShader()
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

    // Release the light constant buffer.
    if (lightBuffer)
    {
        lightBuffer->Release();
        lightBuffer = 0;
    }

    //Release base shader components
    BaseShader::~BaseShader();
}

void TextureShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    D3D11_BUFFER_DESC lightBufferDesc2;
    D3D11_BUFFER_DESC lightBufferAttDesc;
    D3D11_BUFFER_DESC lightBufferAttDesc2;

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

    //set up shadow sammpler
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
    samplerDesc.BorderColor[0] = 1.0f;
    samplerDesc.BorderColor[1] = 1.0f;
    samplerDesc.BorderColor[2] = 1.0f;
    samplerDesc.BorderColor[3] = 1.0f;
    renderer->CreateSamplerState(&samplerDesc, &sampleStateShadow);

    // Setup light buffer
    // Setup the description of the light dynamic constant buffer that is in the pixel shader.
    // Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.
    lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc.ByteWidth = sizeof(LightBufferType);
    lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc.MiscFlags = 0;
    lightBufferDesc.StructureByteStride = 0;
    renderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);
    //buffer for extra lighht data
    lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
    lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc2.MiscFlags = 0;
    lightBufferDesc2.StructureByteStride = 0;
    renderer->CreateBuffer(&lightBufferDesc2, NULL, &lightBuffer2);

    lightBufferAttDesc.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferAttDesc.ByteWidth = sizeof(LightAttBufferType);
    lightBufferAttDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferAttDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferAttDesc.MiscFlags = 0;
    lightBufferAttDesc.StructureByteStride = 0;
    renderer->CreateBuffer(&lightBufferAttDesc, NULL, &lightBufferatt);

    lightBufferAttDesc2.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferAttDesc2.ByteWidth = sizeof(LightBufferAttType2);
    lightBufferAttDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferAttDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferAttDesc2.MiscFlags = 0;
    lightBufferAttDesc2.StructureByteStride = 0;
    renderer->CreateBuffer(&lightBufferAttDesc2, NULL, &lightBufferatt2);
}
void TextureShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* depthMap, Light* light, Light* lightatt[6], ID3D11ShaderResourceView* depthMapatt[6])
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    LightBufferType* lightPtr;
    LightAttBufferType* lightattptr;

    // Transpose the matrices to prepare them for the shader.
    XMMATRIX tworld = XMMatrixTranspose(worldMatrix);
    XMMATRIX tview = XMMatrixTranspose(viewMatrix);
    XMMATRIX tproj = XMMatrixTranspose(projectionMatrix);
    XMMATRIX tLightViewMatrix = XMMatrixTranspose(light->getViewMatrix());
    XMMATRIX tLightProjectionMatrix = XMMatrixTranspose(light->getOrthoMatrix());
    XMMATRIX tLightAttViewMatrix[6];
    XMMATRIX tLightAttProjectionMatrix[6];
    for (int i = 0; i < 6; i++) {//loop through pont light data

        tLightAttViewMatrix[i] = XMMatrixTranspose(lightatt[i]->getViewMatrix());
        tLightAttProjectionMatrix[i] = XMMatrixTranspose(lightatt[i]->getProjectionMatrix());

    }

    // Lock the constant buffer so it can be written to.
    deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    dataPtr = (MatrixBufferType*)mappedResource.pData;
    dataPtr->world = tworld;// worldMatrix;
    dataPtr->view = tview;
    dataPtr->projection = tproj;
    dataPtr->lightView = tLightViewMatrix;
    dataPtr->lightProjection = tLightProjectionMatrix;
    for (int i = 0; i < 6; i++) {

        dataPtr->lightViewatt[i] = tLightAttViewMatrix[i];
        dataPtr->lightProjectionatt[i] = tLightAttProjectionMatrix[i];

    }

    deviceContext->Unmap(matrixBuffer, 0);
    deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

    //Additional
    // Send light data to pixel shader
    //LightBufferType* lightPtr;
    deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    lightPtr = (LightBufferType*)mappedResource.pData;
    lightPtr->diffuse = light->getDiffuseColour();
    lightPtr->direction.x = light->getDirection().x;
    lightPtr->direction.y = light->getDirection().y;
    lightPtr->direction.z = light->getDirection().z;
    lightPtr->direction.w = 0;
    lightPtr->ambient.x = light->getAmbientColour().x;
    lightPtr->ambient.y = light->getAmbientColour().y;
    lightPtr->ambient.z = light->getAmbientColour().z;
    lightPtr->ambient.w = 0;

    deviceContext->Unmap(lightBuffer, 0);
    deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

    LightBufferType2* lightPtr2;
    deviceContext->Map(lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    lightPtr2 = (LightBufferType2*)mappedResource.pData;
    lightPtr2->lightPosition = light->getPosition();
    lightPtr2->padding1 = 0;
    deviceContext->Unmap(lightBuffer2, 0);
    deviceContext->VSSetConstantBuffers(1, 1, &lightBuffer2);
    // see manipulation.cpp for explaination
    deviceContext->Map(lightBufferatt, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    lightattptr = (LightAttBufferType*)mappedResource.pData;
    for (int i = 0; i < 6; i++) {

        lightattptr->diffuse[i] = lightatt[i]->getDiffuseColour();
        lightattptr->direction[i].x = lightatt[i]->getDirection().x;
        lightattptr->direction[i].y = lightatt[i]->getDirection().y;
        lightattptr->direction[i].z = lightatt[i]->getDirection().z;
        lightattptr->direction[i].w = 0;
        lightattptr->ambient[i].x = lightatt[i]->getAmbientColour().x;
        lightattptr->ambient[i].y = lightatt[i]->getAmbientColour().y;
        lightattptr->ambient[i].z = lightatt[i]->getAmbientColour().z;
        lightattptr->ambient[i].w = 0;

    }
    
    deviceContext->Unmap(lightBufferatt, 0);
    deviceContext->PSSetConstantBuffers(1, 1, &lightBufferatt);





    LightBufferAttType2* lightAttPtr2;
    deviceContext->Map(lightBufferatt2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    for (int i = 0; i < 6; i++) {

        lightAttPtr2 = (LightBufferAttType2*)mappedResource.pData;
        lightAttPtr2->lightPosition[i] = lightatt[i]->getPosition();
        lightAttPtr2->padding1[i] = 0;
    }
    deviceContext->Unmap(lightBufferatt2, 0);
    deviceContext->VSSetConstantBuffers(2, 1, &lightBufferatt2);


    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &texture);


    // Set shader texture resource in the pixel shader.
 
    deviceContext->PSSetShaderResources(1, 1, &depthMap);
    deviceContext->PSSetShaderResources(2, 6, depthMapatt);
    deviceContext->PSSetSamplers(0, 1, &sampleState);
    deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}






