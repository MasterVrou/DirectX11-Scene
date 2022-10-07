//
// Game.cpp
//

#include "pch.h"
#include "Game.h"
#include "cmath"

extern void ExitGame() noexcept;

using namespace DirectX;
using namespace SimpleMath;
using Microsoft::WRL::ComPtr;

Game::Game() noexcept(false)
{
    m_deviceResources = std::make_unique<DX::DeviceResources>();
    m_deviceResources->RegisterDeviceNotify(this);
}

// Initialize the Direct3D resources required to run.
void Game::Initialize(HWND window, int width, int height)
{
    m_input.Initialise(window);
    m_deviceResources->SetWindow(window, width, height);

    m_deviceResources->CreateDeviceResources();
    CreateDeviceDependentResources();

    m_deviceResources->CreateWindowSizeDependentResources();
    CreateWindowSizeDependentResources();

    m_mainCameraViewRect.left = 0;
    m_mainCameraViewRect.top = 0;
    m_mainCameraViewRect.right = 800;
    m_mainCameraViewRect.bottom = 600;

    m_reflectionCameraViewRect.left = 500;
    m_reflectionCameraViewRect.top = 0;
    m_reflectionCameraViewRect.right = 800;
    m_reflectionCameraViewRect.bottom = 240;

    //setup camera
    m_mainCamera.setPosition(Vector3(0.0f, 3.0f, 0.0f));
    m_mainCamera.setRotation(Vector3(-90.0f, 45.0f, 0.0f));

    m_reflectionCamera.setPosition(Vector3(-5.f, 6.f, -5.f));
    m_reflectionCamera.setRotation(Vector3(-180.0f, 0.0f, 90.0f));

    //setup light
    m_light.setAmbientColour(0.3f, 0.3f, 0.3f, 1.0f);
    m_light.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
    m_light.setPosition(5.0f, 10.0f, -5.0f);
    m_light.setDirection(1.f, -1.0f, 0.0f);

    m_light2.setAmbientColour(0.5f, 0.5f, 0.5f, 1.0f);
    m_light2.setDiffuseColour(1.0f, 1.0f, 1.0f, 1.0f);
    m_light2.setPosition(-5.5f, -5.5f, -7.f);
    m_light2.setDirection(0.f, -1.0f, 0.0f);
}

#pragma region Frame Update
// Executes the basic game loop.
void Game::Tick()
{
    //take in input
    m_input.Update();								//update the hardware
    m_gameInputCommands = m_input.getGameInput();	//retrieve the input for our game

    m_timer.Tick([&]()
    {
        Update(m_timer);
    });

    Render();
}

