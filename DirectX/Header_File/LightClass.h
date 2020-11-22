#pragma once

class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetDiffuseColor(float, float, float, float);
	void SetLightDIrection(float, float, float);

	XMFLOAT4 GetDiffuseColor();
	XMFLOAT3 GetLightDirection();

private:
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_LightDirection;
};

