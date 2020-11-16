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

//��ġ�� ȸ�����¿� ���� �� ����� �����ϴµ� ���
void CameraClass::Render()
{
	//���� ���� , ��ġ ����, ���� ���� 
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

	//�ٶ󺸴� ������ ����� �����
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;
	lookAtVector = XMLoadFloat3(&lookAt);

	//ȸ�� ���� ���� ������ ����
	pitch = m_rotation.x * 0.0174532925f;
	yaw = m_rotation.y * 0.0174532925f;
	roll = m_rotation.z * 0.0174532925f;

	//������ ������ ȸ�� ����� �����.
	rotationMatrix = XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	//������ ī�޶� ���� ���� �Ѵ�.
	lookAtVector = XMVector3TransformCoord(lookAtVector, rotationMatrix);
	upVector = XMVector3TransformCoord(upVector, rotationMatrix);

	//�ٶ󺸴� ���⿡�ٰ� �ٶ󺸴� ��ġ�� ���ϸ� ���� ������ ���� ���� ź��
	lookAtVector = XMVectorAdd(positionVector, lookAtVector);

	//�̰� �� ��� �� ����� �����.
	m_viewMatirx = XMMatrixLookAtLH(positionVector, lookAtVector, upVector);

}

void CameraClass::GetViewMatrix(XMMATRIX& viewMatirx)
{
	viewMatirx = m_viewMatirx;
}