// Updates the world.
void Game::Update(DX::StepTimer const& timer)
{
    float deltaTime = timer.GetElapsedSeconds() + 3;
    //note that currently.  Delta-time is not considered in the game object movement. 
    if (m_gameInputCommands.left)
    {
        Vector3 rotation = m_mainCamera.getRotation();
        rotation.y = rotation.y += m_mainCamera.getRotationSpeed() / deltaTime;
        m_mainCamera.setRotation(rotation);
    }
    if (m_gameInputCommands.right)
    {
        Vector3 rotation = m_mainCamera.getRotation();
        rotation.y = rotation.y -= m_mainCamera.getRotationSpeed() / deltaTime;
        m_mainCamera.setRotation(rotation);
    }
    if (m_gameInputCommands.forward)
    {
        Vector3 position = m_mainCamera.getPosition(); //get the position
        position += (m_mainCamera.getForward() * m_mainCamera.getMoveSpeed() / deltaTime); //add the forward vector
        m_mainCamera.setPosition(position);
    }
    if (m_gameInputCommands.back)
    {
        Vector3 position = m_mainCamera.getPosition(); //get the position
        position -= (m_mainCamera.getForward() * m_mainCamera.getMoveSpeed() / deltaTime); //add the forward vector
        m_mainCamera.setPosition(position);
    }
    if (m_gameInputCommands.rotLeft)
    {
        Vector3 rotation = m_mainCamera.getRotation();
        rotation.y = rotation.y -= m_input.getDeltaX() / deltaTime;
        m_mainCamera.setRotation(rotation);
    }
    if (m_gameInputCommands.rotUp)
    {
        Vector3 rotation = m_mainCamera.getRotation();
        rotation.x = rotation.x += m_input.getDeltaY() / deltaTime;
        if (rotation.x < -180)
        {
            rotation.x = -179;
        }
        if (rotation.x > 0)
        {
            rotation.x = 1;
        }
        m_mainCamera.setRotation(rotation);
    }

    m_mainCamera.Update();
    m_mainCameraView = m_mainCamera.getCameraMatrix();

    m_reflectionCamera.Update();
    //m_reflectionCamera.setPosition(DirectX::SimpleMath::Vector3(-5.f, 3.f, -5.f));
    //m_reflectionCamera.setRotation(DirectX::SimpleMath::Vector3(m_mainCamera.getRotation().x + 180.0f, -m_mainCamera.getRotation().y + 180.0f, m_mainCamera.getRotation().z + 180.0f));
    m_reflectionCameraView = m_reflectionCamera.getCameraMatrix();

    auto time = static_cast<float>(timer.GetTotalSeconds());

    //m_world = Matrix::CreateRotationZ(cosf(time) * 2.f);
    //m_effect->SetFresnelFactor(cosf(time * 2.f));

    if (ball4Left)
    {
        ball4R -= 0.1f / deltaTime;

        if (ball4R < -3.5f)
        {
            ball4Left = false;
            ball4Right = true;
        }
    }
    if(ball4Right)
    {
        ball4R += 0.1f / deltaTime;

        if (ball4R > -2.f)
        {
            ball4Right = false;
            ball5Right = true;
        }
    }
    if (ball5Right)
    {
        ball5R += 0.1f / deltaTime;

        if (ball5R > -0.5f)
        {
            ball5Left = true;
            ball5Right = false;
        }
    }
    if (ball5Left)
    {
        ball5R -= 0.1 / deltaTime;

        if (ball5R < -2.f)
        {
            ball5Left = false;
            ball4Left = true;
        }
    }

    if (flap1Up)
    {
        flap1R -= 0.1 / deltaTime;

        if (flap1R < -1.5)
        {
            flap1Down = true;
            flap1Up = false;
        }
    }
        
    if (flap1Down)
    {

        flap1R += 0.1 / deltaTime;

        if (flap1R > -0.5)
        {
            flap1Down = false;
            flap1Up = true;
        }
    }

    if (flap2Up)
    {
        flap2R += 0.1 / deltaTime;

        if (flap2R > 1)
        {
            flap2Down = true;
            flap2Up = false;
        }
    }

    if (flap2Down)
    {
        flap2R -= 0.1 / deltaTime;

        if (flap2R < 0)
        {
            flap2Down = false;
            flap2Up = true;
        }
    }

    birdR += 0.01f / deltaTime;

    m_world = Matrix::Identity;
    m_batchEffect->SetView(m_mainCameraView);
    m_batchEffect->SetWorld(m_world);

    if (m_input.Quit())
    {
        ExitGame();
    }
}
#pragma endregion

