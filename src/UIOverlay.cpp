#include "UIOverlay.h"
#include <d3dcompiler.h>
#include <vector>

using namespace DirectX;

UIOverlay::UIOverlay() : m_renderer(nullptr) {
}

UIOverlay::~UIOverlay() {
}

bool UIOverlay::Initialize(Renderer* renderer) {
    if (!renderer) return false;
    m_renderer = renderer;

    if (!CreateShaders()) return false;
    if (!CreateBuffers()) return false;
    if (!CreateTextures()) return false;
    if (!CreateStates()) return false;

    // Initialize buttons
    float screenCenterX = 1280.0f / 2.0f;  // Assuming 1280x720 resolution
    float startY = 250.0f;

    // Main menu buttons
    m_startButton = { 
        XMFLOAT2(screenCenterX - BUTTON_WIDTH / 2, startY),
        XMFLOAT2(BUTTON_WIDTH, BUTTON_HEIGHT),
        "Start Game",
        false,
        false
    };

    m_optionsButton = {
        XMFLOAT2(screenCenterX - BUTTON_WIDTH / 2, startY + BUTTON_HEIGHT + BUTTON_PADDING),
        XMFLOAT2(BUTTON_WIDTH, BUTTON_HEIGHT),
        "Options",
        false,
        false
    };

    m_exitButton = {
        XMFLOAT2(screenCenterX - BUTTON_WIDTH / 2, startY + 2 * (BUTTON_HEIGHT + BUTTON_PADDING)),
        XMFLOAT2(BUTTON_WIDTH, BUTTON_HEIGHT),
        "Exit",
        false,
        false
    };

    // Pause menu button
    m_resumeButton = {
        XMFLOAT2(screenCenterX - BUTTON_WIDTH / 2, startY),
        XMFLOAT2(BUTTON_WIDTH, BUTTON_HEIGHT),
        "Resume",
        false,
        false
    };

    return true;
}

