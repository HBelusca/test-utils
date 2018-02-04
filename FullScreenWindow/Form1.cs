using System;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace FullScreenWindow
{
    public partial class Form1 : Form
    {
        public static class WindowsApi
        {
            [DllImport("user32.dll", SetLastError = true)]
            public static extern IntPtr FindWindowEx(IntPtr hwndParent, IntPtr hwndChildAfter, string lpszClass, string lpszWindow);

            public enum GWL
            {
                GWL_WNDPROC = (-4),
                GWL_HINSTANCE = (-6),
                GWL_HWNDPARENT = (-8),
                GWL_STYLE = (-16),
                GWL_EXSTYLE = (-20),
                GWL_USERDATA = (-21),
                GWL_ID = (-12)
            }

            public const int WS_EX_TOPMOST = 0x00000008;

            [DllImport("user32.dll", EntryPoint = "GetWindowLong")]
            public static extern int GetWindowLong(IntPtr hWnd, GWL nIndex);

            [DllImport("user32.dll", EntryPoint = "GetWindowLongPtr")]
            public static extern IntPtr GetWindowLongPtr(IntPtr hWnd, GWL nIndex);
        }

        public Form1()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, System.EventArgs e)
        {
            Close();
        }

        private void timer1_Tick(object sender, System.EventArgs e)
        {
            IntPtr hwndTaskbar = WindowsApi.FindWindowEx(IntPtr.Zero, IntPtr.Zero, "Shell_TrayWnd", null);
            int exStyle = WindowsApi.GetWindowLong(hwndTaskbar, WindowsApi.GWL.GWL_EXSTYLE);
            if ((exStyle & WindowsApi.WS_EX_TOPMOST) != 0)
                label1.Text = "The taskbar is ON TOP!";
            else
                label1.Text = "The taskbar is NOT on top!";
        }
    }
}
