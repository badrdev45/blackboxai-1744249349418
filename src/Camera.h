#pragma once
#include <directxmath.h>

class Camera {
public:
    Camera();
    ~Camera();

    bool Initialize(DirectX::XMFLOAT3 position);
    void Update();

    // Camera control methods
    void SetPosition(const DirectX::XMFLOAT3& position);
    void SetRotation(float pitch, float yaw);
    void MoveForward(float distance);
    void MoveRight(float distance);
    void MoveUp(float distance);
    void Rotate(float deltaPitch, float deltaYaw);

    // Getter methods
    DirectX::XMMATRIX GetViewMatrix() const;
    DirectX::XMMATRIX GetProjectionMatrix() const;
    DirectX::XMFLOAT3 GetPosition() const { return m_position; }
    DirectX::XMFLOAT3 GetForward() const;
    DirectX::XMFLOAT3 GetRight() const;
    DirectX::XMFLOAT3 GetUp() const;

private:
    // Camera properties
    DirectX::XMFLOAT3 m_position;
    float m_pitch;  // X-axis rotation
    float m_yaw;    // Y-axis rotation
    
    // View frustum properties
    float m_fieldOfView;
    float m_aspectRatio;
    float m_nearPlane;
    float m_farPlane;

    // Cached matrices
    mutable DirectX::XMMATRIX m_viewMatrix;
    mutable DirectX::XMMATRIX m_projectionMatrix;
    mutable bool m_viewDirty;
    mutable bool m_projectionDirty;

    // Helper methods
    void UpdateViewMatrix() const;
    void UpdateProjectionMatrix() const;
    void ClampRotation();

    // Constants
    static constexpr float MAX_PITCH = DirectX::XM_PIDIV2 - 0.1f;  // Just under 90 degrees
    static constexpr float MIN_PITCH = -MAX_PITCH;
};
