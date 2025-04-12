using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Controls;
using System.Diagnostics;
using System.Runtime.InteropServices;
using System.Windows.Media;

namespace GameEditor
{
    public class DxViewportHost : HwndHost
    {

        private IntPtr hwnd;
        private IntPtr nativeRenderer;
   
        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            const int WS_CHILD = 0x40000000;
            const int WS_VISIBLE = 0x10000000;

            hwnd = CreateWindowEx(0, "STATIC", "",
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            hwndParent.Handle, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);

            return new HandleRef(this, hwnd);
        }

        public void RenderFrame()
        {
           
            if (nativeRenderer != IntPtr.Zero)
            {
                Debug.WriteLine($"nativeRenderer handle: {nativeRenderer}");
                RenderFrame(nativeRenderer);
            }
            
        }

        protected override void OnRenderSizeChanged(SizeChangedInfo sizeInfo)
        {
            const uint SWP_NOZORDER = 0x0004;
            const uint SWP_NOMOVE = 0x0002;

            if (hwnd != IntPtr.Zero)
            {
                SetWindowPos(hwnd, IntPtr.Zero, 0, 0, (int)ActualWidth, (int)ActualHeight, SWP_NOMOVE | SWP_NOZORDER);
            }

            if (nativeRenderer == IntPtr.Zero && ActualWidth > 0)
            {
                nativeRenderer = CreateRenderer(hwnd, "DX11");

                if (nativeRenderer == IntPtr.Zero)
                {
                    throw new Exception("Failed to create native renderer!");
                }

                Debug.WriteLine($"Renderer ptr = {nativeRenderer}");
            }
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            DestroyWindow(hwnd.Handle);
        }




        [DllImport("user32.dll", SetLastError = true)]
        static extern IntPtr CreateWindowEx(int dwExStyle, string lpClassName,
        string lpWindowName, int dwStyle, int x, int y,
        int nWidth, int nHeight, IntPtr hWndParent, IntPtr hMenu,
        IntPtr hInstance, IntPtr lpParam);

        [DllImport("user32.dll", SetLastError = true)]
        [return: MarshalAs(UnmanagedType.Bool)]
        static extern bool DestroyWindow(IntPtr hWnd);

        [DllImport("user32.dll", SetLastError = true)]
        static extern bool UpdateWindow(IntPtr hWnd);

        [DllImport("user32.dll", SetLastError = true)]
        static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter,
    int X, int Y, int cx, int cy, uint uFlags);

        [DllImport("NativeRenderer.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern IntPtr CreateRenderer(IntPtr hwnd, [MarshalAs(UnmanagedType.LPStr)] string rendererType);

        [DllImport("NativeRenderer.dll", CallingConvention= CallingConvention.Cdecl)]
        static extern void RenderFrame(IntPtr renderer);

    }
}
