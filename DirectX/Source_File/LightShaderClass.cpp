#include "stdafx.h"
#include "../Header_File/LightShaderClass.h"


LightShaderClass::LightShaderClass()
{
}
LightShaderClass::LightShaderClass(const LightShaderClass& other)
{
}

LightShaderClass::~LightShaderClass()
{
}
bool LightShaderClass::Initialize(ID3D11Device * device, HWND hwnd)
{
	//정점 및 픽셀 쉐이더를 초기화 한다.
	WCHAR vs[] = L"./Light.vs";
	WCHAR ps[] = L"./Light.ps";
	return InitializeShader(device, hwnd, vs, ps);
}

void LightShaderClass::Shutdown()
{
	//버텍스 및 픽셀 쉐이더와 관련된 객체를 끝낸다.
	ShutdownShader();
}

bool LightShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatirx, XMMATRIX viewMatirx, XMMATRIX projectionMatirx, ID3D11ShaderResourceView* texture, XMFLOAT3 lightDirectoin, XMFLOAT4 diffuseColor)
{
	//렌더링에 사용할 쉐이더 매개 변수를 설정 한다.
	if (!SetShaderParameters(deviceContext, worldMatirx, viewMatirx, projectionMatirx, texture, lightDirectoin, diffuseColor))
	{
		return false;
	}

	//설정된 버퍼를 쉐이더로 렌더링 한다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool LightShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFileName, WCHAR *psFileName)
{
	ID3D10Blob* errorMessage = nullptr;

	//버텍스 쉐이더를 컴파일 한다.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFileName, NULL, NULL, "LightVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		//컴파일 실패시 오류메세지를 출력
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		//컴파일 실패 아니면 걍 파일 못찾은거임
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing Light VS Shader File", MB_OK);
		}

		return false;
	}

	//픽셀 쉐이더를 컴파일 한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFileName, NULL, NULL, "LightPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		//컴파일 실패시 오류메세지를 출력
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		}
		//컴파일 실패 아니면 걍 파일 못찾은거임
		else
		{
			MessageBox(hwnd, psFileName, L"Missing Light PS Shader File", MB_OK);
		}

		return false;
	}

	//버퍼에서 정점 쉐이더를 생성한다.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
	{
		return false;
	}
	//버퍼에서 픽셀 쉐이더를 생서한다.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
	{
		return false;
	}

	//정점 입력 레이아웃 구조체를 설정한다.
	//이 설정은 ModelClass와 vertex shader의 VertexInputType 의 설정과 일치해야 한다.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "TEXCOORD";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "NORMAL";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	//레이 아웃 갯수를 가져온다.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//정점 입력 레이아웃을 만든다.
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	//이제 안 쓰는 버퍼를 토사구팽한다.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//정점 쉐이더에 있는 행렬 상수 버퍼(MatrixBuffer)의 구조체를 작성한다.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// 상수 버퍼 포인터를 만들어 이 클래스에서 정점 쉐이더 상수 버퍼에 접근할수 있게 한다.
	// 걍 이 클래스에서 값을 변경 할수 있다는 말 아니냐?
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matirxBuffer)))
	{
		return false;
	}

	//픽셀 쉐이더에 있는 광원 동적 상수 버퍼(LightBufferType)의 구조체를 작성한다.
	D3D11_BUFFER_DESC lightBufferDesc;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;

	if (FAILED(device->CreateBuffer(&lightBufferDesc, NULL, &m_lightBuffer)))
	{
		return false;
	}

	//텍스쳐 셈플러 상태를 생성 및 설정한다.
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	if (FAILED(device->CreateSamplerState(&samplerDesc, &m_sampleState)))
	{
		return false;
	}

	return true;
}

void LightShaderClass::ShutdownShader()
{
	if (m_lightBuffer)
	{
		m_lightBuffer->Release();
		m_lightBuffer = 0;
	}

	if (m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	if (m_matirxBuffer)
	{
		m_matirxBuffer->Release();
		m_matirxBuffer = 0;
	}

	if (m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	if (m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	if (m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}
}

void LightShaderClass::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
{
	//reinterpret_cast: 포인터 끼리의 변환을 허용하는 캐스트 연산자.
	//에러 메시지를 출력창에 표시
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"ERROR COMPILING TEXTURE SHADER", shaderFilename, MB_OK);

}

bool LightShaderClass::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatirx, XMMATRIX viewMatrix, XMMATRIX projectionMatirx, ID3D11ShaderResourceView* texture,XMFLOAT3 lightDirection, XMFLOAT4 diffuseColor)
{
	//HLSL과 Direct Math의 행렬 방향은 같지 않다. 그래서 전치 해야됨
	worldMatirx = XMMatrixTranspose(worldMatirx);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatirx = XMMatrixTranspose(projectionMatirx);

	//상수 버퍼의 내용을 쓸 수 있도록 잠근다? 뭘 잠그라는거야. 가스불?
	//Static Buffer를 만들 것인가? Dynamic Buffer를 만들 것인가?
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matirxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	//얘 그거임. 헤더 파일에 MATRIX 구조체 만들어 놓은거.
	//상수 버퍼의 데이터에 대한 포인터를 가져온다.
	MatrixBufferType* dataptr = (MatrixBufferType*)mappedResource.pData;

	//상수 버퍼에 데이터를 복사한다.
	dataptr->world = worldMatirx;
	dataptr->view = viewMatrix;
	dataptr->projection = projectionMatirx;

	//상수 버퍼의 잠금을 푼다.
	deviceContext->Unmap(m_matirxBuffer, 0);

	//정점 셰이더에서의 상수 버퍼의 위치를 설정한다.
	unsigned bufferNumber = 0;

	//정점 쉐이더의 상수 버퍼를 바뀐 값으로 바꾼다.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matirxBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	if (FAILED(deviceContext->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}
	LightBufferType* dataptr2 = (LightBufferType*)mappedResource.pData;
	dataptr2->diffuseColor = diffuseColor;
	dataptr2->lightDirection = lightDirection;
	dataptr2->padding = 0.0f;

	deviceContext->Unmap(m_lightBuffer, 0);

	bufferNumber = 0;

	deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_lightBuffer);

	return true;
}

void LightShaderClass::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	//픽셀이랑 정점 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//픽셀 쉐이더에서 샘플러 상태를 설정한다.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

}