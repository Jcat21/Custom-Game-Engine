#include "stdafx.h"
#include "camera.h"
#include "components\followCam.h"
#include "game.h"
#include "jsonUtil.h"
#include "renderObj.h"
#include "stringUtil.h"


FollowCam::FollowCam(RenderObj *pObj)
    : Component(pObj)
{
	Matrix4 mat = mObj->mObjectData.modelToWorld;
	mat = Matrix4::CreateRotationX(Math::PiOver2) * mat;	//mrwTODO this 90 rotation accounts for the orientation of the model :(
}

void FollowCam::LoadProperties(const rapidjson::Value& properties)
{
    Component::LoadProperties(properties);
    GetVectorFromJSON(properties, "offset", mOffset);
}

void FollowCam::Update(float deltaTime)
{
    Camera* pCamera = mObj->GetGame()->GetCamera();
    Matrix4 mat = Matrix4::CreateTranslation(mObj->mObjectData.modelToWorld.GetTranslation() + mOffset);
    mat.Invert();
    pCamera->mView = mat;
}