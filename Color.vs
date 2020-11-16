//Global
cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatirx;
	matrix projectionMatrix;
}

//TypeDefs
struct VertexInputType
{
	float4 position: POSITION;
	float4 color : COLOR;
};
struct PixelInputType
{
	float4 position :SV_POSITION;
	float4 color : COLOR;
};

//Vertex Shader
PixelInputType ColorVertexShader(VertexInputType input)
{
	PixelInputType output;

	//이게 1이면 선이던가? 아핀 변환 에서 본거 같은데..?
	//적절한 행렬 계산을 위해 위치 벡터를 4단위로 변경
	input.position.w = 1.0f;
										
	output.position = mul(input.position,worldMatrix);
	output.position = mul(output.position,viewMatirx);
	output.position = mul(output.position,projectionMatrix);

	output.color = input.color;

	return output;
}