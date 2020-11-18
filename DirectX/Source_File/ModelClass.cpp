#include "stdafx.h"
#include "../Header_File/TextureClass.h"
#include "../Header_File/ModelClass.h"

ModelClass::ModelClass()
{
}

ModelClass::ModelClass(const ModelClass &ohter)
{
}

ModelClass::~ModelClass()
{
}

//bool ModelClass::Initialize(ID3D11Device* device)
//{
//	return InitializeBuffers(device);
//}

bool ModelClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	//���� �� �ε��� ���۸� �ʱ�ȭ �Ѵ�.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// �� ���� �ؽ��ĸ� �ε��Ѵ�.
	return LoadTexture(device, deviceContext, textureFilename);
}

void ModelClass::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//�׸��⸦ �غ��ϱ� ���� �׷��� ������ ���ο� �������� �ε��� ���۸� ���´�.
	RenderBuffers(deviceContext);
}

int ModelClass::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* ModelClass::GetTexture()
{
	return m_Texture->GetTexture();
}

bool ModelClass::InitializeBuffers(ID3D11Device* device)
{
	//�ﰢ�� �׸����� �ϴµ�
	//���� �迭�� ���� ���� �����Ѵ�.
	m_vertexCount = 3;
	//�ε��� �迭�� �ε��� ���� �����Ѵ�.
	m_indexCount = 3;

	//���� �迭�� �����.(������� ���� ����ü��)
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}
	//�ε��� �迭�� �����.
	//long�� ������ �������� �����Ƿ� �� ������ 0���� 4,294,967,295 (2 ^ 32 - 1) �����̴�. 
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//���� �迭�� �����͸� �����Ѵ�.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // Bottom Left
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // Top Middle
	vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // Bottom Right
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	//�ε��� �迭�� ���� �����Ѵ�.
	indices[0] = 0; // Bottom Left
	indices[1] = 1; // Top Middle
	indices[2] = 2; // Bottom Right

	//���� ���� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//sub_resource ������ ���� �����Ϳ� ���� �����͸� ����
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexbuffer)))
	{
		return false;
	}

	//���� �ε��� ������ ����ü�� �����Ѵ�.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//�ε��� �����͸� ����Ű�� ���� ���ҽ� ����ü�� �ۼ��Ѵ�.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	//��籸���Ѵ�.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;


	return true;
}


void ModelClass::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	if (m_vertexbuffer)
	{
		m_vertexbuffer->Release();
		m_vertexbuffer = 0;
	}
}

void ModelClass::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	//stride:�Ѱ����� ��,����
	//���� ������ ������ �������� �����Ѵ�.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	//������ �� �� �ֵ��� �Է� ��������� ���� ���۸� Ȱ��ȭ �Ѵ�.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexbuffer, &stride, &offset);
	
	//������ �� �� �ֵ��� �Է� ��������� �ε��� ���۸� Ȱ��ȭ �Ѵ�.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//���� ���۷� �׸� �⺻���� �����Ѵ�. ���⼭�� �ﰢ���̴�.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

bool ModelClass::LoadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char* textureFilename)
{
	m_Texture = new TextureClass;
	if (!m_Texture)
	{
		return false;
	}

	return m_Texture->Initialize(device, deviceContext, textureFilename);
}

void ModelClass::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}
}
