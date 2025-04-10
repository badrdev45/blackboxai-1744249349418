#pragma once
#include <directxmath.h>
#include "Camera.h"
#include "Input.h"

class Player {
public:
    Player();
    ~Player();

    bool Initialize(Camera* camera, Input* input);
    void Update();

    // Movement and control
    void Move(const DirectX::XMFLOAT3& direction);
    void Rotate(float deltaPitch, float deltaYaw);
    void Shoot();

    // Getters
    DirectX::XMFLOAT3 GetPosition() const { return m_position; }
    DirectX::XMFLOAT3 GetForwardVector() const;
    float GetHealth() const { return m_health; }
    int GetAmmo() const { return m_ammo; }
    bool IsAlive() const { return m_health > 0.0f; }

private:
    // Core components
    Camera* m_camera;
    Input* m_input;

    // Player properties
    DirectX::XMFLOAT3 m_position;
    float m_pitch;
    float m_yaw;
    float m_health;
    int m_ammo;

    // Movement properties
    float m_moveSpeed;
    float m_rotationSpeed;
    float m_mouseSensitivity;
    bool m_isJumping;
    DirectX::XMFLOAT3 m_velocity;

    // Combat properties
    float m_shootCooldown;
    float m_lastShotTime;

    // Constants
    static constexpr float MAX_HEALTH = 100.0f;
    static constexpr int MAX_AMMO = 30;
    static constexpr float JUMP_FORCE = 5.0f;
    static constexpr float GRAVITY = -9.81f;

    // Helper methods
    void HandleKeyboardInput();
    void HandleMouseInput();
    void UpdatePosition(float deltaTime);
    void UpdateCamera();
    bool CanShoot() const;
};
