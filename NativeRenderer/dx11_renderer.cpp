#include "pch.h"
#include "dx11_renderer.h"
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include "GmdlFormat.h"



DX11Renderer::DX11Renderer(HWND hwnd)
	: hwnd_(hwnd)
{
	initialize();
}

// Blender export hint: y up, -z forward (!), 0.01 scale
void DX11Renderer::importModel(const char* file) {

    Assimp::Importer importer;

    const auto scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_MakeLeftHanded | aiProcess_FlipWindingOrder | aiProcess_CalcTangentSpace);
    if (!scene) {
        printf("model import failed!\n");
        exit(1);
    }

    Model model;

    unsigned int numMeshes = scene->mRootNode->mChildren[0]->mNumMeshes;
    for (unsigned int i = 0; i < numMeshes; ++i) {
        auto idx = scene->mRootNode->mChildren[0]->mMeshes[i];
        aiMesh* mesh = scene->mMeshes[i];
        auto numUVChannels = mesh->GetNumUVChannels();
        auto hasTextureCoords = mesh->HasTextureCoords(0);
        for (unsigned int v = 0; v < mesh->mNumVertices; ++v) {
            aiVector3D vertex = mesh->mVertices[v];
            aiVector3D texCoord = mesh->mTextureCoords[0][v];
            aiVector3D normal = mesh->mNormals[v];

            GmdlVertex gmdlVertex;
            gmdlVertex.position = { vertex.x, vertex.y, vertex.z };
            gmdlVertex.uv = { texCoord.x, texCoord.y };
            gmdlVertex.normal = { normal.x, normal.y, normal.z };

            model.gmdlVertices.push_back(gmdlVertex);

            /*model.positions.push_back({ vertex.x, vertex.y, vertex.z });
            model.uvs.push_back({ texCoord.x, texCoord.y });
            model.normals.push_back({ normal.x, normal.y, normal.z });*/

        }

        for (int f = 0; f < mesh->mNumFaces; ++f) {
            aiFace face = mesh->mFaces[f];
            model.indices.push_back({ face.mIndices[0] });
            model.indices.push_back({ face.mIndices[1] });
            model.indices.push_back({ face.mIndices[2] });
        }

        // TODO handle material (maybe..)
    }

    // Now write the model data to our own gmdl file format
    FILE* gmdlFile = new FILE();
    fopen_s(&gmdlFile, "c:/dev/temp/mymodel.gmdl", "w");
    GmdlHeader gh;
    for (int i = 0; i < 4; i++) {
        gh.magic[i] = GMDL_MAGIC[i];
    }
    gh.version = GMDL_VERSION;
    gh.vertexCount = model.gmdlVertices.size();
    gh.indexCount = model.indices.size();
    gh.vertexStride = (sizeof(DirectX::XMFLOAT3)) + sizeof(DirectX::XMFLOAT2) + sizeof(DirectX::XMFLOAT3);
    gh.flags = GmdlFlags::GMDL_HAS_UVS | GmdlFlags::GMDL_HAS_NORMALS;
    gh.materialNameLength = 0;
    gh.reserved = 0;
    fwrite(&gh, sizeof(GmdlHeader), 1, gmdlFile);


   
    
}



void DX11Renderer::clean()
{
    float clearColors[] = { 1, 0, 0.3, 1 };
    ID3D11RenderTargetView* rtvs[1] = { rtv };
    ctx->OMSetRenderTargets(1, rtvs, depthStencilView);
    ctx->ClearRenderTargetView(rtv, clearColors);

    // clear our depth target as well
    ctx->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
    OutputDebugStringA("clean called in DX11 renderer!\n");
}

void DX11Renderer::render()
{
    // TODO actual rendering code...
    swapChain->Present(1, 0);
    OutputDebugStringA("render called in DX11 renderer!\n");
}

