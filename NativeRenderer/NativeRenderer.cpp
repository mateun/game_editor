// NativeRenderer.cpp : Hiermit werden die exportierten Funktionen für die DLL definiert.
//

#include "pch.h"
#include "framework.h"
#include "dx11_renderer.h"

#define NO_RENDERER_GUARD if (!renderer) { \
            throw std::runtime_error("no renderer providied! nullptr!"); }

void GetWindowClientSize(HWND hwnd, int& width, int& height)
{
    RECT rect;
    if (GetClientRect(hwnd, &rect))
    {
        width = rect.right - rect.left;
        height = rect.bottom - rect.top;
    }
    else
    {
        width = height = 0; // Handle error case
    }
}

NATIVERENDERER_API NativeRenderer* CreateRenderer(HWND hwnd, const char* rendererType) {
    

    try
    {
        if (std::string(rendererType) == "DX11")
        {
            auto r = new DX11Renderer(hwnd);
            return r;
        }

        OutputDebugStringA("Unknown renderer type!\n");
        return nullptr;
    }
    catch (const std::exception& e)
    {
        OutputDebugStringA(e.what());
        OutputDebugStringA("\n");
        return nullptr;
    }

   
    
}

void RenderFrame(NativeRenderer* renderer)
{
    if (renderer) {
        renderer->clean();
        renderer->render();
    }
    else {
        OutputDebugStringA("renderer invalid!\n");
    }
}

void ResizeRenderer(NativeRenderer* renderer, int width, int height)
{
    NO_RENDERER_GUARD
    renderer->resize(width, height);
}

void ImportModel(NativeRenderer* renderer, const char* modelFilePath)
{
    NO_RENDERER_GUARD
    renderer->importModel(modelFilePath);

}
