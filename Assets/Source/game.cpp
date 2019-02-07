#include "stdafx.h"
#include "game.h"
#include "animation.h"
#include "camera.h"
#include "engineMath.h"
#include "Graphics.h"
#include "jsonUtil.h"
#include "mesh.h"
#include "Profiler.h"
#include "renderCube.h"
#include "Shader.h"
#include "skinnedObj.h"
#include "stringUtil.h"
#include "texture.h"
#include "vertexBuffer.h"
#include "Components\pointLight.h"
#include "Components\character.h"
#include "rapidjson\include\rapidjson\rapidjson.h"
#include "rapidjson\include\rapidjson\document.h"
#include <fstream>
#include <sstream>

#define SKINNED 1

struct VertexPosColor
{
    Vector3 pos;
    Graphics::Color4 color;
};

struct VertexPosNormUV
{
	Vector3 pos;
	Vector3 norm;
	Vector2 uv;
};

struct VertexPosNormColorUV
{
	Vector3 pos;
	Vector3 norm;
	Graphics::Color4 color;
	Vector2 uv;
};

struct VertexSkin
{
    Vector3 pos;
    Vector3 norm;
    uint8_t boneIndex[4];
    uint8_t boneWeight[4];
    Vector2 uv;
};

Game::Game()
	: mCamera(nullptr)
	, mLightingBuffer(nullptr)
	, mShaderCache(this)
	, mTextureCache(this)
	, mMeshCache(this)
    , mSkeletonCache(this)
    , mAnimationCache(this)
{
}

Game::~Game()
{
}

void Game::Init(HWND hWnd, float width, float height)
{
    mJobManager.Begin();
	mGraphics.InitD3D(hWnd, width, height);
	mCamera = new Camera(GetGraphics());

    {   // load the simple shader
        D3D11_INPUT_ELEMENT_DESC simpleLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosColor, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        Shader* pShader = new Shader(&mGraphics);
		pShader->Load(L"Shaders/Mesh.hlsl", simpleLayout, 2);
		mShaderCache.Cache(L"Mesh", pShader);
	}

	{   // load the BasicMesh shader
		D3D11_INPUT_ELEMENT_DESC meshLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormColorUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormColorUV, norm), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(VertexPosNormColorUV, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormColorUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		Shader* pShader = new Shader(&mGraphics);
		pShader->Load(L"Shaders/BasicMesh.hlsl", meshLayout, 4);
		mShaderCache.Cache(L"BasicMesh", pShader);
	}

    {   // load the Unlit shader
        D3D11_INPUT_ELEMENT_DESC meshLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormUV, norm), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        Shader* pShader = new Shader(&mGraphics);
        pShader->Load(L"Shaders/Unlit.hlsl", meshLayout, 3);
        mShaderCache.Cache(L"Unlit", pShader);
    }
    
    {   // load the Phong shader
		D3D11_INPUT_ELEMENT_DESC meshLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormUV, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosNormUV, norm), D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexPosNormUV, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		Shader* pShader = new Shader(&mGraphics);
		pShader->Load(L"Shaders/Phong.hlsl", meshLayout, 3);
		mShaderCache.Cache(L"Phong", pShader);
	}

    {   // load the Skinned shader
        D3D11_INPUT_ELEMENT_DESC meshLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexSkin, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexSkin, norm), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "BONES", 0, DXGI_FORMAT_R8G8B8A8_UINT, 0, offsetof(VertexSkin, boneIndex), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "WEIGHTS", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, offsetof(VertexSkin, boneWeight), D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(VertexSkin, uv), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        Shader* pShader = new Shader(&mGraphics);
        pShader->Load(L"Shaders/Skinned.hlsl", meshLayout, 5);
        mShaderCache.Cache(L"Skinned", pShader);
    }

