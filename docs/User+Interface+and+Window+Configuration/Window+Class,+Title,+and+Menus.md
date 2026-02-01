# User Interface and Window Configuration – Window Class, Title, and Menus

This section describes how the Win32 window class name, title text, menu bar, accelerator keys, and title bar style are defined, and how to override them via command-line arguments for kiosk or development modes.

## Window Class Name

The **window class name** uniquely identifies the application’s window type to the Win32 API.

- Default defined as `spitonicmidiinstrumentpolysamplerswin32class`
- Stored in `TCHAR szWindowClass[1024]`
- Used in `MyRegisterClass` and `CreateWindow` calls

```cpp
TCHAR szWindowClass[1024] = { L"spitonicmidiinstrumentpolysamplerswin32class" };
```

### Override via Command Line

You can supply a custom class name as the 20th argument:

```cpp
if (nArgs > 19) {
    wcscpy(szWindowClass, szArgListW[19]);
}
```

- **Arg 19** (WCHAR): new window class name

## Window Title Text

The **window title** appears in the title bar or taskbar entry.

- Default defined as `spitonicmidiinstrumentpolysamplerswin32title`
- Stored in `TCHAR szTitle[1024]`
- Applied in `CreateWindow` for the window caption

```cpp
TCHAR szTitle[1024] = { L"spitonicmidiinstrumentpolysamplerswin32title" };
```

### Override via Command Line

Pass a custom title as the 21st argument:

```cpp
if (nArgs > 20) {
    wcscpy(szTitle, szArgListW[20]);
}
```

- **Arg 20** (WCHAR): new window title

## Menu Bar Visibility 🍔

The **menu bar** can be shown or hidden at startup to suit kiosk or dev environments.

- Controlled by `global_menubardisplay` (0 = off, 1 = on)
- Applied in `MyRegisterClass` via `wcex.lpszMenuName`

```cpp
if (global_menubardisplay) {
    wcex.lpszMenuName = MAKEINTRESOURCE(IDC_SPIWAVWIN32);
} else {
    wcex.lpszMenuName = NULL;
}
```

### Override via Command Line

Assign a value in the 13th argument:

```cpp
if (nArgs > 12) {
    global_menubardisplay = atoi(szArgList[12]);
}
```

- **Arg 12** (int): `0` to hide menu, `1` to show menu

## Accelerator Keys

**Accelerators** map keystrokes (e.g., Alt+?) to menu commands.

- Enabled by default (`global_acceleratoractive = 1`)
- Loaded in WinMain with `LoadAccelerators` if active

```cpp
if (global_acceleratoractive) {
    hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_SPIWAVWIN32));
} else {
    hAccelTable = NULL;
}
```

### Override via Command Line

Use the 14th argument to toggle:

```cpp
if (nArgs > 13) {
    global_acceleratoractive = atoi(szArgList[13]);
}
```

- **Arg 13** (int): `0` to disable, `1` to enable

## Title Bar and Window Style

The **window style** determines if the standard title bar and frame are shown.

- Controlled by `global_titlebardisplay` (0 = no title, 1 = standard title)
- Chooses between `WS_OVERLAPPEDWINDOW` and `WS_POPUP` in `InitInstance`

```cpp
if (global_titlebardisplay) {
    hWnd = CreateWindow(szWindowClass, szTitle,
        WS_OVERLAPPEDWINDOW, global_x, global_y,
        global_xwidth, global_yheight,
        NULL, NULL, hInstance, NULL);
} else {
    hWnd = CreateWindow(szWindowClass, szTitle,
        WS_POPUP | WS_VISIBLE, global_x, global_y,
        global_xwidth, global_yheight,
        NULL, NULL, hInstance, NULL);
}
```

### Override via Command Line

Set value with the 12th argument:

```cpp
if (nArgs > 11) {
    global_titlebardisplay = atoi(szArgList[11]);
}
```

- **Arg 11** (int): `0` for borderless pop-up, `1` for chrome window

---

| Property | Default Variable | Default Value | Cmd-Line Arg | Description |
| --- | --- | --- | --- | --- |
| Window Class Name | `szWindowClass` | `"spitonicmidiinstrumentpolysamplerswin32class"` | 19 | Overrides the Win32 window class name |
| Window Title Text | `szTitle` | `"spitonicmidiinstrumentpolysamplerswin32title"` | 20 | Overrides the window’s caption |
| Menu Bar Visibility 🍔 | `global_menubardisplay` | `0` (hidden) | 12 | Show (`1`) or hide (`0`) the menu bar |
| Accelerator Keys | `global_acceleratoractive` | `1` (enabled) | 13 | Enable (`1`) or disable (`0`) accelerators |
| Title Bar & Window Chrome | `global_titlebardisplay` | `1` (standard frame) | 11 | Standard window (`1`) or pop-up (`0`) style |


Use these settings to tailor the instrument’s UI for full-screen kiosk displays or to expose standard chrome and menus during development and testing.