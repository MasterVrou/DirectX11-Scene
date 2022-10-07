//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Camera.h"
#include "Input.h"
#include "modelclass.h"
#include "Shader.h"
#include "Light.h"
#include "RenderTexture.h"
#include "SkyboxEffect.h"


// A basic game implementation that creates a D3D11 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game() = default;

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void Update(DX::StepTimer const& timer);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    void renderTexturePassSphere();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>                                    m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                                                           m_timer;

    DirectX::SimpleMath::Matrix                                             m_world;
    DirectX::SimpleMath::Matrix                                             m_view;
    DirectX::SimpleMath::Matrix                                             m_proj;

    std::unique_ptr<DirectX::PrimitiveBatch<DirectX::VertexPositionColor>>  m_batch;
    std::unique_ptr<DirectX::BasicEffect>                                   m_batchEffect;
    std::unique_ptr<DirectX::CommonStates>                                  m_states;
    std::unique_ptr<DirectX::IEffectFactory>                                m_fxFactory;
    std::unique_ptr<DirectX::EnvironmentMapEffect>                          m_environmentEffect;
    std::unique_ptr<DX::SkyboxEffect>                                       m_skyboxEffect;
    Microsoft::WRL::ComPtr<ID3D11InputLayout>                               m_batchInputLayout;
    std::unique_ptr<DirectX::SpriteBatch>                                   m_sprites;

    //Shaders
    Shader                                                                  m_basicShader;

    //Lights
    Light                                                                   m_light;
    Light                                                                   m_light2;

    //Models
    ModelClass                                                              m_cradleBall;
    float                                                                   ball4R = -2.f;
    float                                                                   ball5R = 2.f;
    bool                                                                    ball4Left = true;
    bool                                                                    ball4Right = false;
    bool                                                                    ball5Left = false;
    bool                                                                    ball5Right = false;
    float                                                                   padding;
    float                                                                   birdR = 0;
    float                                                                   flap1R = -0.5f;
    float                                                                   flap2R = 0;
    float                                                                   flap1Up = true;
    float                                                                   flap1Down = false;
    float                                                                   flap2Up = true;
    float                                                                   flap2Down = false;


    ModelClass                                                              m_floorModel;
    ModelClass                                                              m_floorLayerModel;
    ModelClass                                                              m_wallModel;
    ModelClass                                                              m_wallLayerModel;
    ModelClass                                                              m_chairModel;
    ModelClass                                                              m_lightModel;
    ModelClass                                                              m_birdBodyModel;
    ModelClass                                                              m_birdWingModel;
    std::unique_ptr<DirectX::GeometricPrimitive>                            m_sphereModel;
    std::unique_ptr<DirectX::GeometricPrimitive>                            m_sky;


    //Textures
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_floorTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_chairTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_sphereTexture1;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_sphereTexture2;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_silverTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_goldTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_shinyTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_cubeMap2Texture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_lightTexture;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>                        m_birdTexture;

    //RenderToTexture
    RenderTexture*                                                          m_reflectionTexture;

    //Camera and input
    Camera                                                                  m_mainCamera;
    Camera                                                                  m_reflectionCamera;

    RECT										                            m_mainCameraViewRect;
    RECT                                                                    m_reflectionCameraViewRect;

    Input									                                m_input;
    InputCommands							                                m_gameInputCommands;

    DirectX::SimpleMath::Matrix                                             m_mainCameraView;
    DirectX::SimpleMath::Matrix                                             m_reflectionCameraView;
};