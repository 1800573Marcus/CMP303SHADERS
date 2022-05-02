#include "Watershader.h"

Watershader::Watershader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
    initShader(L"WatershaderVS.cso",L"tessellation_hs.cso", L"tessellation_ds.cso", L"ShadowPS.cso");
}


Watershader::~Watershader()
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
void Watershader::initShader(const wchar_t* vsFilename, const wchar_t* hsFilename, const wchar_t* dsFilename, const wchar_t* psFilename)
{
    // InitShader must be overwritten and it will load both vertex and pixel shaders + setup buffers
    initShader(vsFilename, psFilename);

    // Load other required shaders.
    loadHullShader(hsFilename);
    loadDomainShader(dsFilename);
}


void Watershader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC lightBufferDesc;
    D3D11_BUFFER_DESC timeBufferDesc;
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

    //create shadow sampler
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

    //set up buffer for time
    timeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    timeBufferDesc.ByteWidth = sizeof(timerbufferType);
    timeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    timeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    timeBufferDesc.MiscFlags = 0;
    timeBufferDesc.StructureByteStride = 0;
    renderer->CreateBuffer(&timeBufferDesc, NULL, &timerbuffer);

    //set up buffer for the second set of light data

    lightBufferDesc2.Usage = D3D11_USAGE_DYNAMIC;
    lightBufferDesc2.ByteWidth = sizeof(LightBufferType2);
    lightBufferDesc2.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    lightBufferDesc2.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    lightBufferDesc2.MiscFlags = 0;
    lightBufferDesc2.StructureByteStride = 0;
    renderer->CreateBuffer(&lightBufferDesc2, NULL, &lightBuffer2);

    //set up buffers for point lights
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
void Watershader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX& worldMatrix, const XMMATRIX& viewMatrix, const XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture, ID3D11ShaderResourceView* texture2, ID3D11ShaderResourceView* depthMap, Light* light, Light* lightatt[6], ID3D11ShaderResourceView* depthMapatt[6], float time, XMFLOAT4 tess)
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
    for (int i = 0; i < 6; i++) {//loop through array of point lights

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

    for (int i = 0; i < 6; i++) {//loop through array of point lights for matrix buffer

        dataPtr->lightViewatt[i] = tLightAttViewMatrix[i];
        dataPtr->lightProjectionatt[i] = tLightAttProjectionMatrix[i];

    }

    deviceContext->Unmap(matrixBuffer, 0);
    deviceContext->DSSetConstantBuffers(0, 1, &matrixBuffer);// send matrix buffer data to domain shader

    deviceContext->Map(lightBufferatt, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    lightattptr = (LightAttBufferType*)mappedResource.pData;

    for (int i = 0; i < 6; i++) {//loop through point light data

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
    deviceContext->PSSetConstantBuffers(1, 1, &lightBufferatt);//send point light buffer 1 to Pixel shader

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
    deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);// send directional light buffer 1 to the pixel shader

    LightBufferType2* lightPtr2;//set up secondary data for directional light
    deviceContext->Map(lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    lightPtr2 = (LightBufferType2*)mappedResource.pData;
    lightPtr2->lightPosition = light->getPosition();
    lightPtr2->padding1 = 0;
    deviceContext->Unmap(lightBuffer2, 0);
    deviceContext->DSSetConstantBuffers(2, 1, &lightBuffer2);//send secondary directional light data to Domain shader

    LightBufferAttType2* lightAttPtr2;
    deviceContext->Map(lightBufferatt2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    for (int i = 0; i < 6; i++) {

        lightAttPtr2 = (LightBufferAttType2*)mappedResource.pData;
        lightAttPtr2->lightPosition[i] = lightatt[i]->getPosition();
        lightAttPtr2->padding1[i] = 0;
    }
    deviceContext->Unmap(lightBufferatt2, 0);
    deviceContext->DSSetConstantBuffers(3, 1, &lightBufferatt2); // send secondary point light data to Domain shader


    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &texture);
    deviceContext->PSSetSamplers(0, 1, &sampleState);//send sampler to pixel shader

    timerbufferType* timeptr;
    deviceContext->Map(timerbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    timeptr = (timerbufferType*)mappedResource.pData;
    timeptr->elapsedtime = time;
    timeptr->padding.x = 0;
    timeptr->padding.y = 0;
    timeptr->padding.z = 0;
    deviceContext->Unmap(timerbuffer, 0);
    deviceContext->DSSetConstantBuffers(1, 1, &timerbuffer);///send time data to domain shader

    TessBufferType* Tessptr;
    deviceContext->Map(TessBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    Tessptr = (TessBufferType*)mappedResource.pData;
    Tessptr->tessfactor.x = tess.x;
    Tessptr->tessfactor.y = tess.y;
    Tessptr->tessfactor.z = tess.z;
    Tessptr->tessfactor.w = tess.w;

    deviceContext->Unmap(TessBuffer, 0);
    deviceContext->HSSetConstantBuffers(0, 1, &TessBuffer);//send tessellation factor to Hull shader

    deviceContext->DSSetShaderResources(1, 1, &texture2);//send heightmap to Domain shader
    deviceContext->PSSetShaderResources(1, 1, &depthMap);//send light depth maps to Pixel shader
    deviceContext->PSSetShaderResources(2, 6, depthMapatt);   
    deviceContext->DSSetSamplers(0, 1, &sampleState);//send samplers to Pixe and Domain shaders
    deviceContext->PSSetSamplers(1, 1, &sampleStateShadow);
}