#pragma region Frame Render
// Draws the scene.
void Game::Render()
{
    // Don't try to render anything before the first Update.
    if (m_timer.GetFrameCount() == 0)
    {
        return;
    }

    Clear();

    m_deviceResources->PIXBeginEvent(L"Render");
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();

    m_skyboxEffect->SetView(m_mainCameraView);
    m_sky->Draw(m_skyboxEffect.get(), m_batchInputLayout.Get());

    //Set Rendering states. 
    context->OMSetBlendState(m_states->Opaque(), nullptr, 0xFFFFFFFF);
    context->OMSetDepthStencilState(m_states->DepthDefault(), 0);
    context->RSSetState(m_states->CullClockwise());

    renderTexturePassSphere();

    //floor
    m_world = Matrix::Identity;
    Matrix floorPosition = Matrix::CreateTranslation(-5.f, 0.f, -5.f);//half the texture dissapears
    Matrix floorScale = Matrix::CreateScale(0.5);
    m_world = m_world * floorScale * floorPosition;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_floorTexture.Get());
    m_floorModel.Render(context);

    //floor2
    m_world = Matrix::Identity;
    Matrix floorPosition2 = Matrix::CreateTranslation(-5.f, -5.f, -5.f);//half the texture dissapears
    m_world = m_world * floorScale * floorPosition2;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_floorTexture.Get());
    m_floorModel.Render(context);

    //floor2Layer
    m_world = Matrix::Identity;
    Matrix floor2LayerPosition = Matrix::CreateTranslation(-5.f, -5.035f, -5.f);//half the texture dissapears
    m_world = m_world * floorScale * floor2LayerPosition;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light2, m_floorTexture.Get());
    m_floorLayerModel.Render(context);

    //floor3
    m_world = Matrix::Identity;
    Matrix floorPosition3 = Matrix::CreateTranslation(-5.f, -10.f, -5.f);//half the texture dissapears
    m_world = m_world * floorScale * floorPosition3;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light2, m_floorTexture.Get());
    m_floorModel.Render(context);

    //floor3Layer
    m_world = Matrix::Identity;
    Matrix floor2LayerPosition3 = Matrix::CreateTranslation(-5.f, -10.f, -5.f);//half the texture dissapears
    m_world = m_world * floorScale * floor2LayerPosition3;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_floorTexture.Get());
    m_floorLayerModel.Render(context);
    

    //wall
    m_world = Matrix::Identity;
    Matrix wallPosition = Matrix::CreateTranslation(-7.5f, -7.5f, -5.f);//half the texture dissapears
    m_world = m_world * floorScale * wallPosition;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_floorTexture.Get());
    m_wallModel.Render(context);

    //wallLayer
    m_world = Matrix::Identity;
    Matrix wallLayerPosition = Matrix::CreateTranslation(-7.47f, -7.5f, -5.f);//half the texture dissapears
    Matrix wallLayerRotation = Matrix::CreateRotationY(90 * 3.14f / 180.f);
    m_world = m_world * floorScale * wallLayerRotation * wallLayerPosition;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light2, m_floorTexture.Get());
    m_wallLayerModel.Render(context);

    //wall2
    m_world = Matrix::Identity;
    Matrix wallPosition2 = Matrix::CreateTranslation(-2.5f, -7.5f, -5.f);//half the texture dissapears
    m_world = m_world * floorScale * wallPosition2;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_floorTexture.Get());
    m_wallModel.Render(context);

    //wallLayer2
    m_world = Matrix::Identity;
    Matrix wallLayerPosition2 = Matrix::CreateTranslation(-2.55f, -7.5f, -5.f);//half the texture dissapears
    //Matrix wallLayerRotation = Matrix::CreateRotationY(90 * 3.14f / 180.f);
    m_world = m_world * floorScale * wallLayerRotation * wallLayerPosition2;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light2, m_floorTexture.Get());
    m_wallLayerModel.Render(context);

    //wall3
    m_world = Matrix::Identity;
    Matrix wallPosition3 = Matrix::CreateTranslation(-5.f, -7.5f, -7.5f);//half the texture dissapears
    Matrix wallRotation = Matrix::CreateRotationY(90 * 3.14f / 180.f);
    m_world = m_world * floorScale * wallRotation * wallPosition3;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_floorTexture.Get());
    m_wallModel.Render(context);

    //wallLayer3
    m_world = Matrix::Identity;
    Matrix wallLayerPosition3 = Matrix::CreateTranslation(-5.f, -7.5f, -7.47f);//half the texture dissapears
    //Matrix wallLayerRotation = Matrix::CreateRotationY(90 * 3.14f / 180.f);
    m_world = m_world * floorScale * wallLayerPosition3;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light2, m_floorTexture.Get());
    m_wallLayerModel.Render(context);

    //light
    m_world = Matrix::Identity;
    Matrix lightPosition = Matrix::CreateTranslation(-5.5f, -5.2f, -7.3f);
    Matrix lightScale = Matrix::CreateScale(0.2);
    Matrix lightRotation = Matrix::CreateRotationX(-45.f * 3.14f / 180.f);
    m_world = m_world * lightScale* lightRotation * lightPosition;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light2, m_lightTexture.Get());
    m_lightModel.Render(context);

    //chair
    m_world = Matrix::Identity;
    Matrix chairPosition = Matrix::CreateTranslation(-5.f, 0.f, -5.f);
    m_world = m_world * chairPosition;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_chairTexture.Get());
    m_chairModel.Render(context);
    
    
    //birdBody
    m_world = Matrix::Identity;
    Matrix birdBodyTranslation = Matrix::CreateTranslation(0.f, -2.5f, 0.f);
    Matrix birdBodyRPoint = Matrix::CreateTranslation(0.f, 0.f, 10.1f);
    Matrix birdBodyScale = Matrix::CreateScale(0.5);
    Matrix birdBodyRotation = Matrix::CreateRotationY(birdR);
    m_world = m_world * birdBodyScale * birdBodyRPoint * birdBodyRotation * birdBodyTranslation;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_birdTexture.Get());
    m_birdBodyModel.Render(context);
    
    //birdWing1
    m_world = Matrix::Identity;
    Matrix birdWingTranslation = Matrix::CreateTranslation(0.f, -2.6f, 0.f);
    Matrix birdWingRPoint = Matrix::CreateTranslation(-0.1f, 0.f, 10.1f);
    Matrix birdWingScale = Matrix::CreateScale(0.2);
    Matrix birdWingRoundRotation = Matrix::CreateRotationY(birdR);
    Matrix birdWingRotation = Matrix::CreateRotationX(90 * 3.14f / 180.f);
    Matrix birdWingFlap = Matrix::CreateRotationX(flap1R);
    m_world = m_world * birdWingScale * birdWingRotation * birdWingFlap * birdWingRPoint * birdWingRoundRotation  * birdWingTranslation;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_birdTexture.Get());
    m_birdWingModel.Render(context);

    //birdWing2
    m_world = Matrix::Identity;
    Matrix birdWingTranslation2 = Matrix::CreateTranslation(0.f, -2.6f, 0.f);
    Matrix birdWingRPoint2 = Matrix::CreateTranslation(-0.1f, 0.f, 9.9f);
    Matrix birdWingRoundRotation2 = Matrix::CreateRotationY(birdR);
    Matrix birdWingRotation2 = Matrix::CreateRotationX(-90 * 3.14f / 180.f);
    Matrix birdWingFlap2 = Matrix::CreateRotationX(flap2R);
    m_world = m_world * birdWingScale * birdWingRotation2 * birdWingFlap2 * birdWingRPoint2 * birdWingRoundRotation2  * birdWingTranslation2;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_mainCameraView, &m_proj, &m_light, m_birdTexture.Get());
    m_birdWingModel.Render(context);

    //cradle balls
    
    if (ball4R > -2.f)
    {
        padding = 1.f;
    }
    else
    {
        padding = 0.f;
    }
    
    //ball1
    m_world = Matrix::Identity;
    Matrix ball1Position = Matrix::CreateTranslation(-5.f, -4.f, -5.f);
    Matrix ballScale = Matrix::CreateScale(0.5);
    m_world = m_world  * ballScale * ball1Position;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters2(context, &m_world, &m_mainCameraView, &m_proj, &m_light, padding, m_silverTexture.Get(), m_goldTexture.Get());
    m_cradleBall.Render(context);

    //ball2
    m_world = Matrix::Identity;
    Matrix ball2Position = Matrix::CreateTranslation(-4.5f, -4.f, -5.f);
    m_world = m_world * ballScale * ball2Position;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters2(context, &m_world, &m_mainCameraView, &m_proj, &m_light, padding, m_goldTexture.Get(), m_silverTexture.Get());
    m_cradleBall.Render(context);

    //ball3
    m_world = Matrix::Identity;
    Matrix ball3Position = Matrix::CreateTranslation(-5.5f, -4.f, -5.f);
    m_world = m_world * ballScale * ball3Position;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters2(context, &m_world, &m_mainCameraView, &m_proj, &m_light, padding, m_goldTexture.Get(), m_silverTexture.Get());
    m_cradleBall.Render(context);

    //ball4
    m_world = Matrix::Identity;
    Matrix ball4RPosition = Matrix::CreateTranslation(1.f, 0.5f, 0.f);//ball4 rotation point
    Matrix ball4Rotation = Matrix::CreateRotationZ(ball4R);
    Matrix ball4Position = Matrix::CreateTranslation(-6.f, -2.88f, -5.f);
    m_world = m_world * ballScale * ball4RPosition * ball4Rotation * ball4Position;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters2(context, &m_world, &m_mainCameraView, &m_proj, &m_light, padding, m_silverTexture.Get(), m_goldTexture.Get());
    m_cradleBall.Render(context);

    


    //ball5
    m_world = Matrix::Identity;
    Matrix ball5RPosition = Matrix::CreateTranslation(1.f, 0.5f, 0.f);//ball5 rotation point
    Matrix ball5Rotation = Matrix::CreateRotationZ(ball5R);
    Matrix ball5Position = Matrix::CreateTranslation(-4.f, -2.88f, -5.f);
    m_world = m_world * ballScale * ball5RPosition * ball5Rotation * ball5Position;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters2(context, &m_world, &m_mainCameraView, &m_proj, &m_light, padding, m_silverTexture.Get(), m_goldTexture.Get());
    m_cradleBall.Render(context);

    //sphere
    m_world = Matrix::Identity;

    m_environmentEffect->SetTexture(m_reflectionTexture->getShaderResourceView());
    m_environmentEffect->SetEnvironmentMap(m_shinyTexture.Get());
    m_world = m_world * Matrix::CreateScale(2) * Matrix::CreateRotationX(-90) * Matrix::CreateTranslation(-5.f, 3.f, -6.f);
    m_environmentEffect->SetWorld(m_world);
    m_environmentEffect->SetView(m_mainCameraView);
    m_environmentEffect->SetProjection(m_proj);
    m_environmentEffect->SetEnvironmentMapAmount(1.f);
    m_sphereModel->Draw(m_environmentEffect.get(), m_batchInputLayout.Get(), false, false, [=] {
        auto sampler = m_states->LinearWrap();
        context->PSSetSamplers(1, 1, &sampler);
        });

    //context;

    m_batchEffect->Apply(context);

    const int vCount = 4;
    const int iCount = 6;
    uint16_t iArray[iCount];
    VertexPositionColor vArray[vCount];

    iArray[0] = 0;
    iArray[1] = 1;
    iArray[2] = 2;
    iArray[3] = 2;
    iArray[4] = 3;
    iArray[5] = 0;

    //back face
    vArray[0] = VertexPositionColor(Vector3(-5.f, -9.f, -5.f), Colors::Green);
    vArray[1] = VertexPositionColor(Vector3(-6.f, -9.f, -5.f), Colors::Blue);
    vArray[2] = VertexPositionColor(Vector3(-6.f, -10.f, -5.f), Colors::Yellow);
    vArray[3] = VertexPositionColor(Vector3(-5.f, -10.f, -5.f), Colors::Red);

    m_batch->Begin();
        m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArray, iCount, vArray, vCount);
    m_batch->End();
    
    //front face
    vArray[3] = VertexPositionColor(Vector3(-5.f, -9.f, -4.f), Colors::Yellow);
    vArray[2] = VertexPositionColor(Vector3(-6.f, -9.f, -4.f), Colors::Red);
    vArray[1] = VertexPositionColor(Vector3(-6.f, -10.f, -4.f), Colors::Green);
    vArray[0] = VertexPositionColor(Vector3(-5.f, -10.f, -4.f), Colors::Blue);

    m_batch->Begin();
    m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArray, iCount, vArray, vCount);
    m_batch->End();

    //up Face
    vArray[1] = VertexPositionColor(Vector3(-5.f, -9.f, -5.f), Colors::Green);
    vArray[0] = VertexPositionColor(Vector3(-6.f, -9.f, -5.f), Colors::Blue);
    vArray[3] = VertexPositionColor(Vector3(-6.f, -9.f, -4.f), Colors::Red);
    vArray[2] = VertexPositionColor(Vector3(-5.f, -9.f, -4.f), Colors::Yellow);

    m_batch->Begin();
    m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArray, iCount, vArray, vCount);
    m_batch->End();

    //right Face
    vArray[0] = VertexPositionColor(Vector3(-5.f, -9.f, -5.f), Colors::Green);
    vArray[1] = VertexPositionColor(Vector3(-5.f, -10.f, -5.f), Colors::Red);
    vArray[2] = VertexPositionColor(Vector3(-5.f, -10.f, -4.f), Colors::Blue);
    vArray[3] = VertexPositionColor(Vector3(-5.f, -9.f, -4.f), Colors::Yellow);

    m_batch->Begin();
    m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArray, iCount, vArray, vCount);
    m_batch->End();

    //left Face
    vArray[3] = VertexPositionColor(Vector3(-6.f, -9.f, -5.f), Colors::Blue);
    vArray[2] = VertexPositionColor(Vector3(-6.f, -10.f, -5.f), Colors::Yellow);
    vArray[1] = VertexPositionColor(Vector3(-6.f, -10.f, -4.f), Colors::Green);
    vArray[0] = VertexPositionColor(Vector3(-6.f, -9.f, -4.f), Colors::Red);

    m_batch->Begin();
    m_batch->DrawIndexed(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, iArray, iCount, vArray, vCount);
    m_batch->End();

    

    /*m_sprites->Begin();
    m_sprites->Draw(m_reflectionTexture->getShaderResourceView(), m_reflectionCameraViewRect);
    m_sprites->End(); */

    m_deviceResources->PIXEndEvent();

    // Show the new frame.
    m_deviceResources->Present();
}

