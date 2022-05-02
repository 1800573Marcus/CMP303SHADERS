// Lab1.cpp
// Lab 1 example, simple coloured triangle mesh
#include "App1.h"

App1::App1()
{
	mesh = nullptr;
	shader = nullptr;

}

void App1::init(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight, Input* in, bool VSYNC, bool FULL_SCREEN)
{
	// Call super/parent init function (required!)
	BaseApplication::init(hinstance, hwnd, screenWidth, screenHeight, in, VSYNC, FULL_SCREEN);
	//TexturesCom_DesertSand1_3x3_1K_albedo.tif
		// Initalise scene variables.
	centrecoords.x = (screenWidth);
	centrecoords.y = (screenHeight);
	textureMgr->loadTexture(L"sand", L"res/TexturesCom_DesertSand1_3x3_1K_albedo.tif");//load in texture and model assets
	textureMgr->loadTexture(L"sandhieght", L"res/heightmap.png");
	textureMgr->loadTexture(L"water", L"res/water005.jpg");
	textureMgr->loadTexture(L"waterhieght", L"res/water 0342normal2.jpg");
	model[0] = new AModel(renderer->getDevice(), "res/Wall.obj");
	textureMgr->loadTexture(L"wall", L"res/wall.png");
	modeltex[0] = L"wall";
	model[1] = new AModel(renderer->getDevice(), "res/Roof.obj");
	textureMgr->loadTexture(L"roof", L"res/roof.png");
	modeltex[1] = L"roof";
	model[2] = new AModel(renderer->getDevice(), "res/Door.obj");
	textureMgr->loadTexture(L"door", L"res/door.png");
	modeltex[2] = L"door";
	model[3] = new AModel(renderer->getDevice(), "res/Glass.obj");
	textureMgr->loadTexture(L"glass", L"res/glass.png");
	modeltex[3] = L"glass";
	model[4] = new AModel(renderer->getDevice(), "res/Wood.obj");
	textureMgr->loadTexture(L"wood", L"res/wood.png");
	modeltex[4] = L"wood";
	model[5] = new AModel(renderer->getDevice(), "res/Metal_without_Rooster.obj");
	textureMgr->loadTexture(L"metal", L"res/metal.png");
	modeltex[5] = L"metal";

	mesh = new PlaneMesh(renderer->getDevice(), renderer->getDeviceContext());//set up meshes for scene
	Tessmesh = new TessPlaneMesh(renderer->getDevice(), renderer->getDeviceContext());

	shader = new ManipulationShader(renderer->getDevice(), hwnd);//shader initialisiation
	watershader = new Watershader(renderer->getDevice(), hwnd);
	texshader = new TextureShader(renderer->getDevice(), hwnd);
	waterdeph = new WaterDeph(renderer->getDevice(), hwnd);
	texdepth = new TextureDepthShader(renderer->getDevice(), hwnd);
	depthshader = new ManipulationDepth(renderer->getDevice(), hwnd);
	orthoshader = new TextureShaderOrtho(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);


	// Variables for defining shadow map
	int shadowmapWidth = 1024;
	int shadowmapHeight = 1024;
	int sceneWidth = 100;
	int sceneHeight = 100;

	// shadow maps for light and depth map for blur stages
	shadowMap = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
	shadowMapblur = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);

	// Confirgure directional light
	light = new Light();
	light->setAmbientColour(0.01f, 0.01f, 0.01f, 1.0f);
	light->setDiffuseColour(1.0, 1.0, 1.0, 1.0f);
	light->setDirection(0.0f, -0.7f, 0.7f);
	light->setPosition(50.f, 50.f, 0.f);
	light->generateOrthoMatrix((float)sceneWidth, (float)sceneHeight, 0.1f, 100.f);

	//configure point light
	for (int i = 0; i < 6; i++) {

		shadowMapatt[i] = new ShadowMap(renderer->getDevice(), shadowmapWidth, shadowmapHeight);
		lightatt[i] = new Light();
		lightatt[i]->setAmbientColour(0.0f, 0.0f, 0.0f, 1.0f);
		lightatt[i]->setDiffuseColour(0.5f, 0.5f, 0.0f, 1.0f);
		lightatt[i]->setPosition(51.5, 11.5, 46.5);
	}
	lightatt[0]->setDirection(0, -1, 0);
	lightatt[1]->setDirection(0, 1, 0);
	lightatt[2]->setDirection(1, 0, 0);
	lightatt[3]->setDirection(-1, 0, 0);
	lightatt[4]->setDirection(0, 0, -1);
	lightatt[5]->setDirection(0, 0, 1);

	for (int i = 0; i < 6; i++) {

		lightatt[i]->generateViewMatrix();
		lightatt[i]->generateProjectionMatrix(1, 100);
	}



	//set up variable for post processing
	orthoMesh = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth, screenHeight);	// Full screen size
	DownSampleOrtho = new OrthoMesh(renderer->getDevice(), renderer->getDeviceContext(), screenWidth/2, screenHeight/2);	// half screen size
    
	horizontalBlurShader = new HorizontalBlurShader(renderer->getDevice(), hwnd);
	verticalBlurShader = new VerticalBlurShader(renderer->getDevice(), hwnd);

	renderTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	horizontalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	verticalBlurTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	downsampleTexture = new RenderTexture(renderer->getDevice(), screenWidth / 2, screenHeight / 2, SCREEN_NEAR, SCREEN_DEPTH);
	upSampleTexture = new RenderTexture(renderer->getDevice(), screenWidth, screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	
}

