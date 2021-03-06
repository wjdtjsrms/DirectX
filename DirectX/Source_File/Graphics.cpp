#include "stdafx.h"
#include "../Header_File/D3DClass.h"
#include "../Header_File/CameraClass.h"
#include "../Header_File/ModelClass.h"
#include "../Header_File/ColorShaderClass.h"
#include "../Header_File/TextureShaderClass.h"
#include "../Header_File/LightShaderClass.h"
#include "../Header_File/LightClass.h"
#include "../Header_File/Graphics.h"



GraphicsClass::GraphicsClass()
{
}
GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}
GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd) 
{
	//메모리 정렬
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
	{
		return false;
	}

	if (!m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCRREN, SCREEN_DEPTH, SCREEN_NEAR))
	{
		MessageBox(hwnd, L"D3D_INITIALIZE_ERROR", L"ERROR", MB_OK);
		return false;
	}

	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		MessageBox(hwnd, L"CAMERA_ERROR", L"ERROR", MB_OK);
		return false;
	}
	m_Camera->SetPosition(0.0f, 0.0f, -5.0f);

	m_Model = new ModelClass;
	if (!m_Model)
	{

		return false;
	}
	

	if (!m_Model->Initialize(m_Direct3D->GetDevice(),m_Direct3D->GetDeviceContext(), const_cast <char *> ("DirectX/Resource/stone01.tga")))
	{
		MessageBox(hwnd, L"MODEL_INITIALIZE_ERROR", L"ERROR", MB_OK);
		return false;
	}

	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}
	if (!m_ColorShader->Initialize(m_Direct3D->GetDevice(),hwnd))
	{
		MessageBox(hwnd, L"SHADER_INITIALIZE_ERROR", L"ERROR", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}
	if (!m_TextureShader->Initialize(m_Direct3D->GetDevice(),hwnd))
	{
		MessageBox(hwnd, L"TEXTURE_SHADER_INITIALIZE_ERROR", L"ERROR", MB_OK);
		return false;
	}

	m_LightShaderClass = new LightShaderClass;
	if (!m_LightShaderClass)
	{
		return false;
	}

	if (!m_LightShaderClass->Initialize(m_Direct3D->GetDevice(), hwnd))
	{
		MessageBox(hwnd, L"Light_SHADER_INITIALIZE_ERROR", L"ERROR", MB_OK);
		return false;
	}

	m_LightClass = new LightClass;
	if (!m_LightClass)
	{
		return false;
	}
	m_LightClass->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_LightClass->SetLightDIrection(0.0f, 0.0f, 1.0f);
	
	return true;
}

bool GraphicsClass::Frame()
{
	static float rotation = 0.0f;
	rotation += (float)XM_PI *0.005f;
	if (rotation > 360.0f)
	{
		rotation -= 360.0f;
	}

	return Render(rotation);
}

bool GraphicsClass::Render(float rotation)
{
	XMVECTOR lightDV = XMLoadFloat3(&m_LightClass->GetLightDirection());
	XMFLOAT3 lightDF;

	//씬을 그리기 위해 버퍼를 지운다.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_Camera->Render();

	XMMATRIX worldMatirx, viewMatrix, projectionMatirx;
	m_Direct3D->GetWorldMatrix(worldMatirx);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatirx);

	//worldMatirx = XMMatrixRotationY(rotation);

	lightDV = XMVector3TransformNormal(lightDV, XMMatrixRotationY(rotation));
	XMStoreFloat3(&lightDF, lightDV);
	m_Model->Render(m_Direct3D->GetDeviceContext());

	//if (!m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatirx, viewMatrix, projectionMatirx))
	//{
	//	return false;
	//}
	//if (!m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatirx, viewMatrix, projectionMatirx, m_Model->GetTexture()))
	//{
	//	return false;
	//}
	if (!m_LightShaderClass->Render(m_Direct3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatirx, viewMatrix, projectionMatirx, m_Model->GetTexture(), lightDF, m_LightClass->GetDiffuseColor()))
	{
		return false;
	}
	//버퍼의 내용을 화면에 출력한다.
	m_Direct3D->EndScene();

	return true;
}

void GraphicsClass::Shutdown()
{
	if (m_LightClass)
	{
		delete m_LightClass;
		m_LightClass = 0;
	}
	if (m_LightShaderClass)
	{
		m_LightShaderClass->Shutdown();
		delete m_LightShaderClass;
		m_LightShaderClass = 0;
	}
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		_aligned_free(m_Direct3D);
		m_Direct3D = 0;
	}
}