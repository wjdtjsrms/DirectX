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
	//정점 및 인덱스 버퍼를 초기화 한다.
	if (!InitializeBuffers(device))
	{
		return false;
	}

	// 이 모델의 텍스쳐를 로드한다.
	return LoadTexture(device, deviceContext, textureFilename);
}

void ModelClass::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();
}

void ModelClass::Render(ID3D11DeviceContext* deviceContext)
{
	//그리기를 준비하기 위해 그래픽 파이프 라인에 꼭지점과 인덱스 버퍼를 놓는다.
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
	//삼각형 그릴려고 하는듯
	//정점 배열의 정점 수를 설정한다.
	m_vertexCount = 3;
	//인덱스 배열의 인덱스 수를 설정한다.
	m_indexCount = 3;

	//정점 배열을 만든다.(헤더에서 만든 구조체임)
	VertexType* vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}
	//인덱스 배열을 만든다.
	//long은 음수를 저장하지 않으므로 그 범위는 0에서 4,294,967,295 (2 ^ 32 - 1) 까지이다. 
	unsigned long* indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	//정점 배열에 데이터를 설정한다.
	vertices[0].position = XMFLOAT3(-1.0f, -1.0f, 0.0f); // Bottom Left
	vertices[0].color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);

	vertices[1].position = XMFLOAT3(0.0f, 1.0f, 0.0f); // Top Middle
	vertices[1].color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);

	vertices[2].position = XMFLOAT3(1.0f, -1.0f, 0.0f); // Bottom Right
	vertices[2].color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	//인덱스 배열의 값을 설정한다.
	indices[0] = 0; // Bottom Left
	indices[1] = 1; // Top Middle
	indices[2] = 2; // Bottom Right

	//정적 정점 버퍼의 구조체를 설정한다.
	D3D11_BUFFER_DESC vertexBufferDesc;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	//sub_resource 구조에 정점 데이터에 대한 포인터를 제공
	D3D11_SUBRESOURCE_DATA vertexData;
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexbuffer)))
	{
		return false;
	}

	//정적 인덱스 버퍼의 구조체를 설정한다.
	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	//인덱스 데이터를 가르키는 보조 리소스 구조체를 작성한다.
	D3D11_SUBRESOURCE_DATA indexData;
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	if (FAILED(device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer)))
	{
		return false;
	}

	//토사구팽한다.
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
	//stride:한걸음의 폭,보폭
	//정점 버퍼의 단위와 오프셋을 설정한다.
	unsigned int stride = sizeof(VertexType);
	unsigned int offset = 0;

	//렌더링 할 수 있도록 입력 어셈블러에서 정점 버퍼를 활성화 한다.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexbuffer, &stride, &offset);
	
	//렌더링 할 수 있도록 입력 어셈블러에서 인덱스 버퍼를 활성화 한다.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//정점 버퍼로 그릴 기본형을 선택한다. 여기서는 삼각형이다.
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
