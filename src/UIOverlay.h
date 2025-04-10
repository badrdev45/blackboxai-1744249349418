#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <wrl/client.h>
#include <string>
#include "Renderer.h"

using Microsoft::WRL::ComPtr;

class UIOverlay {
public:
    UIOverlay();
    ~UIOverlay();

    bool Initialize(Renderer* renderer);

    void Update(GameState currentState);
    void RenderMainMenu();
    void RenderHUD();
    void RenderPauseMenu();

private:
    // Core components
    Renderer* m_renderer;
    
    // DirectX resources
    ComPtr<ID3D11Buffer> m_vertexBuffer;
    ComPtr<ID3D11Buffer> m_indexBuffer;
    ComPtr<ID3D11VertexShader> m_vertexShader;
    ComPtr<ID3D11PixelShader> m_pixelShader;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11SamplerState> m_samplerState;
    ComPtr<ID3D11BlendState> m_blendState;

    // UI textures
    ComPtr<ID3D11ShaderResourceView> m_crosshairTexture;
    ComPtr<ID3D11ShaderResourceView> m_menuBackgroundTexture;
    ComPtr<ID3D11ShaderResourceView> m_buttonTexture;

    // UI state
    struct Button {
        DirectX::XMFLOAT2 position;
        DirectX::XMFLOAT2 size;
        std::string text;
        bool isHovered;
        bool isPressed;
    };

    // UI elements
    Button m_startButton;
    Button m_optionsButton;
    Button m_exitButton;
    Button m_resumeButton;

    // Helper methods
    bool CreateShaders();
    bool CreateBuffers();
    bool CreateTextures();
    bool CreateStates();
    
    void RenderButton(const Button& button);
    void RenderText(const std::string& text, DirectX::XMFLOAT2 position, float scale = 1.0f);
    void RenderCrosshair();
    void RenderHealthBar(float health);
    void RenderAmmoCount(int ammo);
    
    bool IsPointInRect(DirectX::XMFLOAT2 point, DirectX::XMFLOAT2 position, DirectX::XMFLOAT2 size);
    void UpdateButtonStates();

    // Vertex structure for UI elements
    struct UIVertex {
        DirectX::XMFLOAT3 position;
        DirectX::XMFLOAT2 texCoord;
        DirectX::XMFLOAT4 color;
    };

    // Constants
    static constexpr float BUTTON_WIDTH = 200.0f;
    static constexpr float BUTTON_HEIGHT = 50.0f;
    static constexpr float BUTTON_PADDING = 20.0f;
};