App1::~App1()
{
	// Run base application deconstructor
	BaseApplication::~BaseApplication();

	// Release the Direct3D object.
	if (mesh)
	{
		delete mesh;
		mesh = 0;
	}

	if (shader)
	{
		delete shader;
		shader = 0;
	}
}


bool App1::frame()
{
	bool result;

	result = BaseApplication::frame();
	if (!result)
	{
		return false;
	}
	
	// Render the graphics.
	result = render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool App1::render()
{

	// Perform depth of direction light
	depthPass();
	//depth pass for point light
	depthPassatt();
	//depth pass for blur stages
	depthPassblur();
	//render the scene
	finalPass();

 //blur passes
	horizontalBlur();
	verticalBlur();
	 upsample();
//render blurred scene to orthomesh
	 finalblurpass();

	// Render GUI
	 	gui();
	

	
	// Present the rendered scene to the screen.
	renderer->endScene();

	return true;
}
void App1::depthPass() {
	// Set the render target to be the render to texture.
	shadowMap->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the light and d3d objects.
	light->generateViewMatrix();
	XMMATRIX lightViewMatrix = light->getViewMatrix();
	XMMATRIX lightProjectionMatrix = light->getOrthoMatrix(); //orthomatri used to light full scene
	XMMATRIX worldMatrix = renderer->getWorldMatrix();

	times += timer->getTime();//total elapsed time calculated

	mesh->sendData(renderer->getDeviceContext());//send mesh to depth shader
	depthshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"sandhieght"));//render island geometry for depth
	depthshader->render(renderer->getDeviceContext(), mesh->getIndexCount());


	worldMatrix = XMMatrixTranslation(0.f, 7.0f, 0.f);//traslate world poeotion to render water higher that the groundd mesh
	Tessmesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);//Set up mesh for Tessellation
	waterdeph->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"waterhieght"), times, TessellationFactor);//render water for depth pass
	waterdeph->render(renderer->getDeviceContext(), Tessmesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();//reset world matric for new translation
	worldMatrix = XMMatrixTranslation(50, 10, 50);//move world matrx so the house appears on the island

	for (int i = 0; i < 6; i++) {//loop through each model that is part of the house
		model[i]->sendData(renderer->getDeviceContext());//end current model to depth shader
		texdepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);//render house for depth pass
		texdepth->render(renderer->getDeviceContext(), model[i]->getIndexCount());
	}
	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}
