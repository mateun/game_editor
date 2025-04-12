#pragma once
#ifdef NATIVERENDERER_EXPORTS
#define NATIVERENDERER_API __declspec(dllexport)
#else
#define NATIVERENDERER_API __declspec(dllimport)
#endif

#include "framework.h"


void GetWindowClientSize(HWND hwnd, int& width, int& height);



/// <summary>
/// This class defines a native rendering backend (e.g. DX11, DX12, GL, Vulkan..)
/// Pointers to this class are provided to (non-native) dll consumers.
/// </summary>
class NativeRenderer {
public:
	virtual void clean() = 0;
	virtual void render() = 0;
	virtual void resize(int w, int h) = 0;
	virtual void importModel(const char* modelFilePath) = 0;

};


extern "C" {

	NATIVERENDERER_API NativeRenderer* CreateRenderer(HWND hwnd, const char* rendererType = "DX11");
	NATIVERENDERER_API void RenderFrame(NativeRenderer* renderer);
	NATIVERENDERER_API void ResizeRenderer(NativeRenderer* r, int width, int height);
	NATIVERENDERER_API void DestroyRenderer(NativeRenderer* r);

	NATIVERENDERER_API void ImportModel(const char* modelFilePath);

}
