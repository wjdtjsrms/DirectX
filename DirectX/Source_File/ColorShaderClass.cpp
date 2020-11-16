#include "stdafx.h"
#include "../Header_File/ColorShaderClass.h"



ColorShaderClass::ColorShaderClass()
{
}
ColorShaderClass::ColorShaderClass(const ColorShaderClass&  other)
{
}
ColorShaderClass::~ColorShaderClass()
{
}

bool ColorShaderClass::Initialize(ID3D11Device * device, HWND hwnd)
{	
	//정점 및 픽셀 쉐이더를 초기화 한다.
	WCHAR vs[] = L"./Color.vs";
	WCHAR ps[] = L"./Color.ps";
	return InitializeShader(device,hwnd,vs,ps);
}

void ColorShaderClass::Shutdown()
{
	//버텍스 및 픽셀 쉐이더와 관련된 객체를 끝낸다.
	ShutdownShader();
}

bool ColorShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatirx, XMMATRIX viewMatirx, XMMATRIX projectionMatirx)
{
	//렌더링에 사용할 쉐이더 매개 변수를 설정 한다.
	if (!SetShaderParameters(deviceContext, worldMatirx, viewMatirx, projectionMatirx))
	{
		return false;
	}

	//설정된 버퍼를 쉐이더로 렌더링 한다.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool ColorShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFileName, WCHAR *psFileName)
{
	ID3D10Blob* errorMessage = nullptr;

	//버텍스 쉐이더를 컴파일 한다.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if(FAILED(D3DCompileFromFile(vsFileName, NULL, NULL, "ColorVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		//컴파일 실패시 오류메세지를 출력
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		//컴파일 실패 아니면 걍 파일 못찾은거임
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing VS Shader File", MB_OK);
		}

		return false;
	}

	//픽셀 쉐이더를 컴파일 한다.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFileName, NULL, NULL, "ColorPixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		//컴파일 실패시 오류메세지를 출력
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		}
		//컴파일 실패 아니면 걍 파일 못찾은거임
		else
		{
			MessageBox(hwnd, psFileName, L"Missing PS Shader File", MB_OK);
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
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

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


	return true;
}

void ColorShaderClass::ShutdownShader()
{
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

void ColorShaderClass::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
{
	//reinterpret_cast: 포인터 끼리의 변환을 허용하는 캐스트 연산자.
	//에러 메시지를 출력창에 표시
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"ERROR COMPILING SHADER", shaderFilename, MB_OK);

}

bool ColorShaderClass::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatirx, XMMATRIX viewMatrix, XMMATRIX projectionMatirx)
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

	return true;
}

void ColorShaderClass::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	// 정점 입력 레이아웃을 설정한다.
	deviceContext->IASetInputLayout(m_layout);

	//픽셀이랑 정점 셰이더를 설정한다.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//그린다.
	deviceContext->DrawIndexed(indexCount, 0, 0);

}