/// <summary>
/// This sets up all base DX11 components, such as 
/// device, context, swap-chain etc.
/// </summary>
void DX11Renderer::initialize()
{
    int w, h;
    GetWindowClientSize(hwnd_, w, h);
    D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_1;
    HRESULT result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevels, 1, D3D11_SDK_VERSION,
        &device_, NULL, &ctx);

    if (FAILED(result)) {
        throw std::runtime_error("CreateDevice failed\n");

    }

    UINT ql;
    device_->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UINT, 8, &ql);

    DXGI_SWAP_CHAIN_DESC scdesc;
    ZeroMemory(&scdesc, sizeof(scdesc));
    scdesc.BufferCount = 1;
    scdesc.BufferDesc.Width = w;
    scdesc.BufferDesc.Height = h;
    scdesc.Windowed = true;

    scdesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    scdesc.BufferDesc.RefreshRate.Numerator = 60;
    scdesc.BufferDesc.RefreshRate.Denominator = 1;

    scdesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    scdesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    scdesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

    scdesc.OutputWindow = hwnd_;
    scdesc.SampleDesc.Count = 1;	// 1 sample per pixel
    scdesc.SampleDesc.Quality = 0;
    scdesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    scdesc.Flags = 0;

    IDXGIDevice* pDXGIDevice = nullptr;
    result = device_->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);
    IDXGIAdapter* pDXGIAdapter = nullptr;
    result = pDXGIDevice->GetAdapter(&pDXGIAdapter);
    IDXGIFactory* pIDXGIFactory = nullptr;
    pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pIDXGIFactory);

    result = pIDXGIFactory->CreateSwapChain(device_, &scdesc, &swapChain);
    if (FAILED(result)) {
        throw std::runtime_error("error creating swapchain");
        
    }

    pIDXGIFactory->Release();
    pDXGIAdapter->Release();
    pDXGIDevice->Release();


    // Gather the debug interface
    debugger = 0;
    result = device_->QueryInterface(__uuidof(ID3D11Debug), (void**)&debugger);
    if (FAILED(result)) {
        throw std::runtime_error("debuger creation failed");
        
    }



    // Create a backbuffer
    result = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(result)) {
        throw std::runtime_error("backbuffer creation failed");
        
    }

    // Bind the backbuffer as our render target
    result = device_->CreateRenderTargetView(backBuffer, NULL, &rtv);
    if (FAILED(result)) {
        throw std::runtime_error("rtv creation failed");
        
    }

    // Create a depth/stencil buffer
    D3D11_TEXTURE2D_DESC td;
    td.Width = w;
    td.Height = h;
    td.MipLevels = 1;
    td.ArraySize = 1;
    td.Format = DXGI_FORMAT_D32_FLOAT;
    td.SampleDesc.Count = 8;
    td.SampleDesc.Quality = 0;
    td.Usage = D3D11_USAGE_DEFAULT;
    td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    td.CPUAccessFlags = 0;
    td.MiscFlags = 0;

    result = device_->CreateTexture2D(&td, 0, &depthStencilBuffer);
    if (FAILED(result)) {
        throw std::runtime_error("D S buffer creation failed");
        
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dpd;
    ZeroMemory(&dpd, sizeof(dpd));
    dpd.Flags = 0;
    dpd.Format = DXGI_FORMAT_D32_FLOAT;
    dpd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

    result = device_->CreateDepthStencilView(depthStencilBuffer, &dpd, &depthStencilView);
    if (FAILED(result)) {
        throw std::runtime_error("D S view creation failed");
    }

    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthStencilDesc.StencilEnable = FALSE;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    OutputDebugStringA("DX11 initilaized!\n");

}

Shader DX11Renderer::CompileShader(const std::wstring& filename)
{
    Shader shader;

    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
    flags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* errorBlob;

    HRESULT hr = D3DCompileFromFile(filename.c_str(), nullptr, nullptr,
        "VSMain", "vs_5_0", flags, 0, &shader.vsBlob, &errorBlob);

    device_->CreateVertexShader(shader.vsBlob->GetBufferPointer(),
        shader.vsBlob->GetBufferSize(), nullptr, &shader.vs);

    ID3DBlob* psBlob;
    hr = D3DCompileFromFile(filename.c_str(), nullptr, nullptr,
        "PSMain", "ps_5_0", flags, 0, &psBlob, &errorBlob);

    device_->CreatePixelShader(psBlob->GetBufferPointer(),
        psBlob->GetBufferSize(), nullptr, &shader.ps);



    return shader;
}


