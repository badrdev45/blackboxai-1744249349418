#pragma once
#include <windows.h>
#include <directxmath.h>
#include <array>

class Input {
public:
    Input();
    ~Input();

    bool Initialize(HWND hwnd);
    void Update();

    // Keyboard state
    bool IsKeyDown(int key) const;
    bool IsKeyPressed(int key) const;  // Returns true only on the first frame the key is down
    bool IsKeyReleased(int key) const; // Returns true only on the first frame the key is up

    // Mouse state
    DirectX::XMFLOAT2 GetMouseDelta() const;
    DirectX::XMFLOAT2 GetMousePosition() const;
    bool IsMouseButtonDown(int button) const;
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonReleased(int button) const;

    // Mouse control
    void SetMousePosition(int x, int y);
    void ShowCursor(bool show);
    void ConfineCursor(bool confine);

private:
    HWND m_hwnd;
    
    // Keyboard state
    static const int NUM_KEYS = 256;
    std::array<bool, NUM_KEYS> m_currentKeyState;
    std::array<bool, NUM_KEYS> m_previousKeyState;

    // Mouse state
    struct MouseState {
        int x;
        int y;
        int prevX;
        int prevY;
        bool leftButton;
        bool rightButton;
        bool middleButton;
        bool prevLeftButton;
        bool prevRightButton;
        bool prevMiddleButton;
    } m_mouseState;

    bool m_cursorVisible;
    bool m_cursorConfined;
    RECT m_cursorClip;

    // Helper methods
    void UpdateKeyboardState();
    void UpdateMouseState();
    void ClampMousePosition();
};
