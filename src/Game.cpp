#include "Game.h"
#include <stdexcept>

Game::Game() : 
    m_hwnd(nullptr),
    m_width(0),
    m_height(0),
    m_gameState(GameState::MainMenu) {
}

Game::~Game() {
    // Smart pointers will automatically clean up resources
}

bool Game::Initialize(HWND hwnd, int width, int height) {
    m_hwnd = hwnd;
    m_width = width;
    m_height = height;

    // Initialize all subsystems
    if (!InitializeRenderer()) {
        throw std::runtime_error("Failed to initialize renderer");
        return false;
    }

    if (!InitializeInput()) {
        throw std::runtime_error("Failed to initialize input");
        return false;
    }

    if (!InitializeCamera()) {
        throw std::runtime_error("Failed to initialize camera");
        return false;
    }

    if (!InitializePlayer()) {
        throw std::runtime_error("Failed to initialize player");
        return false;
    }

    if (!InitializeUI()) {
        throw std::runtime_error("Failed to initialize UI");
        return false;
    }

    return true;
}

bool Game::InitializeRenderer() {
    try {
        m_renderer = std::make_unique<Renderer>();
        return m_renderer->Initialize(m_hwnd, m_width, m_height);
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool Game::InitializeInput() {
    try {
        m_input = std::make_unique<Input>();
        return m_input->Initialize(m_hwnd);
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool Game::InitializeCamera() {
    try {
        m_camera = std::make_unique<Camera>();
        return m_camera->Initialize(DirectX::XMFLOAT3(0.0f, 0.0f, -5.0f));
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool Game::InitializePlayer() {
    try {
        m_player = std::make_unique<Player>();
        return m_player->Initialize(m_camera.get(), m_input.get());
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

bool Game::InitializeUI() {
    try {
        m_uiOverlay = std::make_unique<UIOverlay>();
        return m_uiOverlay->Initialize(m_renderer.get());
    }
    catch (const std::exception& e) {
        // Log error
        return false;
    }
}

void Game::Update() {
    switch (m_gameState) {
        case GameState::MainMenu:
            UpdateUI();
            break;

        case GameState::Playing:
            UpdateInput();
            UpdatePlayer();
            UpdateCamera();
            UpdateUI();
            break;

        case GameState::Paused:
            UpdateUI();
            break;
    }
}

void Game::UpdateInput() {
    if (m_input) {
        m_input->Update();
        
        // Check for pause button (ESC)
        if (m_input->IsKeyPressed(VK_ESCAPE)) {
            m_gameState = (m_gameState == GameState::Playing) ? 
                         GameState::Paused : GameState::Playing;
        }
    }
}

void Game::UpdateCamera() {
    if (m_camera) {
        m_camera->Update();
    }
}

void Game::UpdatePlayer() {
    if (m_player) {
        m_player->Update();
    }
}

void Game::UpdateUI() {
    if (m_uiOverlay) {
        m_uiOverlay->Update(m_gameState);
    }
}

void Game::Render() {
    if (!m_renderer) return;

    m_renderer->BeginScene();

    switch (m_gameState) {
        case GameState::MainMenu:
            m_uiOverlay->RenderMainMenu();
            break;

        case GameState::Playing:
            // Render 3D scene
            m_renderer->Render(m_camera.get());
            
            // Render HUD
            m_uiOverlay->RenderHUD();
            break;

        case GameState::Paused:
            // Render 3D scene (dimmed)
            m_renderer->Render(m_camera.get(), true);
            
            // Render pause menu
            m_uiOverlay->RenderPauseMenu();
            break;
    }

    m_renderer->EndScene();
}