#if 0
    {   // create a simple triangle
        VertexPosColor triangle[] =
        {
            { Vector3(0.0f, 0.5f, 0.0f),	Graphics::Color4(1.0f, 0.0f, 0.0f, 1.0f) },
            { Vector3(0.45f, -0.5, 0.0f),	Graphics::Color4(0.0f, 1.0f, 0.0f, 1.0f) },
            { Vector3(-0.45f, -0.5f, 0.0f), Graphics::Color4(0.0f, 0.0f, 1.0f, 1.0f) }
        };
        uint16_t triangleIndex[] =
        { 0, 1, 2 };

        // create the vertex buffer
        VertexBuffer* pVertBuff = new VertexBuffer(&mGraphics,
            triangle, sizeof(triangle), sizeof(triangle[0]),
            triangleIndex, sizeof(triangleIndex), sizeof(triangleIndex[0])
        );
        Mesh *pMesh = new Mesh(this, pVertBuff, GetShader(L"Mesh"));
        RenderObj* pObj = new RenderObj(this, pMesh);
        mMeshCache.Cache(L"Triangle", pMesh);
        mRenderObj.push_back(pObj);
#if 0
        pObj = new RenderCube(this, GetShader(L"BasicMesh"), LoadTexture(L"Assets/Textures/Platform.png"));
        mRenderObj.push_back(pObj);
#else
#if 0
        pObj = new RenderObj(this, LoadMesh(L"Assets/Meshes/Cube.itpmesh2"));
#else
        pObj = new RenderObj(this, LoadMesh(L"Assets/Meshes/PlayerShip.itpmesh2"));
#endif
        mRenderObj.push_back(pObj);
#endif

        //		pMesh = Mesh::StaticLoad(L"Assets/Meshes/PlayerShip.itpmesh2", this);
                //mMeshCache.Cache(L"ship", pMesh);
                //pObj = new RenderObj(this, pMesh);
                //mRenderObj.push_back(pObj);
    }
#else
    LoadLevel(L"Assets/Levels/Level07.itplevel");
#endif

    {
		mLightingBuffer = mGraphics.CreateGraphicsBuffer(&mLightingData, sizeof(mLightingData), D3D11_BIND_CONSTANT_BUFFER, D3D11_CPU_ACCESS_WRITE, D3D11_USAGE_DYNAMIC);

#if 0
		{   //create a point light
			PointLightData *pLight = AllocateLight();
			if (nullptr != pLight)
			{
				pLight->mDiffuseColor = Vector3(1.0f, 1.0f, 1.0f);
				pLight->mSpecularColor = Vector3(1.0f, 1.0f, 1.0f);
				pLight->mPosition = Vector3(-100.0f, 0.0f, 50.0f);
				pLight->mSpecularPower = 10.0f;
				pLight->mInnerRadius = 100.0f;
				pLight->mOuterRadius = 200.0f;
			}
		}
		{   //create a point light
			PointLightData *pLight = AllocateLight();
			if (nullptr != pLight)
			{
				pLight->mDiffuseColor = Vector3(1.0f, 1.0f, 1.0f);
				pLight->mSpecularColor = Vector3(1.0f, 1.0f, 1.0f);
				pLight->mPosition = Vector3(100.0f, -50.0f, 0.0f);
				pLight->mSpecularPower = 10.0f;
				pLight->mInnerRadius = 100.0f;
				pLight->mOuterRadius = 200.0f;
			}
		}
		// set the ambient light
		SetAmbientLight(Vector3(0.1f, 0.1f, 0.1f));
#endif
    }
}

class TestJob : public JobManager::Job
{
public:
    void DoIt() override {
        printf("Hello World\n");
    }
};
static TestJob s_testJob;

void Game::Shutdown()
{
    mJobManager.End();
	for (RenderObj* pObj : mRenderObj)
		delete pObj;
	mRenderObj.clear();
	mLightingBuffer->Release();
	delete mCamera;
    mAnimationCache.Clear();
    mSkeletonCache.Clear();
	mMeshCache.Clear();
	mTextureCache.Clear();
	mShaderCache.Clear();
	mGraphics.CleanD3D();
}

