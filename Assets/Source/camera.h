#pragma once
#include "engineMath.h"

class Graphics;

class Camera
{
public:
	struct PerCameraConstants
	{
		Matrix4 viewProj;
        Vector3 cameraPos;
        float pad;
	};

	Camera(Graphics *graphics);
	~Camera();

	void SetActive();
	virtual void Update(float deltaTime) {}

	Matrix4 mView;
	Matrix4 mProj;

protected:
	Graphics *mGraphics;
	PerCameraConstants mCameraData;
	ID3D11Buffer *mCameraBuffer;
};