// Helper method to clear the back buffers.
void Game::Clear()
{
    m_deviceResources->PIXBeginEvent(L"Clear");

    // Clear the views.
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTarget = m_deviceResources->GetRenderTargetView();
    auto depthStencil = m_deviceResources->GetDepthStencilView();

    context->ClearRenderTargetView(renderTarget, Colors::CornflowerBlue);
    context->ClearDepthStencilView(depthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
    context->OMSetRenderTargets(1, &renderTarget, depthStencil);

    // Set the viewport.
    auto viewport = m_deviceResources->GetScreenViewport();
    context->RSSetViewports(1, &viewport);

    m_deviceResources->PIXEndEvent();
}
#pragma endregion

#pragma region Message Handlers
// Message handlers
void Game::OnActivated()
{
    // TODO: Game is becoming active window.
}

void Game::OnDeactivated()
{
    // TODO: Game is becoming background window.
}

void Game::OnSuspending()
{
    // TODO: Game is being power-suspended (or minimized).
}

void Game::OnResuming()
{
    m_timer.ResetElapsedTime();

    // TODO: Game is being power-resumed (or returning from minimize).
}

void Game::OnWindowMoved()
{
    auto r = m_deviceResources->GetOutputSize();
    m_deviceResources->WindowSizeChanged(r.right, r.bottom);
}

void Game::OnWindowSizeChanged(int width, int height)
{
    if (!m_deviceResources->WindowSizeChanged(width, height))
        return;

    CreateWindowSizeDependentResources();

    // TODO: Game window is being resized.
}

// Properties
void Game::GetDefaultSize(int& width, int& height) const noexcept
{
    // TODO: Change to desired default window size (note minimum size is 320x200).
    width = 800;
    height = 600;
}
#pragma endregion

#pragma region Direct3D Resources
// These are the resources that depend on the device.
void Game::CreateDeviceDependentResources()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto device = m_deviceResources->GetD3DDevice();



    m_sky = GeometricPrimitive::CreateGeoSphere(context, 2.f, 3, false);
    m_skyboxEffect = std::make_unique<DX::SkyboxEffect>(device);
    m_sky->CreateInputLayout(m_skyboxEffect.get(), m_batchInputLayout.ReleaseAndGetAddressOf());
    
    /// <summary>
    /// /////////////////////////////////////////////////////////////////////////
    /// </summary>
    m_batchEffect = std::make_unique<BasicEffect>(device);
    m_batchEffect->SetVertexColorEnabled(true);

    {
        void const* shaderByteCode;
        size_t byteCodeLength;

        m_batchEffect->GetVertexShaderBytecode(&shaderByteCode, &byteCodeLength);

        DX::ThrowIfFailed(
            device->CreateInputLayout(VertexPositionColor::InputElements,
                VertexPositionColor::InputElementCount,
                shaderByteCode, byteCodeLength,
                m_batchInputLayout.ReleaseAndGetAddressOf())
        );
    }

    m_batch = std::make_unique<PrimitiveBatch<VertexPositionColor>>(context);
    /// <summary>
    /// /////////////////////////////////////////////////////////////////////////
    /// </summary>

    m_states = std::make_unique<CommonStates>(device);
    m_fxFactory = std::make_unique<EffectFactory>(device);
    m_sprites = std::make_unique<SpriteBatch>(context);
    
    m_environmentEffect = std::make_unique<EnvironmentMapEffect>(device);
    m_environmentEffect->EnableDefaultLighting();

    m_sphereModel = GeometricPrimitive::CreateSphere(context);
    m_sphereModel->CreateInputLayout(m_environmentEffect.get(), m_batchInputLayout.ReleaseAndGetAddressOf());
    
    m_basicShader.InitStandard(device, L"light_vs.cso", L"light_ps.cso");

    m_floorModel.InitializeBox(device, 10.0f, 0.1f, 10.0f);
    m_floorLayerModel.InitializeBox(device, 10.0, 0.001, 10.f);

    m_wallModel.InitializeBox(device, 0.1f, 10.f, 10.f);
    m_wallLayerModel.InitializeBox(device, 10.f, 10.f, 0.001f);

    m_lightModel.InitializeModel(device, "cone.obj");

    m_cradleBall.InitializeSphere(device);

    m_chairModel.InitializeModel(device, "chair.obj");
    m_birdBodyModel.InitializeModel(device, "birdBody.obj");
    m_birdWingModel.InitializeModel(device, "birdWing.obj");

    CreateDDSTextureFromFile(device, L"floor.dds", nullptr, m_floorTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"chair.dds", nullptr, m_chairTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"skybox.dds", nullptr, m_shinyTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"skybox.dds", nullptr, m_cubeMap2Texture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"silver.dds", nullptr, m_silverTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"gold.dds", nullptr, m_goldTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"white.dds", nullptr, m_lightTexture.ReleaseAndGetAddressOf());
    CreateDDSTextureFromFile(device, L"black.dds", nullptr, m_birdTexture.ReleaseAndGetAddressOf());

    m_skyboxEffect->SetTexture(m_cubeMap2Texture.Get());
    
    
    m_reflectionTexture = new RenderTexture(device, 800, 600, 1, 2);
    device;
}