bool UIOverlay::CreateShaders() {
    // Shader source code for UI rendering
    const char* vsSource = R"(
        cbuffer ConstantBuffer : register(b0) {
            matrix Transform;
        };

        struct VS_INPUT {
            float3 Position : POSITION;
            float2 TexCoord : TEXCOORD;
            float4 Color : COLOR;
        };

        struct VS_OUTPUT {
            float4 Position : SV_POSITION;
            float2 TexCoord : TEXCOORD;
            float4 Color : COLOR;
        };

        VS_OUTPUT main(VS_INPUT input) {
            VS_OUTPUT output;
            output.Position = mul(float4(input.Position, 1.0f), Transform);
            output.TexCoord = input.TexCoord;
            output.Color = input.Color;
            return output;
        }
    )";

    const char* psSource = R"(
        Texture2D tex2D : register(t0);
        SamplerState samplerState : register(s0);

        struct PS_INPUT {
            float4 Position : SV_POSITION;
            float2 TexCoord : TEXCOORD;
            float4 Color : COLOR;
        };

        float4 main(PS_INPUT input) : SV_Target {
            float4 texColor = tex2D.Sample(samplerState, input.TexCoord);
            return texColor * input.Color;
        }
    )";

    // Compile and create shaders
    ComPtr<ID3DBlob> vsBlob;
    ComPtr<ID3DBlob> psBlob;
    ComPtr<ID3DBlob> errorBlob;

    HRESULT hr = D3DCompile(vsSource, strlen(vsSource), nullptr, nullptr, nullptr,
                           "main", "vs_4_0", 0, 0, vsBlob.GetAddressOf(), 
                           errorBlob.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = D3DCompile(psSource, strlen(psSource), nullptr, nullptr, nullptr,
                    "main", "ps_4_0", 0, 0, psBlob.GetAddressOf(), 
                    errorBlob.GetAddressOf());
    if (FAILED(hr)) return false;

    // Create shader objects
    ID3D11Device* device = m_renderer->GetDevice();
    hr = device->CreateVertexShader(vsBlob->GetBufferPointer(), 
                                  vsBlob->GetBufferSize(), nullptr, 
                                  m_vertexShader.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = device->CreatePixelShader(psBlob->GetBufferPointer(), 
                                 psBlob->GetBufferSize(), nullptr, 
                                 m_pixelShader.GetAddressOf());
    if (FAILED(hr)) return false;

    // Create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, 
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 20, 
          D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = device->CreateInputLayout(layout, ARRAYSIZE(layout),
                                 vsBlob->GetBufferPointer(),
                                 vsBlob->GetBufferSize(),
                                 m_inputLayout.GetAddressOf());
    return SUCCEEDED(hr);
}

bool UIOverlay::CreateBuffers() {
    // Create vertex buffer for UI elements
    std::vector<UIVertex> vertices = {
        // Crosshair vertices
        { XMFLOAT3(-10.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(10.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(0.0f, -10.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) },
        { XMFLOAT3(0.0f, 10.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) }
    };

    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(UIVertex) * vertices.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    HRESULT hr = m_renderer->GetDevice()->CreateBuffer(&bd, &initData, 
                                                      m_vertexBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    // Create index buffer
    std::vector<UINT> indices = { 0, 1, 2, 3 };  // For crosshair lines

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(UINT) * indices.size();
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    initData.pSysMem = indices.data();

    hr = m_renderer->GetDevice()->CreateBuffer(&bd, &initData, 
                                             m_indexBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}

bool UIOverlay::CreateTextures() {
    // In a real implementation, we would load textures from files
    // For this example, we'll create simple placeholder textures
    return true;
}

bool UIOverlay::CreateStates() {
    // Create sampler state
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

    HRESULT hr = m_renderer->GetDevice()->CreateSamplerState(&sampDesc, 
                                                            m_samplerState.GetAddressOf());
    if (FAILED(hr)) return false;

    // Create blend state for alpha blending
    D3D11_BLEND_DESC blendDesc = {};
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    hr = m_renderer->GetDevice()->CreateBlendState(&blendDesc, m_blendState.GetAddressOf());
    return SUCCEEDED(hr);
}

void UIOverlay::Update(GameState currentState) {
    UpdateButtonStates();
}

void UIOverlay::RenderMainMenu() {
    // Set UI rendering states
    ID3D11DeviceContext* context = m_renderer->GetDeviceContext();
    context->OMSetBlendState(m_blendState.Get(), nullptr, 0xffffffff);
    context->PSSetSamplers(0, 1, m_samplerState.GetAddressOf());

    // Render background
    // TODO: Implement background rendering

    // Render buttons
    RenderButton(m_startButton);
    RenderButton(m_optionsButton);
    RenderButton(m_exitButton);
}

void UIOverlay::RenderHUD() {
    RenderCrosshair();
    RenderHealthBar(100.0f);  // TODO: Get actual health value
    RenderAmmoCount(30);      // TODO: Get actual ammo count
}

void UIOverlay::RenderPauseMenu() {
    // Dim the background
    // TODO: Implement screen dimming

    // Render pause menu elements
    RenderButton(m_resumeButton);
    RenderButton(m_exitButton);
}

void UIOverlay::RenderButton(const Button& button) {
    // TODO: Implement button rendering with proper texturing and text
}

void UIOverlay::RenderText(const std::string& text, XMFLOAT2 position, float scale) {
    // TODO: Implement text rendering
}

void UIOverlay::RenderCrosshair() {
    // TODO: Implement crosshair rendering
}

void UIOverlay::RenderHealthBar(float health) {
    // TODO: Implement health bar rendering
}

void UIOverlay::RenderAmmoCount(int ammo) {
    // TODO: Implement ammo counter rendering
}

bool UIOverlay::IsPointInRect(XMFLOAT2 point, XMFLOAT2 position, XMFLOAT2 size) {
    return point.x >= position.x && point.x <= position.x + size.x &&
           point.y >= position.y && point.y <= position.y + size.y;
}

void UIOverlay::UpdateButtonStates() {
    // TODO: Implement button state updates based on mouse position and clicks
}
