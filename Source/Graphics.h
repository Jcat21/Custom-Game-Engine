#pragma once


class Graphics
{
public:
    enum TextureSlot
    {
        TEXTURE_SLOT_DIFFUSE,
		TEXTURE_SLOT_NORMAL,
		TEXTURE_SLOT_TOTAL
    };

	enum ConstantBuffer
	{
		CONSTANT_BUFFER_CAMERA,
		CONSTANT_BUFFER_RENDEROBJ,
        CONSTANT_BUFFER_LIGHTING,
		CONSTANT_BUFFER_SKINNING,
	};

    class Color4
    {
    public:
        Color4()
            : r(0), g(0), b(0), a(0)
        {}
        Color4(float _r, float _g, float _b, float _a = 1.0f)
            : r(_r), g(_g), b(_b), a(_a)
        {}
        float r, g, b, a;
    };

    Graphics();
    ~Graphics();

    void InitD3D(HWND hWnd, float width, float height);
    void BeginFrame(const Color4 &clearColor);
    void EndFrame();
    void CleanD3D();

	float GetScreenWidth() const { return mScreenWidth; }
	float GetScreenHeight() const { return mScreenHeight; }
    
	ID3D11Device* GetDevice() { return mDev; }
    ID3D11DeviceContext* GetDeviceContext() { return mDevcon; }

	ID3D11Buffer* CreateGraphicsBuffer(const void *initialData, int inDataSize, D3D11_BIND_FLAG inBindFlags, D3D11_CPU_ACCESS_FLAG inCPUAccessFlags, D3D11_USAGE inUsage);
	void UploadBuffer(ID3D11Buffer *buffer, const void *data, size_t dataSize);

	bool CreateDepthStencil(int inWidth, int inHeight, ID3D11Texture2D **pDepthTexture, ID3D11DepthStencilView **pDepthView);
	ID3D11DepthStencilState *CreateDepthStencilState(bool inDepthTestEnable, D3D11_COMPARISON_FUNC inDepthComparisonFunction);

	void SetActiveTexture(int slot, ID3D11ShaderResourceView* pView);
	void SetActiveSampler(int slot, ID3D11SamplerState* pSampler);

private:
    float mScreenWidth;
    float mScreenHeight;
	
	IDXGISwapChain* mSwapchain;             // the pointer to the swap chain interface
	ID3D11Device* mDev;                     // the pointer to our Direct3D device interface
	ID3D11DeviceContext* mDevcon;           // the pointer to our Direct3D device context

	ID3D11RenderTargetView* mBackbuffer;    // the pointer to our back buffer

	ID3D11Texture2D* mDepthStencilTexture;
	ID3D11DepthStencilView* mDepthStencilView;
	ID3D11DepthStencilState* mDepthState;

	ID3D11SamplerState* mDefaultSampler;
};