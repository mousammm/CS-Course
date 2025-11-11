Add-Type @"

using System;

using System.Runtime.InteropServices;



public class WindowsAPI {

    [DllImport("kernel32.dll")]

    public static extern uint GetCurrentThreadId();

    

    [DllImport("user32.dll")]

    public static extern IntPtr GetForegroundWindow();

}

"@



[WindowsAPI]::GetCurrentThreadId()
