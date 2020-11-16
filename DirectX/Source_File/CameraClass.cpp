#include "stdafx.h"
#include "../Header_File/CameraClass.h"
CameraClass::CameraClass()
{
	m_position = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_rotation = XMFLOAT3(0.0f, 0.0f, 0.0f);
}

CameraClass::CameraClass(const CameraClass& ohter)
{
}

CameraClass::~CameraClass()
{
}

void CameraClass::SetPosition(float x, float y, float z)
{
	m_position.x = x;
	m_position.y = y;
	m_position.z = z;
}

void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotation.x = x;
	m_rotation.y = y;
	m_rotation.z = z;
}

XMFLOAT3 CameraClass::GetPosition()
{
	return m_position;
}

XMFLOAT3 CameraClass::GetRotation()
{
	return m_rotation;
}

//위치와 회전상태에 따른 뷰 행렬을 생성하는데 사용
void CameraClass::Render()
{
	//상향 벅터 , 위치 벡터, 방향 벡터 
	XMFLOAT3 up, position, lookAt;
	XMVECTOR upVector, positionVector, lookAtVector;
	float yaw, pitch, roll;
	XMMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	upVector = XMLoadFloat3(&up);

	position = m_position;
	positionVector = XMLoadFloat3(&position);

	//바라보는 방향은 엿장수 맘대로
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	//회전 값을 라디안 단위로 설정
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;

	//설정한 값으로 회전 행렬을 만든다.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//돌릴때 카메라도 같이 돌게 한다.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//바라보는 방향에다가 바라보는 위치를 더하면 드디어 진정한 방향 벡터 탄생
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//이거 다 쓰까서 뷰 행렬을 만든다.
	m_viewMatirx = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatirx)
{
	viewMatirx = m_viewMatirx;
}
