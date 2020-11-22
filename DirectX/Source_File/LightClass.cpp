#include "stdafx.h"
#include "../Header_File/LightClass.h"

LightClass::LightClass()
{
}
LightClass::LightClass(const LightClass& other)
{
}

LightClass::~LightClass()
{
}

void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = XMFLOAT4(red, green, blue, alpha);
}

void LightClass::SetLightDIrection(float x, float y, float z)
{
	m_LightDirection = XMFLOAT3(x, y, z);
}

XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}

XMFLOAT3 LightClass::GetLightDirection()
{
	return m_LightDirection;
}
