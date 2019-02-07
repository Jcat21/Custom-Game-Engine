#include "stdafx.h"
#include "camera.h"
#include "Graphics.h"

Camera::Camera(Graphics *graphics)
    : mGraphics(graphics)
{
	mView = Matrix4::CreateTranslation(Vector3(0.0f, 0.0f, -100.f));
#if 0
	mProj = Matrix4::CreateOrtho(mGraphics->GetScreenWidth(), mGraphics->GetScreenHeight(), 1000.0f, -1000.0f);
#else
	mProj = Matrix4::CreateRotationY(-Math::PiOver2) * Matrix4::CreateRotationZ(-Math::PiOver2) *
		Matrix4::CreatePerspectiveFOV(Math::ToRadians(70.0f),
		mGraphics->GetScreenWidth(), mGraphics->GetScreenHeight(),
		25.0f, 10000.0f);
#endif
    mCameraBuffer = mGraphics->CreateGraphicsBuffer(&mCameraData, sizeof(mCameraData), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);
}

Camera::~Camera()
{
    mCameraBuffer->Release();
}

void Camera::SetActive()
{
    mCameraData.viewProj = mView * mProj;
    Matrix4 mat = mView;
    mat.Invert();
    mCameraData.cameraPos = mat.GetTranslation();
    mGraphics->UploadBuffer(mCameraBuffer, &mCameraData, sizeof(mCameraData));
	mGraphics->GetDeviceContext()->VSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &mCameraBuffer);
	mGraphics->GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_CAMERA, 1, &mCameraBuffer);
}