void DX11Renderer::loadGMDLFile(const char* fileName)
{
    FILE* file = new FILE[256];
    auto err = fopen_s(&file, fileName, "ro");
    GmdlHeader header;
    fread(&header, sizeof(header), 1, file);

    // Validate
    if (memcmp(header.magic, GMDL_MAGIC, 4) != 0 || header.version != GMDL_VERSION)
        throw std::runtime_error("Invalid GMDL file");

    uint32_t flags;
    fread(&flags, 4, 1, file);

    // TODO:
    // Build the input layout based on the flags (do we have normals, uvs, tangents etc.)

    std::string materialName(header.materialNameLength, '\0');
    fread((void*)materialName.data(), 1, header.materialNameLength, file);

    std::vector<uint8_t> vertexData(header.vertexCount * header.vertexStride);
    fread(vertexData.data(), 1, vertexData.size(), file);

    std::vector<uint32_t> indices(header.indexCount);
    fread(indices.data(), 1, indices.size() * sizeof(uint32_t), file);

    // Example for direct GPU upload
    
    D3D11_INPUT_ELEMENT_DESC ied[] = {
           {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
           //{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },	// same slot, but 12 bytes after the pos
           {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},        // other slot (buffer), starting at 0
           {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},

    };
    ID3D11InputLayout* inputLayout;
    Shader shader;
    auto res = const_cast<ID3D11Device*>(device_)->CreateInputLayout(ied, 3, 
        shader.vsBlob->GetBufferPointer(),
        shader.vsBlob->GetBufferSize(), &inputLayout);

    if (FAILED(res)) {
        exit(2);
    }

    Model model;

    D3D11_BUFFER_DESC bd;
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = sizeof(DirectX::XMFLOAT3) * model.positions.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    device_->CreateBuffer(&bd, NULL, &model.vertexBuffer);

    // TODO : remove this, do we even need this??
    D3D11_MAPPED_SUBRESOURCE ms;
    ctx->Map(model.vertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);
    memcpy(ms.pData, model.positions.data(), sizeof(DirectX::XMFLOAT3) * model.positions.size());
    ctx->Unmap(model.vertexBuffer, NULL);


    // UVs
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(DirectX::XMFLOAT2) * model.uvs.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA uvData;
    uvData.pSysMem = model.uvs.data();
    uvData.SysMemPitch = 0;
    uvData.SysMemSlicePitch = 0;
    device_->CreateBuffer(&bd, &uvData, &model.uvBuffer);

    // Normals
    ZeroMemory(&bd, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(DirectX::XMFLOAT3) * model.normals.size();
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    D3D11_SUBRESOURCE_DATA normalData;
    normalData.pSysMem = model.normals.data();
    normalData.SysMemPitch = 0;
    normalData.SysMemSlicePitch = 0;
    device_->CreateBuffer(&bd, &normalData, &model.normalBuffer);

    // Indices
    D3D11_BUFFER_DESC ibd;
    ZeroMemory(&ibd, sizeof(ibd));
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(unsigned int) * model.indices.size();
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = model.indices.data();
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    res = device_->CreateBuffer(&ibd, &indexData, &model.indexBuffer);

    //ctx->VSSetShader(shaders::vertexShader, 0, 0);
    //ctx->PSSetShader(shaders::pixelShader, 0, 0);

    //ctx->IASetInputLayout(inputLayout);

    //UINT uvstride = sizeof(DirectX::XMFLOAT2);
    //UINT stride = sizeof(DirectX::XMFLOAT3);
    //UINT offset = 0;
    //ctx->IASetVertexBuffers(0, 1, &model.vertexBuffer, &stride, &offset);
    //ctx->IASetVertexBuffers(1, 1, &model.uvBuffer, &uvstride, &offset);
    //ctx->IASetVertexBuffers(2, 1, &model.normalBuffer, &stride, &offset);

    //// Handle mvp matrices
    //D3D11_BUFFER_DESC mbd;
    //ZeroMemory(&mbd, sizeof(mbd));
    //mbd.Usage = D3D11_USAGE_DYNAMIC;
    //mbd.ByteWidth = sizeof(MatrixBufferType);
    //mbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //mbd.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
    //res = device_->CreateBuffer(&mbd, nullptr, &model.matrixBuffer);
    //if (FAILED(res)) {
    //    printf("matrix constant buffer creation failed\n");
    //    exit(1);
    //}

    //// Setting the rasterizer state

    //D3D11_RASTERIZER_DESC rd;
    //ZeroMemory(&rd, sizeof(rd));
    //rd.FillMode = D3D11_FILL_SOLID;
    //rd.DepthClipEnable = true;
    //rd.CullMode = D3D11_CULL_BACK;
    //rd.FrontCounterClockwise = false;
    //res = device_->CreateRasterizerState(&rd, &model.rs);
    //if (FAILED(res)) {
    //    printf("wireframe rs failed\n");
    //    exit(1);
    //}


}

void DX11Renderer::resize(int w, int h)
{
    // TODO reconstruct swap chains etc.
    // Everything that is dependant on the window size.
}


