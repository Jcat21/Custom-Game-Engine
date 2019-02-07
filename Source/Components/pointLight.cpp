#include "stdafx.h"
#include "components\pointLight.h"
#include "game.h"
#include "jsonUtil.h"
#include "renderObj.h"


PointLight::PointLight(RenderObj *pObj)
    : Component(pObj)
{
    mLight = mObj->GetGame()->AllocateLight();
}

PointLight::~PointLight() 
{
    mObj->GetGame()->FreeLight(mLight);
}

void PointLight::LoadProperties(const rapidjson::Value& properties)
{
    Component::LoadProperties(properties);
    if (nullptr != mLight)
    {
        GetVectorFromJSON(properties, "diffuseColor", mLight->mDiffuseColor);
        GetVectorFromJSON(properties, "specularColor", mLight->mSpecularColor);
        GetFloatFromJSON(properties, "specularPower", mLight->mSpecularPower);
        GetFloatFromJSON(properties, "innerRadius", mLight->mInnerRadius);
        GetFloatFromJSON(properties, "outerRadius", mLight->mOuterRadius);
        mLight->mPosition = mObj->mObjectData.modelToWorld.GetTranslation();
    }
}
