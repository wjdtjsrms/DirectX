#include "stdafx.h"
#include "../Header_File/TextureShaderClass.h"

TextureShaderClass::TextureShaderClass()
{
}
TextureShaderClass::TextureShaderClass(const TextureShaderClass& other)
{
}
TextureShaderClass::~TextureShaderClass()
{
}

bool TextureShaderClass::Initialize(ID3D11Device * device, HWND hwnd)
{
	//���� �� �ȼ� ���̴��� �ʱ�ȭ �Ѵ�.
	WCHAR vs[] = L"./Texture.vs";
	WCHAR ps[] = L"./Texture.ps";
	return InitializeShader(device, hwnd, vs, ps);
}

void TextureShaderClass::Shutdown()
{
	//���ؽ� �� �ȼ� ���̴��� ���õ� ��ü�� ������.
	ShutdownShader();
}

bool TextureShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatirx, XMMATRIX viewMatirx, XMMATRIX projectionMatirx,ID3D11ShaderResourceView* texture)
{
	//�������� ����� ���̴� �Ű� ������ ���� �Ѵ�.
	if (!SetShaderParameters(deviceContext, worldMatirx, viewMatirx, projectionMatirx,texture))
	{
		return false;
	}

	//������ ���۸� ���̴��� ������ �Ѵ�.
	RenderShader(deviceContext, indexCount);

	return true;
}

bool TextureShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFileName, WCHAR *psFileName)
{
	ID3D10Blob* errorMessage = nullptr;

	//���ؽ� ���̴��� ������ �Ѵ�.
	ID3D10Blob* vertexShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(vsFileName, NULL, NULL, "TextureVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage)))
	{
		//������ ���н� �����޼����� ���
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, vsFileName);
		}
		//������ ���� �ƴϸ� �� ���� ��ã������
		else
		{
			MessageBox(hwnd, vsFileName, L"Missing TEXTURE VS Shader File", MB_OK);
		}

		return false;
	}

	//�ȼ� ���̴��� ������ �Ѵ�.
	ID3D10Blob* pixelShaderBuffer = nullptr;
	if (FAILED(D3DCompileFromFile(psFileName, NULL, NULL, "TexturePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage)))
	{
		//������ ���н� �����޼����� ���
		if (errorMessage)
		{
			OutputShaderErrorMessage(errorMessage, hwnd, psFileName);
		}
		//������ ���� �ƴϸ� �� ���� ��ã������
		else
		{
			MessageBox(hwnd, psFileName, L"Missing TEXTURE PS Shader File", MB_OK);
		}

		return false;
	}

	//���ۿ��� ���� ���̴��� �����Ѵ�.
	if (FAILED(device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
	{
		return false;
	}
	//���ۿ��� �ȼ� ���̴��� �����Ѵ�.
	if (FAILED(device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
	{
		return false;
	}

	//���� �Է� ���̾ƿ� ����ü�� �����Ѵ�.
	//�� ������ ModelClass�� vertex shader�� VertexInputType �� ������ ��ġ�ؾ� �Ѵ�.
	D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
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

	//���� �ƿ� ������ �����´�.
	unsigned int numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	//���� �Է� ���̾ƿ��� �����.
	if (FAILED(device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_layout)))
	{
		return false;
	}

	//���� �� ���� ���۸� ��籸���Ѵ�.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	//���� ���̴��� �ִ� ��� ��� ����(MatrixBuffer)�� ����ü�� �ۼ��Ѵ�.
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// ��� ���� �����͸� ����� �� Ŭ�������� ���� ���̴� ��� ���ۿ� �����Ҽ� �ְ� �Ѵ�.
	// �� �� Ŭ�������� ���� ���� �Ҽ� �ִٴ� �� �ƴϳ�?
	if (FAILED(device->CreateBuffer(&matrixBufferDesc, NULL, &m_matirxBuffer)))
	{
		return false;
	}

	//�ؽ��� ���÷� ���¸� ���� �� �����Ѵ�.
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

void TextureShaderClass::ShutdownShader()
{
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

void TextureShaderClass::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
{
	//reinterpret_cast: ������ ������ ��ȯ�� ����ϴ� ĳ��Ʈ ������.
	//���� �޽����� ���â�� ǥ��
	OutputDebugStringA(reinterpret_cast<const char*>(errorMessage->GetBufferPointer()));

	errorMessage->Release();
	errorMessage = 0;

	MessageBox(hwnd, L"ERROR COMPILING TEXTURE SHADER", shaderFilename, MB_OK);

}

bool TextureShaderClass::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatirx, XMMATRIX viewMatrix, XMMATRIX projectionMatirx,ID3D11ShaderResourceView* texture)
{
	//HLSL�� Direct Math�� ��� ������ ���� �ʴ�. �׷��� ��ġ �ؾߵ�
	worldMatirx = XMMatrixTranspose(worldMatirx);
	viewMatrix = XMMatrixTranspose(viewMatrix);
	projectionMatirx = XMMatrixTranspose(projectionMatirx);

	//��� ������ ������ �� �� �ֵ��� ��ٴ�? �� ��׶�°ž�. ������?
	//Static Buffer�� ���� ���ΰ�? Dynamic Buffer�� ���� ���ΰ�?
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	if (FAILED(deviceContext->Map(m_matirxBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		return false;
	}

	//�� �װ���. ��� ���Ͽ� MATRIX ����ü ����� ������.
	//��� ������ �����Ϳ� ���� �����͸� �����´�.
	MatrixBufferType* dataptr = (MatrixBufferType*)mappedResource.pData;

	//��� ���ۿ� �����͸� �����Ѵ�.
	dataptr->world = worldMatirx;
	dataptr->view = viewMatrix;
	dataptr->projection = projectionMatirx;

	//��� ������ ����� Ǭ��.
	deviceContext->Unmap(m_matirxBuffer, 0);

	//���� ���̴������� ��� ������ ��ġ�� �����Ѵ�.
	unsigned bufferNumber = 0;

	//���� ���̴��� ��� ���۸� �ٲ� ������ �ٲ۴�.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matirxBuffer);

	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void TextureShaderClass::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
	// ���� �Է� ���̾ƿ��� �����Ѵ�.
	deviceContext->IASetInputLayout(m_layout);

	//�ȼ��̶� ���� ���̴��� �����Ѵ�.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	//�ȼ� ���̴����� ���÷� ���¸� �����Ѵ�.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	//�׸���.
	deviceContext->DrawIndexed(indexCount, 0, 0);

}