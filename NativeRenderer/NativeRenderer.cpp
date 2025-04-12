// NativeRenderer.cpp : Hiermit werden die exportierten Funktionen für die DLL definiert.
//

#include "pch.h"
#include "framework.h"
#include "NativeRenderer.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <dxgi.h>


//// Dies ist ein Beispiel für eine exportierte Variable.
//NATIVERENDERER_API int nNativeRenderer=0;
//
//// Dies ist ein Beispiel für eine exportierte Funktion.
//NATIVERENDERER_API int fnNativeRenderer(void)
//{
//    return 0;
//}
//
//// Dies ist der Konstruktor einer Klasse, die exportiert wurde.
//CNativeRenderer::CNativeRenderer()
//{
//    return;
//}

static ID3D11Device* device;
static ID3D11DeviceContext* ctx;

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

NATIVERENDERER_API int CreateRenderer(HWND hwnd) {
    int w, h;
    GetWindowClientSize(hwnd, w, h);
    D3D_FEATURE_LEVEL featureLevels = D3D_FEATURE_LEVEL_11_1;
    HRESULT result = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, &featureLevels, 1, D3D11_SDK_VERSION,
        &device, NULL, &ctx);
    OutputDebugStringA("Dirctx initilaized!\n");

    // TODO continue with DX11 setup
    return 42;
}