static float s_angle = 0.0f;
void Game::Update(float deltaTime)
{
    mJobManager.AddJob(&s_testJob);
    mCamera->Update(deltaTime);
#if 0
    s_angle += 1.0f * deltaTime;
	mRenderObj[0]->mObjectData.modelToWorld = Matrix4::CreateRotationZ(s_angle) 
		* Matrix4::CreateScale(300.0f);
	mRenderObj[1]->mObjectData.modelToWorld = Matrix4::CreateRotationZ(s_angle)
		* Matrix4::CreateRotationY(0.5f * Math::PiOver2)
//		* Matrix4::CreateScale(100.0f);
		* Matrix4::CreateScale(2.0f);
#else
    for (RenderObj* pObj : mRenderObj)
        pObj->Update(deltaTime);
#endif
}

void Game::RenderFrame()
{
    {
        PROFILE_SCOPE(WaitForJobs);
        mJobManager.WaitForJobs();
    }

	// clear the back buffer to a deep blue
	static const Graphics::Color4 s_clearColor(0.0f, 0.2f, 0.4f, 1.0f);
	mGraphics.BeginFrame(s_clearColor);

    // draw a triangle with the simple shader
	mCamera->SetActive();
	mGraphics.UploadBuffer(mLightingBuffer, &mLightingData, sizeof(mLightingData));
	mGraphics.GetDeviceContext()->PSSetConstantBuffers(Graphics::CONSTANT_BUFFER_LIGHTING, 1, &mLightingBuffer);
	for (RenderObj* pObj : mRenderObj)
	{
		pObj->Draw();
	}

    // switch the back buffer and the front buffer
    mGraphics.EndFrame();
}

void Game::OnKeyDown(uint32_t key)
{
	m_keyIsHeld[key] = true;
}

void Game::OnKeyUp(uint32_t key)
{
	m_keyIsHeld[key] = false;
}

bool Game::IsKeyHeld(uint32_t key) const
{
	const auto find = m_keyIsHeld.find(key);
	if (find != m_keyIsHeld.end())
		return find->second;
	return false;
}

PointLightData* Game::AllocateLight()
{
	for (int i = 0; i < MAX_POINT_LIGHTS; ++i)
	{
		if (false == mLightingData.mLightPool[i].mIsEnabled)
		{
			mLightingData.mLightPool[i].mIsEnabled = true;
			return &mLightingData.mLightPool[i];
		}
	}
	return nullptr;
}

void Game::FreeLight(PointLightData *pLight)
{
#ifdef _DEBUG
	std::ptrdiff_t index = pLight - mLightingData.mLightPool;
	DbgAssert(index >= 0 && index < MAX_POINT_LIGHTS, "trying to free a light that didn't come from the pool");
#endif
	pLight->mIsEnabled = false;
}

bool Game::LoadLevel(const WCHAR* fileName)
{
	std::ifstream file(fileName);
	if (!file.is_open())
	{
		return false;
	}

	std::stringstream fileStream;
	fileStream << file.rdbuf();
	std::string contents = fileStream.str();
	rapidjson::StringStream jsonStr(contents.c_str());
	rapidjson::Document doc;
	doc.ParseStream(jsonStr);

	if (!doc.IsObject())
	{
		return false;
	}

	std::string str = doc["metadata"]["type"].GetString();
	int ver = doc["metadata"]["version"].GetInt();

	// Check the metadata
	if (!doc["metadata"].IsObject() ||
		str != "itplevel" ||
		ver != 2)
	{
		return false;
	}

#if 1
	// Load camera data
	const rapidjson::Value& cameraData = doc["camera"];
	if (cameraData.IsObject())
	{
		Vector3 pos;
		GetVectorFromJSON(cameraData, "position", pos);
		Quaternion rot;
		GetQuaternionFromJSON(cameraData, "rotation", rot);
		Matrix4 camMat = Matrix4::CreateFromQuaternion(rot)
			* Matrix4::CreateTranslation(pos);
		camMat.Invert();
		mCamera->mView = camMat;
	}
#endif

#if 1
	// Load light data
	const rapidjson::Value& lightingData = doc["lightingData"];
	if (!lightingData.IsObject())
	{
		return false;
	}

	GetVectorFromJSON(lightingData, "ambient", mLightingData.mAmbient);
#if 0
	const rapidjson::Value& pointLights = lightingData["pointLights"];
	if (pointLights.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < pointLights.Size(); ++i)
		{
			PointLightData* pLight = AllocateLight();
			GetVectorFromJSON(pointLights[i], "diffuseColor", pLight->mDiffuseColor);
			GetVectorFromJSON(pointLights[i], "specularColor", pLight->mSpecularColor);
			GetVectorFromJSON(pointLights[i], "position", pLight->mPosition);
			GetFloatFromJSON(pointLights[i], "specularPower", pLight->mSpecularPower);
			GetFloatFromJSON(pointLights[i], "innerRadius", pLight->mInnerRadius);
			GetFloatFromJSON(pointLights[i], "outerRadius", pLight->mOuterRadius);
		}
	}
