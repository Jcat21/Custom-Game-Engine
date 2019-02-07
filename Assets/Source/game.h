#pragma once
#include "Graphics.h"
#include "assetCache.h"
#include "job.h"
#include "lights.h"


class Animation;
class Camera;
class Mesh;
class RenderObj;
class Shader;
class Skeleton;
class Texture;
class VertexBuffer;

class Game
{
public:
    Game();
    ~Game();

    void Init(HWND hWnd, float width, float height);
    void Shutdown();
	void Update(float deltaTime);
    void RenderFrame();

	void OnKeyDown(uint32_t key);
	void OnKeyUp(uint32_t key);
	bool IsKeyHeld(uint32_t key) const;

	Graphics* GetGraphics() { return &mGraphics; }

	PointLightData* AllocateLight();
	void FreeLight(PointLightData *pLight);
	void SetAmbientLight(const Vector3& color) { mLightingData.mAmbient = color; }
	const Vector3 &GetAmbientLight() const { return mLightingData.mAmbient; }

	Shader* GetShader(const std::wstring& shaderName);
	Texture* LoadTexture(const std::wstring& fileName);
	Mesh* LoadMesh(const std::wstring& fileName);
    Skeleton* LoadSkeleton(const std::wstring& fileName);
    Animation* LoadAnimation(const std::wstring& fileName);

    void AddJob(JobManager::Job* pJob) { mJobManager.AddJob(pJob); }

private:
	std::unordered_map<uint32_t, bool> m_keyIsHeld;
	Graphics mGraphics;

	Camera* mCamera;

	struct LightingData
	{
		Vector3 mAmbient;
		float pad;
		PointLightData mLightPool[MAX_POINT_LIGHTS];
	} mLightingData;
	ID3D11Buffer *mLightingBuffer;

	AssetCache<Shader> mShaderCache;
	AssetCache<Texture> mTextureCache;
    AssetCache<Mesh> mMeshCache;
    AssetCache<Skeleton> mSkeletonCache;
    AssetCache<Animation> mAnimationCache;

	std::vector<RenderObj*> mRenderObj;

    JobManager mJobManager;

	bool LoadLevel(const WCHAR* fileName);
};