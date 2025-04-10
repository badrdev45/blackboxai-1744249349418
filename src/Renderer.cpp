#include "Renderer.h"
#include <d3dcompiler.h>
#include <directxcolors.h>
#include <stdexcept>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

Renderer::Renderer() : m_hwnd(nullptr), m_width(0), m_height(0) {}

Renderer::~Renderer() {}

bool Renderer::Initialize(HWND hwnd, int width, int height) {
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;

    if (!InitializeDevice()) return false;
    if (!InitializeRenderTarget()) return false;
    if (!InitializeDepthStencil()) return false;
    if (!InitializeRasterizerState()) return false;
    if (!InitializeShaders()) return false;
    if (!InitializeConstantBuffer()) return false;

    return true;
}

bool Renderer::InitializeDevice() {
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;

    D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0
    };
    UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferCount = 1;
    sd.BufferDesc.Width = m_width;
    sd.BufferDesc.Height = m_height;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = m_hwnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    D3D_FEATURE_LEVEL featureLevel;

    HRESULT hr = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags,
        featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
        &sd, m_swapChain.GetAddressOf(),
        m_device.GetAddressOf(), &featureLevel,
        m_deviceContext.GetAddressOf()
    );

    return SUCCEEDED(hr);
}

bool Renderer::InitializeRenderTarget() {
    ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), 
                                       reinterpret_cast<void**>(backBuffer.GetAddressOf()));
    if (FAILED(hr)) return false;

    hr = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, 
                                        m_renderTargetView.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Renderer::InitializeDepthStencil() {
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = m_width;
    depthStencilDesc.Height = m_height;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    ComPtr<ID3D11Texture2D> depthStencilBuffer;
    HRESULT hr = m_device->CreateTexture2D(&depthStencilDesc, nullptr, 
                                          depthStencilBuffer.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = m_device->CreateDepthStencilView(depthStencilBuffer.Get(), nullptr, 
                                         m_depthStencilView.GetAddressOf());
    if (FAILED(hr)) return false;

    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    dsDesc.DepthEnable = true;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

    hr = m_device->CreateDepthStencilState(&dsDesc, m_depthStencilState.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Renderer::InitializeRasterizerState() {
    D3D11_RASTERIZER_DESC rasterizerDesc = {};
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.CullMode = D3D11_CULL_BACK;
    rasterizerDesc.FrontCounterClockwise = false;
    rasterizerDesc.DepthClipEnable = true;

    HRESULT hr = m_device->CreateRasterizerState(&rasterizerDesc, 
                                                m_rasterizerState.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Renderer::InitializeShaders() {
    // Note: In a real implementation, we would load and compile shaders from files
    // For this example, we'll use simple hardcoded shaders
    const char* vsSource = R"(
        cbuffer ConstantBuffer : register(b0) {
            matrix World;
            matrix View;
            matrix Projection;
        };
        
        struct VS_INPUT {
            float4 Pos : POSITION;
            float4 Color : COLOR;
        };
        
        struct VS_OUTPUT {
            float4 Pos : SV_POSITION;
            float4 Color : COLOR;
        };
        
        VS_OUTPUT main(VS_INPUT input) {
            VS_OUTPUT output;
            output.Pos = mul(input.Pos, World);
            output.Pos = mul(output.Pos, View);
            output.Pos = mul(output.Pos, Projection);
            output.Color = input.Color;
            return output;
        }
    )";

    const char* psSource = R"(
        struct PS_INPUT {
            float4 Pos : SV_POSITION;
            float4 Color : COLOR;
        };
        
        float4 main(PS_INPUT input) : SV_Target {
            return input.Color;
        }
    )";

    // Compile shaders
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
    hr = m_device->CreateVertexShader(vsBlob->GetBufferPointer(), 
                                     vsBlob->GetBufferSize(), nullptr, 
                                     m_vertexShader.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = m_device->CreatePixelShader(psBlob->GetBufferPointer(), 
                                    psBlob->GetBufferSize(), nullptr, 
                                    m_pixelShader.GetAddressOf());
    if (FAILED(hr)) return false;

    // Create input layout
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, 
          D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, 
          D3D11_INPUT_PER_VERTEX_DATA, 0 }
    };

    hr = m_device->CreateInputLayout(layout, ARRAYSIZE(layout),
                                   vsBlob->GetBufferPointer(),
                                   vsBlob->GetBufferSize(),
                                   m_inputLayout.GetAddressOf());
    return SUCCEEDED(hr);
}

bool Renderer::InitializeConstantBuffer() {
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

    HRESULT hr = m_device->CreateBuffer(&bd, nullptr, m_constantBuffer.GetAddressOf());
    return SUCCEEDED(hr);
}

void Renderer::BeginScene() {
    // Clear render target and depth stencil
    float clearColor[4] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor);
    m_deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), 
                                         D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 
                                         1.0f, 0);

    // Set render target
    m_deviceContext->OMSetRenderTargetView(m_renderTargetView.Get(), 
                                         m_depthStencilView.Get());

    // Set viewport
    D3D11_VIEWPORT viewport = {};
    viewport.Width = static_cast<float>(m_width);
    viewport.Height = static_cast<float>(m_height);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    m_deviceContext->RSSetViewports(1, &viewport);
}

void Renderer::Render(Camera* camera, bool dimScene) {
    if (!camera) return;

    // Update constant buffer with new matrices
    ConstantBuffer cb;
    cb.world = XMMatrixIdentity();
    cb.view = camera->GetViewMatrix();
    cb.projection = camera->GetProjectionMatrix();

    m_deviceContext->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &cb, 0, 0);

    // Set shaders and constant buffers
    m_deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
    m_deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
    m_deviceContext->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

    // Set the input layout and primitive topology
    m_deviceContext->IASetInputLayout(m_inputLayout.Get());
    m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // TODO: Add actual geometry rendering here
    // For now, this is just a placeholder for the rendering pipeline
}

void Renderer::EndScene() {
    m_swapChain->Present(1, 0);
}
