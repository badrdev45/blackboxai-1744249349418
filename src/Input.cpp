#include "Input.h"

Input::Input() :
    m_hwnd(nullptr),
    m_cursorVisible(true),
    m_cursorConfined(false) {
    
    // Initialize arrays
    m_currentKeyState.fill(false);
    m_previousKeyState.fill(false);

    // Initialize mouse state
    m_mouseState = {};
}

Input::~Input() {
    // Restore cursor state
    ShowCursor(true);
    ClipCursor(nullptr);
}

bool Input::Initialize(HWND hwnd) {
    m_hwnd = hwnd;

    // Get window dimensions for cursor confinement
    GetClientRect(m_hwnd, &m_cursorClip);
    MapWindowPoints(m_hwnd, nullptr, reinterpret_cast<POINT*>(&m_cursorClip), 2);

    // Center the mouse in the window
    int centerX = (m_cursorClip.left + m_cursorClip.right) / 2;
    int centerY = (m_cursorClip.top + m_cursorClip.bottom) / 2;
    SetMousePosition(centerX, centerY);

    return true;
}

void Input::Update() {
    // Store previous states
    m_previousKeyState = m_currentKeyState;
    m_mouseState.prevLeftButton = m_mouseState.leftButton;
    m_mouseState.prevRightButton = m_mouseState.rightButton;
    m_mouseState.prevMiddleButton = m_mouseState.middleButton;
    m_mouseState.prevX = m_mouseState.x;
    m_mouseState.prevY = m_mouseState.y;

    // Update current states
    UpdateKeyboardState();
    UpdateMouseState();

    // Confine cursor if needed
    if (m_cursorConfined) {
        ClampMousePosition();
    }
}

void Input::UpdateKeyboardState() {
    for (int i = 0; i < NUM_KEYS; ++i) {
        m_currentKeyState[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }
}

void Input::UpdateMouseState() {
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(m_hwnd, &cursorPos);

    m_mouseState.x = cursorPos.x;
    m_mouseState.y = cursorPos.y;

    m_mouseState.leftButton = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    m_mouseState.rightButton = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    m_mouseState.middleButton = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;
}

bool Input::IsKeyDown(int key) const {
    return m_currentKeyState[key];
}

bool Input::IsKeyPressed(int key) const {
    return m_currentKeyState[key] && !m_previousKeyState[key];
}

bool Input::IsKeyReleased(int key) const {
    return !m_currentKeyState[key] && m_previousKeyState[key];
}

DirectX::XMFLOAT2 Input::GetMouseDelta() const {
    return DirectX::XMFLOAT2(
        static_cast<float>(m_mouseState.x - m_mouseState.prevX),
        static_cast<float>(m_mouseState.y - m_mouseState.prevY)
    );
}

DirectX::XMFLOAT2 Input::GetMousePosition() const {
    return DirectX::XMFLOAT2(
        static_cast<float>(m_mouseState.x),
        static_cast<float>(m_mouseState.y)
    );
}

bool Input::IsMouseButtonDown(int button) const {
    switch (button) {
        case VK_LBUTTON: return m_mouseState.leftButton;
        case VK_RBUTTON: return m_mouseState.rightButton;
        case VK_MBUTTON: return m_mouseState.middleButton;
        default: return false;
    }
}

bool Input::IsMouseButtonPressed(int button) const {
    switch (button) {
        case VK_LBUTTON:
            return m_mouseState.leftButton && !m_mouseState.prevLeftButton;
        case VK_RBUTTON:
            return m_mouseState.rightButton && !m_mouseState.prevRightButton;
        case VK_MBUTTON:
            return m_mouseState.middleButton && !m_mouseState.prevMiddleButton;
        default:
            return false;
    }
}

bool Input::IsMouseButtonReleased(int button) const {
    switch (button) {
        case VK_LBUTTON:
            return !m_mouseState.leftButton && m_mouseState.prevLeftButton;
        case VK_RBUTTON:
            return !m_mouseState.rightButton && m_mouseState.prevRightButton;
        case VK_MBUTTON:
            return !m_mouseState.middleButton && m_mouseState.prevMiddleButton;
        default:
            return false;
    }
}

void Input::SetMousePosition(int x, int y) {
    POINT pt = { x, y };
    ClientToScreen(m_hwnd, &pt);
    SetCursorPos(pt.x, pt.y);
    
    m_mouseState.x = x;
    m_mouseState.y = y;
    m_mouseState.prevX = x;
    m_mouseState.prevY = y;
}

void Input::ShowCursor(bool show) {
    if (m_cursorVisible != show) {
        m_cursorVisible = show;
        ::ShowCursor(show);
    }
}

void Input::ConfineCursor(bool confine) {
    m_cursorConfined = confine;
    if (confine) {
        ClipCursor(&m_cursorClip);
    } else {
        ClipCursor(nullptr);
    }
}

void Input::ClampMousePosition() {
    int x = m_mouseState.x;
    int y = m_mouseState.y;
    bool needsUpdate = false;

    if (x < m_cursorClip.left) {
        x = m_cursorClip.left;
        needsUpdate = true;
    }
    else if (x > m_cursorClip.right) {
        x = m_cursorClip.right;
        needsUpdate = true;
    }

    if (y < m_cursorClip.top) {
        y = m_cursorClip.top;
        needsUpdate = true;
    }
    else if (y > m_cursorClip.bottom) {
        y = m_cursorClip.bottom;
        needsUpdate = true;
    }

    if (needsUpdate) {
        SetMousePosition(x, y);
    }
}