void App1::depthPassatt() {
	for (int i = 0; i < 6; i++) {//loop for each light that is part of the point light

		// Set the render target to be the render to depth map.
		shadowMapatt[i]->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

		// get the world, view, and projection matrices from the light and d3d objects.
		XMMATRIX lightViewMatrix = lightatt[i]->getViewMatrix();
		XMMATRIX lightProjectionMatrix = lightatt[i]->getProjectionMatrix();
		XMMATRIX worldMatrix = renderer->getWorldMatrix();

		times += timer->getTime();//get elapsed time based on delta time

		mesh->sendData(renderer->getDeviceContext());//render island mesh for depth pass
		depthshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"sandhieght"));
		depthshader->render(renderer->getDeviceContext(), mesh->getIndexCount());


		worldMatrix = XMMatrixTranslation(0.f, 7.0f, 0.f);//tramslate to correct location
		Tessmesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);//set up mesh for tessellation
		waterdeph->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix, textureMgr->getTexture(L"waterhieght"), times, TessellationFactor);
		waterdeph->render(renderer->getDeviceContext(), Tessmesh->getIndexCount());

		worldMatrix = renderer->getWorldMatrix();//reset for nbew translation
		worldMatrix = XMMatrixTranslation(50, 10, 50);//translate to higher position for water

		for (int j = 0; i < 6; i++) {//;oop through each model that is part of the house
			model[j]->sendData(renderer->getDeviceContext());
			texdepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, lightViewMatrix, lightProjectionMatrix);//render house for depth pass
			texdepth->render(renderer->getDeviceContext(), model[j]->getIndexCount());
		}
		// Set back buffer as render target and reset view port.
		renderer->setBackBufferRenderTarget();
		renderer->resetViewport();
	}
}

void App1::finalPass() {


	//render scene to downsample texture
	downsampleTexture->setRenderTarget(renderer->getDeviceContext());
	downsampleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);

	ID3D11ShaderResourceView* shadowmapsatt[6];//set up shadow maps for shadrers

	for (int i = 0; i < 6; i++) {
		shadowmapsatt[i] = shadowMapatt[i]->getDepthMapSRV();
	}

	camera->update();//update camera
	

	// Get the world, view, projection, and ortho matrices from the camera and Direct3D objects.
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX viewMatrix = camera->getViewMatrix();
	XMMATRIX projectionMatrix = renderer->getProjectionMatrix();

	times += timer->getTime(); //get elapsed time

	mesh->sendData(renderer->getDeviceContext());//render island with lighting and shadows
	shader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"sand"), textureMgr->getTexture(L"sandhieght"), shadowMap->getDepthMapSRV(), light,lightatt, shadowmapsatt);
	shader->render(renderer->getDeviceContext(), mesh->getIndexCount());


	worldMatrix = XMMatrixTranslation(0.f, 7.0f, 0.f);//translate to correct location
	Tessmesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);//set up water mesh for tesselation
	//render water with lighting and shadows
	watershader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(L"water"), textureMgr->getTexture(L"waterhieght"), shadowMap->getDepthMapSRV(), light, lightatt, shadowmapsatt, times, TessellationFactor);
	watershader->render(renderer->getDeviceContext(), Tessmesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();//reset translation for new one
	worldMatrix = XMMatrixTranslation(50, 10, 50);//set house position

	for (int i = 0; i < 6; i++) {//loop through each model that is part of the house
		model[i]->sendData(renderer->getDeviceContext());
		//render house with lighting and shadows
		texshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->getTexture(modeltex[i]), shadowMap->getDepthMapSRV(),light, lightatt, shadowmapsatt);
		texshader->render(renderer->getDeviceContext(), model[i]->getIndexCount());
	}
	renderer->setBackBufferRenderTarget();
}
void App1::depthPassblur() {
	// Set the render target to be the render to texture.
	shadowMapblur->BindDsvAndSetNullRenderTarget(renderer->getDeviceContext());

	// get the world, view, and projection matrices from the camera and d3d objects.
	
	XMMATRIX camViewMatrix = camera->getViewMatrix();
	XMMATRIX camProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90), 1.0f, 0.1, 100);//view matrix created as camera has no perspective projection matrix 
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	
	

	times += timer->getTime();//get elapsed time
	
	mesh->sendData(renderer->getDeviceContext());
	//render island for depth pass
	depthshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camViewMatrix, camProjectionMatrix, textureMgr->getTexture(L"sandhieght"));
	depthshader->render(renderer->getDeviceContext(), mesh->getIndexCount());


	worldMatrix = XMMatrixTranslation(0.f, 7.0f, 0.f);//move water
	Tessmesh->sendData(renderer->getDeviceContext(), D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);//set up water for tessellation
	//render water for depth pass
	waterdeph->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camViewMatrix, camProjectionMatrix, textureMgr->getTexture(L"waterhieght"), times, TessellationFactor);
	waterdeph->render(renderer->getDeviceContext(), Tessmesh->getIndexCount());

	worldMatrix = renderer->getWorldMatrix();//reset for new translation
	worldMatrix = XMMatrixTranslation(50, 10, 50);//set house position

	for (int i = 0; i < 6; i++) {//loop through models part of house
		model[i]->sendData(renderer->getDeviceContext());
		texdepth->setShaderParameters(renderer->getDeviceContext(), worldMatrix, camViewMatrix, camProjectionMatrix);
		texdepth->render(renderer->getDeviceContext(), model[i]->getIndexCount());
	}
	// Set back buffer as render target and reset view port.
	renderer->setBackBufferRenderTarget();
	renderer->resetViewport();
}

