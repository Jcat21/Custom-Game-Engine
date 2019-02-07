#pragma once

class Graphics;

class VertexBuffer
{
public:
	VertexBuffer(Graphics *graphics,
		const void* vertexData, uint32_t vertexCount, uint32_t vertexStride,
		const void* indexData, uint32_t indexCount, uint32_t indexStride
		);
	~VertexBuffer();
	void SetActive() const;
	void Draw() const;

private:
	Graphics *mGraphics;
	ID3D11Buffer *mVertBuffer;
	ID3D11Buffer *mIndexBuffer;
	DXGI_FORMAT mIndexFormat;
	uint32_t mIndexCount;
	uint32_t mVertexStride;
};