#endif
#endif

#if 1
	const rapidjson::Value& renderObjects = doc["renderObjects"];
	if (renderObjects.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < renderObjects.Size(); ++i)
		{
			std::string meshstr;
			GetStringFromJSON(renderObjects[i], "mesh", meshstr);
			std::wstring meshwstr;
			StringUtil::String2WString(meshwstr, meshstr);
			Mesh* pMesh = LoadMesh(meshwstr.c_str());
            RenderObj *pObj = nullptr;
#if SKINNED
            SkinnedObj *pSkinObj = nullptr;
#endif
            if (nullptr != pMesh)
			{
#if SKINNED
                if (pMesh->IsSkinned())
                    pObj = pSkinObj = new SkinnedObj(this, pMesh);
				else
#endif
					pObj = new RenderObj(this, pMesh);
            }
            else
            {
                pObj = new RenderObj(this, nullptr);
            }
            Vector3 pos = Vector3::Zero;
			GetVectorFromJSON(renderObjects[i], "position", pos);
			Quaternion rot = Quaternion::Identity;
			GetQuaternionFromJSON(renderObjects[i], "rotation", rot);
			float scale = 1.0f;
			GetFloatFromJSON(renderObjects[i], "scale", scale);
			Matrix4 rotMat = Matrix4::CreateRotationY(Math::ToRadians(-120.0f))
				* Matrix4::CreateRotationZ(Math::ToRadians(-90.0f));
			pObj->mObjectData.modelToWorld = Matrix4::CreateScale(scale)
				* Matrix4::CreateFromQuaternion(rot)
				* Matrix4::CreateTranslation(pos);
			mRenderObj.push_back(pObj);

#if 1
            const rapidjson::Value& components = renderObjects[i]["components"];
            if (components.IsArray())
            {
                for (rapidjson::SizeType componentIndex = 0; componentIndex < components.Size(); ++componentIndex)
                {
                    std::string compType;
                    GetStringFromJSON(components[componentIndex], "type", compType);
                    if (compType == "PointLight")
                    {
                        PointLight* pLight = new PointLight(pObj);
                        pLight->LoadProperties(components[componentIndex]);
                        pObj->AddComponent(pLight);
                    }
#if 1
					else if (compType == "Character")
					{
                        if (nullptr != pSkinObj)
                        {
                            Character* pChar = new Character(pSkinObj);
                            pChar->LoadProperties(components[componentIndex]);
                            pObj->AddComponent(pChar);
                        }
					}
#endif
				}
            }
#endif
		}
	}
#endif
	return true;
}

Shader* Game::GetShader(const std::wstring& shaderName)
{
	return mShaderCache.Get(shaderName);
}

Texture* Game::LoadTexture(const std::wstring& fileName)
{
	return mTextureCache.Load(fileName);
}

Mesh* Game::LoadMesh(const std::wstring& fileName)
{
	return mMeshCache.Load(fileName);
}

Skeleton* Game::LoadSkeleton(const std::wstring& fileName)
{
    return mSkeletonCache.Load(fileName);
}

Animation* Game::LoadAnimation(const std::wstring& fileName)
{
    return mAnimationCache.Load(fileName);
}