void App1::horizontalBlur() {

	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	//set up horizontal blur
	float screenSizeX = (float)horizontalBlurTexture->getTextureWidth();//get dmesnions needed for blur
	horizontalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	horizontalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 1.0f, 1.0f, 0.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	orthoMatrix = horizontalBlurTexture->getOrthoMatrix();

	// Render for Horizontal Blur
	renderer->setZBuffer(false);
	DownSampleOrtho->sendData(renderer->getDeviceContext());
	//render blurred version of th scene
	horizontalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, downsampleTexture->getShaderResourceView(), shadowMapblur->getDepthMapSRV(), centrecoords);
	horizontalBlurShader->render(renderer->getDeviceContext(), DownSampleOrtho->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();

}
void App1::verticalBlur()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;

	float screenSizeY = (float)verticalBlurTexture->getTextureHeight();//get dmesnions needed for blur
	verticalBlurTexture->setRenderTarget(renderer->getDeviceContext());
	verticalBlurTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 1.0f, 1.0f, 1.0f);

	worldMatrix = renderer->getWorldMatrix();
	baseViewMatrix = camera->getOrthoViewMatrix();
	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	orthoMatrix = verticalBlurTexture->getOrthoMatrix();

	// Render for Vertical Blur
	renderer->setZBuffer(false);
	DownSampleOrtho->sendData(renderer->getDeviceContext());
	verticalBlurShader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, baseViewMatrix, orthoMatrix, horizontalBlurTexture->getShaderResourceView(), screenSizeY, shadowMapblur->getDepthMapSRV(), centrecoords);
	verticalBlurShader->render(renderer->getDeviceContext(), DownSampleOrtho->getIndexCount());
	renderer->setZBuffer(true);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	renderer->setBackBufferRenderTarget();
}
void App1::upsample() {
	// Set the render target to be the render to texture and clear it  
	upSampleTexture->setRenderTarget(renderer->getDeviceContext());
	upSampleTexture->clearRenderTarget(renderer->getDeviceContext(), 0.0f, 0.0f, 1.0f, 1.0f);
	// Get matrices    
	camera->update();
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();    // Default camera position for orthographic rendering
	// Render shape with simple lighting shader set.
	orthoMesh->sendData(renderer->getDeviceContext());
	//
	orthoshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, verticalBlurTexture->getShaderResourceView());

	orthoshader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	// Reset the render target back to the original back buffer and not the render to texture anymore.  
	renderer->setBackBufferRenderTarget();
}

void App1::finalblurpass()
{
	// Clear the scene. (default blue colour)
	renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

	// RENDER THE RENDER TEXTURE SCENE
	// Requires 2D rendering and an ortho mesh.
	renderer->setZBuffer(false);
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();  // ortho matrix for 2D rendering
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix();	// Default camera position for orthographic rendering

	orthoMesh->sendData(renderer->getDeviceContext());
	orthoshader->setShaderParameters(renderer->getDeviceContext(), worldMatrix, orthoViewMatrix, orthoMatrix, upSampleTexture->getShaderResourceView());
	orthoshader->render(renderer->getDeviceContext(), orthoMesh->getIndexCount());
	renderer->setZBuffer(true);






}
void App1::gui()
{
	// Force turn off unnecessary shader stages.
	renderer->getDeviceContext()->GSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->HSSetShader(NULL, NULL, 0);
	renderer->getDeviceContext()->DSSetShader(NULL, NULL, 0);

	// Build UI
	ImGui::Text("FPS: %.2f", timer->getFPS());
	ImGui::Checkbox("Wireframe mode", &wireframeToggle);

	// Render UI
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