// Allocate all memory resources that change on a window SizeChanged event.
void Game::CreateWindowSizeDependentResources()
{
    auto size = m_deviceResources->GetOutputSize();
    float aspectRatio = float(size.right) / float(size.bottom);
    float fovAngleY = 70.0f * XM_PI / 180.0f;

    // This is a simple example of change that can be made when the app is in
    // portrait or snapped view.
    if (aspectRatio < 1.0f)
    {
        fovAngleY *= 2.0f;
    }

    // This sample makes use of a right-handed coordinate system using row-major matrices.
    m_proj = Matrix::CreatePerspectiveFieldOfView(
        fovAngleY,
        aspectRatio,
        0.01f,
        100.0f
    );
    m_skyboxEffect->SetProjection(m_proj);
    m_batchEffect->SetProjection(m_proj);
}

void Game::OnDeviceLost()
{
    m_states.reset();
    m_fxFactory.reset();
    m_batchInputLayout.Reset();
    m_sphereModel.reset();
    m_floorTexture.Reset();
    m_environmentEffect.reset();
    m_silverTexture.Reset();
    m_shinyTexture.Reset();
    m_sky.reset();
    m_skyboxEffect.reset();
    m_cubeMap2Texture.Reset();

    m_batch.reset();
    m_batchEffect.reset();
}

