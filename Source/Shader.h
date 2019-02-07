#pragma once

class Game;
class Graphics;

class Shader
{
public:
    Shader(Graphics* graphics);
    ~Shader();

    bool Load(const WCHAR* fileName, const D3D11_INPUT_ELEMENT_DESC* layoutArray, int numLayoutElements);
    void SetActive() const;

private:
    Graphics* mGraphics;
    ID3D11VertexShader* mVS;
    ID3D11PixelShader* mPS;
    ID3D11InputLayout* mLayout;
};