#pragma once
#include "pch.h"
#include "NativeRenderer.h"


struct Shader;

class DX11Renderer : public NativeRenderer {

public:

	DX11Renderer(HWND hwnd);
	void importModel(const char* modelFilePath) override;
	void clean() override;
	void render() override;

private:
	void initialize();
	Shader CompileShader(const std::wstring& filename);
	void loadGMDLFile(const char* fileName);

private:
	HWND hwnd_;
	Shader* uberShader_ = nullptr;


	ID3D11Device* device_;
	ID3D11DeviceContext* ctx;
	IDXGISwapChain* swapChain;
	ID3D11Debug* debugger;
	ID3D11Texture2D* backBuffer;
	ID3D11RenderTargetView* rtv;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;


	// Inherited via NativeRenderer
	void resize(int w, int h) override;

};

struct GmdlVertex;
struct Model {
	
	
	std::vector<GmdlVertex> gmdlVertices;

	// Legacy
	std::vector<DirectX::XMFLOAT3> positions;
	std::vector<DirectX::XMFLOAT2> uvs;
	std::vector<DirectX::XMFLOAT3> normals;
	std::vector<unsigned int> indices;
	// End legacy



	ID3D11Buffer* matrixBuffer = nullptr;
	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* uvBuffer = nullptr;
	ID3D11Buffer* normalBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;
	ID3D11Texture2D* tex = nullptr;
	ID3D11SamplerState* samplerState = nullptr;
	ID3D11ShaderResourceView* srv = nullptr;
	ID3D11RasterizerState* rs = nullptr;
};

struct Shader
{
	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;
	ID3DBlob* vsBlob = nullptr; // Keep this around for InputLayout creation
};

struct GmdlVertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 normal;  // optional
	DirectX::XMFLOAT2 uv;      // optional
	DirectX::XMFLOAT3 tangent; // optional
};