void Game::OnDeviceRestored()
{
    CreateDeviceDependentResources();

    CreateWindowSizeDependentResources();
}

void Game::renderTexturePassSphere()
{
    auto context = m_deviceResources->GetD3DDeviceContext();
    auto renderTargetView = m_deviceResources->GetRenderTargetView();
    auto depthTargetView = m_deviceResources->GetDepthStencilView();

    m_reflectionTexture->setRenderTarget(context);

    m_reflectionTexture->clearRenderTarget(context, 0.0f, 0.0f, 1.0f, 1.0f);

    //floor
    m_world = Matrix::Identity;
    Matrix floorPosition = Matrix::CreateTranslation(-5.f, 0.f, -5.f);
    Matrix floorScale = Matrix::CreateScale(0.5);
    m_world = m_world * floorScale * floorPosition;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_reflectionCameraView, &m_proj, &m_light, m_floorTexture.Get());
    m_floorModel.Render(context);

    //chair
    m_world = Matrix::Identity;
    Matrix chairPosition = Matrix::CreateTranslation(-5.f, 0.f, -5.f);
    Matrix chairScale = Matrix::CreateScale(1.5);
    m_world = m_world * chairScale * chairPosition;

    m_basicShader.EnableShader(context);
    m_basicShader.SetShaderParameters(context, &m_world, &m_reflectionCameraView, &m_proj, &m_light, m_chairTexture.Get());
    m_chairModel.Render(context);

    ////sphere
    //m_world = Matrix::Identity;
    //Matrix spherePosition = Matrix::CreateTranslation(-2.f, 2.f, -5.f);
    //m_world = m_world * spherePosition;

    //m_sphereModel->Draw(m_world, m_reflectionCameraView, m_proj, Colors::White, m_chairTexture.Get());

    context->OMSetRenderTargets(1, &renderTargetView, depthTargetView);
}
#pragma endregion
