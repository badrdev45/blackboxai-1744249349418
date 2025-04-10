#include "Camera.h"
#include <algorithm>

using namespace DirectX;

Camera::Camera() :
    m_position(0.0f, 0.0f, 0.0f),
    m_pitch(0.0f),
    m_yaw(0.0f),
    m_fieldOfView(XM_PIDIV4),  // 45 degrees
    m_aspectRatio(16.0f / 9.0f),
    m_nearPlane(0.1f),
    m_farPlane(1000.0f),
    m_viewDirty(true),
    m_projectionDirty(true) {
}

Camera::~Camera() {
}

bool Camera::Initialize(XMFLOAT3 position) {
    m_position = position;
    m_viewDirty = true;
    m_projectionDirty = true;
    return true;
}

void Camera::Update() {
    // This method can be expanded to include per-frame camera updates
    // such as smooth camera movement or effects
}

void Camera::SetPosition(const XMFLOAT3& position) {
    m_position = position;
    m_viewDirty = true;
}

void Camera::SetRotation(float pitch, float yaw) {
    m_pitch = pitch;
    m_yaw = yaw;
    ClampRotation();
    m_viewDirty = true;
}

void Camera::MoveForward(float distance) {
    XMVECTOR forward = XMVector3Transform(
        XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
        XMMatrixRotationRollPitchYaw(0.0f, m_yaw, 0.0f)
    );
    
    XMVECTOR position = XMLoadFloat3(&m_position);
    position += forward * distance;
    XMStoreFloat3(&m_position, position);
    
    m_viewDirty = true;
}

void Camera::MoveRight(float distance) {
    XMVECTOR right = XMVector3Transform(
        XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
        XMMatrixRotationRollPitchYaw(0.0f, m_yaw, 0.0f)
    );
    
    XMVECTOR position = XMLoadFloat3(&m_position);
    position += right * distance;
    XMStoreFloat3(&m_position, position);
    
    m_viewDirty = true;
}

void Camera::MoveUp(float distance) {
    m_position.y += distance;
    m_viewDirty = true;
}

void Camera::Rotate(float deltaPitch, float deltaYaw) {
    m_pitch += deltaPitch;
    m_yaw += deltaYaw;
    
    ClampRotation();
    m_viewDirty = true;
}

XMMATRIX Camera::GetViewMatrix() const {
    if (m_viewDirty) {
        UpdateViewMatrix();
    }
    return m_viewMatrix;
}

XMMATRIX Camera::GetProjectionMatrix() const {
    if (m_projectionDirty) {
        UpdateProjectionMatrix();
    }
    return m_projectionMatrix;
}

XMFLOAT3 Camera::GetForward() const {
    XMVECTOR forward = XMVector3Transform(
        XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
        XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f)
    );
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, forward);
    return result;
}

XMFLOAT3 Camera::GetRight() const {
    XMVECTOR right = XMVector3Transform(
        XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
        XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f)
    );
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, right);
    return result;
}

XMFLOAT3 Camera::GetUp() const {
    XMVECTOR up = XMVector3Transform(
        XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f),
        XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f)
    );
    
    XMFLOAT3 result;
    XMStoreFloat3(&result, up);
    return result;
}

void Camera::UpdateViewMatrix() const {
    // Create rotation matrix
    XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f);
    
    // Transform camera target and up vector by rotation matrix
    XMVECTOR target = XMVector3Transform(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), rotationMatrix);
    XMVECTOR up = XMVector3Transform(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), rotationMatrix);
    
    // Create view matrix
    XMVECTOR position = XMLoadFloat3(&m_position);
    m_viewMatrix = XMMatrixLookToLH(position, target, up);
    
    m_viewDirty = false;
}

void Camera::UpdateProjectionMatrix() const {
    m_projectionMatrix = XMMatrixPerspectiveFovLH(
        m_fieldOfView,
        m_aspectRatio,
        m_nearPlane,
        m_farPlane
    );
    
    m_projectionDirty = false;
}

void Camera::ClampRotation() {
    // Keep pitch within reasonable limits to prevent gimbal lock
    m_pitch = std::max(MIN_PITCH, std::min(m_pitch, MAX_PITCH));
    
    // Normalize yaw to [0, 2π]
    while (m_yaw >= XM_2PI) {
        m_yaw -= XM_2PI;
    }
    while (m_yaw < 0.0f) {
        m_yaw += XM_2PI;
    }
}
