#include "stdafx.h"
#include <stdio.h>
#include "../Header_File/TextureClass.h"

TextureClass::TextureClass()
{
}
TextureClass::TextureClass(const TextureClass& ohter)
{
}
TextureClass::~TextureClass()
{
}

bool TextureClass::Initialize(ID3D11Device* device, ID3D11DeviceContext* deviceContext, char * filename)
{
	int width = 0;
	int height = 0;

	// Targa �̹��� �����͸� �޸𸮿� �ε��Ѵ�.
	if (!LoadTarga(filename, height, width))
	{
		return false;
	}

	//�ؽ����� ����ü�� �����Ѵ�.
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Height = height;
	textureDesc.Width = width;
	textureDesc.MipLevels = 0;
	textureDesc.ArraySize = 1;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;

	//�� �ؽ��ĸ� �����Ѵ�.
	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_texture)))
	{
		return false;
	}

	//Targa �̹��� �������� width ����� ����
	UINT rowPitch = (width * 4) * sizeof(unsigned char);

	//Targa �̹��� �����͸� �ؽ�ó�� �����Ѵ�.
	//m_texture <- m_targaData
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	//���̴� ���ҽ� �� ����ü�� �����Ѵ�.
	D3D11_SHADER_RESOURCE_VIEW_DESC  srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	//�ؽ�ó�� ���̴� ���ҽ� �並 �����.
	if (FAILED(device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView)))
	{
		return false;
	}

	//�� �ؽ�ó�� ���� �Ӹ��� �����.
	deviceContext->GenerateMips(m_textureView);

	delete[] m_targaData;
	m_targaData = 0;

	return true;
}

void TextureClass::Shutdown()
{
	if (m_textureView)
	{
		m_textureView->Release();
		m_textureView = 0;
	}

	if (m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}

	if (m_targaData)
	{
		delete[] m_targaData;
		m_targaData = 0;
	}
}

ID3D11ShaderResourceView * TextureClass::GetTexture()
{
	return m_textureView;
}

bool TextureClass::LoadTarga(char* filename, int& height, int& width)
{
	//targa ������ ���̳ʸ� ���� ������ ����.  
	FILE* filePtr;
	if (fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	//���� ����� �о�´�.
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(targaFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//���� ������� �߿� ������ ���´�.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	//������ 32bit���� 24bit���� Ȯ���Ѵ�. 
	//���� 24��Ʈ�� �ȵ�? �ٷ� ���ع����� bit per pixel �ΰ�? ȭ�� �� ��Ʈ
	if (bpp != 32)
	{
		return false;
	}

	//32��Ʈ �̹��� �������� ũ�⸦ ����Ѵ�.
	int imageSize = width * height * 4;

	//targa �̹��� ������ �� �޸𸮸� �Ҵ��Ѵ�.
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	//targa �̹��� �����͸� �д´�.
	//fileptr�� imagesize ��ŭ targaimage�� ����� �д´�?
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	if (fclose(filePtr) != 0)
	{
		return false;
	}

	//targa �̹����� ������ ��ŭ �޸𸮸� �Ҵ��Ѵ�.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// ������ �迭�� �ε����� �ʱ�ȭ
	int index = 0;

	// �̹��� �������� �ε����� �ʱ�ȭ
	int k = (width * height * 4) - (width * 4);

	//���� ����
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2]; //����
 			m_targaData[index + 1] = targaImage[k + 1]; //�ʷ�
			m_targaData[index + 2] = targaImage[k + 0]; //�Ķ�
			m_targaData[index + 3] = targaImage[k + 3]; //����

			k += 4;
			index += 4;
		}

		k -= (width * 8);
	}

	delete[] targaImage;
	targaImage = 0;
	return true;
}
