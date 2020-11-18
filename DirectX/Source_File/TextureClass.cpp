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

	// Targa 이미지 데이터를 메모리에 로드한다.
	if (!LoadTarga(filename, height, width))
	{
		return false;
	}

	//텍스쳐의 구조체를 설정한다.
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

	//빈 텍스쳐를 생선한다.
	if (FAILED(device->CreateTexture2D(&textureDesc, NULL, &m_texture)))
	{
		return false;
	}

	//Targa 이미지 데이터의 width 사이즈를 설정
	UINT rowPitch = (width * 4) * sizeof(unsigned char);

	//Targa 이미지 데이터를 텍스처에 복사한다.
	//m_texture <- m_targaData
	deviceContext->UpdateSubresource(m_texture, 0, NULL, m_targaData, rowPitch, 0);

	//셰이더 리소스 뷰 구조체를 설정한다.
	D3D11_SHADER_RESOURCE_VIEW_DESC  srvDesc;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = 1;

	//텍스처의 셰이더 리소스 뷰를 만든다.
	if (FAILED(device->CreateShaderResourceView(m_texture, &srvDesc, &m_textureView)))
	{
		return false;
	}

	//이 텍스처에 대해 밉맵을 만든다.
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
	//targa 파일을 바이너리 모드로 파일을 연다.  
	FILE* filePtr;
	if (fopen_s(&filePtr, filename, "rb") != 0)
	{
		return false;
	}

	//파일 헤더를 읽어온다.
	TargaHeader targaFileHeader;
	unsigned int count = (unsigned int)fread(&targaFileHeader, sizeof(targaFileHeader), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	//파일 헤더에서 중요 정보를 얻어온다.
	height = (int)targaFileHeader.height;
	width = (int)targaFileHeader.width;
	int bpp = (int)targaFileHeader.bpp;

	//파일이 32bit인지 24bit인지 확인한다. 
	//뭐야 24비트면 안됨? 바로 컷해버리누 bit per pixel 인가? 화소 당 비트
	if (bpp != 32)
	{
		return false;
	}

	//32비트 이미지 데이터의 크기를 계산한다.
	int imageSize = width * height * 4;

	//targa 이미지 데이터 용 메모리를 할당한다.
	unsigned char* targaImage = new unsigned char[imageSize];
	if (!targaImage)
	{
		return false;
	}

	//targa 이미지 데이터를 읽는다.
	//fileptr로 imagesize 만큼 targaimage에 적어가며 읽는다?
	count = (unsigned int)fread(targaImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	if (fclose(filePtr) != 0)
	{
		return false;
	}

	//targa 이미지의 데이터 만큼 메모리를 할당한다.
	m_targaData = new unsigned char[imageSize];
	if (!m_targaData)
	{
		return false;
	}

	// 저장할 배열의 인덱스를 초기화
	int index = 0;

	// 이미지 데이터의 인덱스를 초기화
	int k = (width * height * 4) - (width * 4);

	//설명 보류
	for (int j = 0; j < height; j++)
	{
		for (int i = 0; i < width; i++)
		{
			m_targaData[index + 0] = targaImage[k + 2]; //빨강
 			m_targaData[index + 1] = targaImage[k + 1]; //초록
			m_targaData[index + 2] = targaImage[k + 0]; //파랑
			m_targaData[index + 3] = targaImage[k + 3]; //알파

			k += 4;
			index += 4;
		}

		k -= (width * 8);
	}

	delete[] targaImage;
	targaImage = 0;
	return true;
}
