using System;
using System.Windows;
using System.Windows.Interop;
using System.Windows.Controls;
using System.Diagnostics;
using System.Runtime.InteropServices;

namespace GameEditor
{
    public class DxViewportHost : HwndHost
    {

        private IntPtr hwnd;
        private IntPtr nativeRenderer;
        private const int WS_CHILD = 0x40000000;
        private const int WS_VISIBLE = 0x10000000;

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            hwnd = CreateWindowEx(0, "STATIC", "",
            WS_CHILD | WS_VISIBLE,
            0, 0, 0, 0,
            hwndParent.Handle, IntPtr.Zero, IntPtr.Zero, IntPtr.Zero);

            int val = CreateRenderer(hwnd);
            Debug.WriteLine("value from c++: " + val);
            
            return new HandleRef(this, hwnd);
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

        [DllImport("NativeRenderer.dll")]
        static extern int CreateRenderer(IntPtr hwnd);

    }
}
