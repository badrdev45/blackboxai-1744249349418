#include "Player.h"
#include <algorithm>
#include <chrono>

using namespace DirectX;

Player::Player() :
    m_camera(nullptr),
    m_input(nullptr),
    m_position(0.0f, 0.0f, 0.0f),
    m_pitch(0.0f),
    m_yaw(0.0f),
    m_health(MAX_HEALTH),
    m_ammo(MAX_AMMO),
    m_moveSpeed(5.0f),
    m_rotationSpeed(2.0f),
    m_mouseSensitivity(0.003f),
    m_isJumping(false),
    m_velocity(0.0f, 0.0f, 0.0f),
    m_shootCooldown(0.1f),
    m_lastShotTime(0.0f) {
}

Player::~Player() {
}

bool Player::Initialize(Camera* camera, Input* input) {
    if (!camera || !input) {
        return false;
    }

    m_camera = camera;
    m_input = input;

    // Set initial camera position and rotation
    m_camera->SetPosition(m_position);
    m_camera->SetRotation(m_pitch, m_yaw);

    // Hide and confine cursor for FPS controls
    m_input->ShowCursor(false);
    m_input->ConfineCursor(true);

    return true;
}

void Player::Update() {
    // Get the current time for delta time calculation
    static auto lastTime = std::chrono::high_resolution_clock::now();
    auto currentTime = std::chrono::high_resolution_clock::now();
    float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
    lastTime = currentTime;

    // Handle input
    HandleKeyboardInput();
    HandleMouseInput();

    // Update position based on physics
    UpdatePosition(deltaTime);

    // Update camera to match player position and rotation
    UpdateCamera();
}

void Player::HandleKeyboardInput() {
    // Movement vector
    XMFLOAT3 moveDirection(0.0f, 0.0f, 0.0f);

    // Forward/Backward
    if (m_input->IsKeyDown('W')) {
        moveDirection.z += 1.0f;
    }
    if (m_input->IsKeyDown('S')) {
        moveDirection.z -= 1.0f;
    }

    // Left/Right
    if (m_input->IsKeyDown('A')) {
        moveDirection.x -= 1.0f;
    }
    if (m_input->IsKeyDown('D')) {
        moveDirection.x += 1.0f;
    }

    // Jump
    if (m_input->IsKeyPressed(VK_SPACE) && !m_isJumping) {
        m_velocity.y = JUMP_FORCE;
        m_isJumping = true;
    }

    // Normalize movement vector if moving diagonally
    if (moveDirection.x != 0.0f || moveDirection.z != 0.0f) {
        XMVECTOR movement = XMVector3Normalize(XMLoadFloat3(&moveDirection));
        XMStoreFloat3(&moveDirection, movement);
    }

    // Apply movement
    Move(moveDirection);
}

void Player::HandleMouseInput() {
    // Get mouse movement
    XMFLOAT2 mouseDelta = m_input->GetMouseDelta();

    // Apply rotation based on mouse movement
    Rotate(-mouseDelta.y * m_mouseSensitivity, -mouseDelta.x * m_mouseSensitivity);

    // Handle shooting
    if (m_input->IsMouseButtonDown(VK_LBUTTON) && CanShoot()) {
        Shoot();
    }
}

void Player::Move(const XMFLOAT3& direction) {
    // Transform movement direction by camera rotation (only yaw)
    XMMATRIX rotationMatrix = XMMatrixRotationY(m_yaw);
    XMVECTOR directionVector = XMLoadFloat3(&direction);
    directionVector = XMVector3Transform(directionVector, rotationMatrix);

    // Scale by move speed
    directionVector = XMVectorScale(directionVector, m_moveSpeed);

    // Update velocity (horizontal movement)
    XMFLOAT3 horizontalVelocity;
    XMStoreFloat3(&horizontalVelocity, directionVector);
    m_velocity.x = horizontalVelocity.x;
    m_velocity.z = horizontalVelocity.z;
}

void Player::Rotate(float deltaPitch, float deltaYaw) {
    // Update rotation angles
    m_pitch += deltaPitch;
    m_yaw += deltaYaw;

    // Clamp pitch to prevent over-rotation
    m_pitch = std::max(-XM_PIDIV2, std::min(m_pitch, XM_PIDIV2));

    // Normalize yaw to [0, 2π]
    while (m_yaw >= XM_2PI) {
        m_yaw -= XM_2PI;
    }
    while (m_yaw < 0.0f) {
        m_yaw += XM_2PI;
    }
}

void Player::UpdatePosition(float deltaTime) {
    // Apply gravity
    if (m_isJumping) {
        m_velocity.y += GRAVITY * deltaTime;
    }

    // Update position based on velocity
    m_position.x += m_velocity.x * deltaTime;
    m_position.y += m_velocity.y * deltaTime;
    m_position.z += m_velocity.z * deltaTime;

    // Simple ground collision
    if (m_position.y <= 0.0f) {
        m_position.y = 0.0f;
        m_velocity.y = 0.0f;
        m_isJumping = false;
    }
}

void Player::UpdateCamera() {
    // Update camera position and rotation to match player
    m_camera->SetPosition(m_position);
    m_camera->SetRotation(m_pitch, m_yaw);
}

void Player::Shoot() {
    if (m_ammo <= 0) {
        return;
    }

    // Get current time
    float currentTime = static_cast<float>(
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count()
    ) / 1000.0f;

    // Check cooldown
    if (currentTime - m_lastShotTime < m_shootCooldown) {
        return;
    }

    // Perform shooting logic
    m_ammo--;
    m_lastShotTime = currentTime;

    // TODO: Implement actual shooting mechanics (raycasting, projectiles, etc.)
}

bool Player::CanShoot() const {
    return m_ammo > 0;
}

XMFLOAT3 Player::GetForwardVector() const {
    // Calculate forward vector based on pitch and yaw
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f);
    XMVECTOR forwardVector = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    
    XMFLOAT3 forward;
    XMStoreFloat3(&forward, forwardVector);
    return forward;
}
