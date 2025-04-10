#pragma once
#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include "Camera.h"

using Microsoft::WRL::ComPtr;

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool Initialize(HWND hwnd, int width, int height);
    void BeginScene();
    void Render(Camera* camera, bool dimScene = false);
    void EndScene();

    // Getter for Direct3D device (needed by other components)
    ID3D11Device* GetDevice() const { return m_device.Get(); }
    ID3D11DeviceContext* GetDeviceContext() const { return m_deviceContext.Get(); }

private:
    // DirectX objects
    ComPtr<ID3D11Device> m_device;
    ComPtr<ID3D11DeviceContext> m_deviceContext;
    ComPtr<IDXGISwapChain> m_swapChain;
    ComPtr<ID3D11RenderTargetView> m_renderTargetView;
    ComPtr<ID3D11DepthStencilView> m_depthStencilView;
    ComPtr<ID3D11DepthStencilState> m_depthStencilState;
    ComPtr<ID3D11RasterizerState> m_rasterizerState;

    // Shader objects
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11Buffer> m_constantBuffer;

    // Window properties
    HWND m_hwnd;
    int m_width;
    int m_height;

    // Initialize DirectX components
    bool InitializeDevice();
    bool InitializeRenderTarget();
    bool InitializeDepthStencil();
    bool InitializeRasterizerState();
    bool InitializeShaders();
    bool InitializeConstantBuffer();

    // Shader compilation helper
    bool CompileShaderFromFile(const WCHAR* filename, const char* entryPoint, 
                             const char* shaderModel, ID3DBlob** blob);

    struct ConstantBuffer {
        DirectX::XMMATRIX world;
        DirectX::XMMATRIX view;
        DirectX::XMMATRIX projection;
    };
};
