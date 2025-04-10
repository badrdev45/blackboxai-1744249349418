#pragma once
#include <windows.h>
#include <d3d11.h>
#include <directxmath.h>
#include <memory>
#include "Renderer.h"
#include "Input.h"
#include "Camera.h"
#include "Player.h"
#include "UIOverlay.h"

class Game {
public:
    Game();
    ~Game();

    bool Initialize(HWND hwnd, int width, int height);
    void Update();
    void Render();

private:
    // Window properties
    HWND m_hwnd;
    int m_width;
    int m_height;

    // DirectX objects
    std::unique_ptr<Renderer> m_renderer;
    std::unique_ptr<Input> m_input;
    std::unique_ptr<Camera> m_camera;
    std::unique_ptr<Player> m_player;
    std::unique_ptr<UIOverlay> m_uiOverlay;

    // Game states
    enum class GameState {
        MainMenu,
        Playing,
        Paused
    };
    GameState m_gameState;

    // Initialize subsystems
    bool InitializeRenderer();
    bool InitializeInput();
    bool InitializeCamera();
    bool InitializePlayer();
    bool InitializeUI();

    // Update subsystems
    void UpdateInput();
    void UpdateCamera();
    void UpdatePlayer();
    void UpdateUI();
};
