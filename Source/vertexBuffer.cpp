#include "stdafx.h"
#include "vertexBuffer.h"
#include "Graphics.h"

VertexBuffer::VertexBuffer(Graphics *graphics,
	const void* vertData, uint32_t vertCount, uint32_t vertStride,
	const void* indexData, uint32_t indexCount, uint32_t indexStride)
	: mGraphics(graphics)
	, mIndexCount(indexCount)
	, mVertexStride(vertStride)
{
	mIndexFormat = DXGI_FORMAT_UNKNOWN;
	if (indexStride == sizeof(uint8_t))
		mIndexFormat = DXGI_FORMAT_R8_UINT;
	else if (indexStride == sizeof(uint16_t))
		mIndexFormat = DXGI_FORMAT_R16_UINT;
	DbgAssert(DXGI_FORMAT_UNKNOWN != mIndexFormat, "Invalid index format");

	mVertBuffer = mGraphics->CreateGraphicsBuffer(vertData, vertCount * vertStride, D3D11_BIND_VERTEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
	mIndexBuffer = mGraphics->CreateGraphicsBuffer(indexData, indexCount * indexStride, D3D11_BIND_INDEX_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

VertexBuffer::~VertexBuffer()
{
	mVertBuffer->Release();
	mIndexBuffer->Release();
}

void VertexBuffer::SetActive() const
{
	ID3D11DeviceContext* pDevCon = mGraphics->GetDeviceContext();
	UINT stride = mVertexStride;
	UINT offset = 0;
	pDevCon->IASetVertexBuffers(0, 1, &mVertBuffer, &stride, &offset);
	pDevCon->IASetIndexBuffer(mIndexBuffer, mIndexFormat, 0);
}

void VertexBuffer::Draw() const
{
	ID3D11DeviceContext* pDevCon = mGraphics->GetDeviceContext();
	pDevCon->DrawIndexed(mIndexCount, 0, 0);
}