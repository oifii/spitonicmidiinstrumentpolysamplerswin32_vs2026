# Installation

This section guides you through setting up the required build tools on Windows. You will install Microsoft Visual Studio with the **Desktop development with C++** workload, ensure the correct Windows SDK is available, and prepare your environment to open and build the Win32 GUI application.

## 🎯 Prerequisites

Before proceeding, confirm you have or will install:

- **Microsoft Visual Studio** (2022 v17.5+ or later)
- **Desktop development with C++** workload
- **Windows 10 SDK** (version 10.0.x)
- **MSVC v14.5** toolset (Platform Toolset `v145`)
- Win32 GUI application support

> These components match the project’s Visual Studio configuration: `<WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>` and `<PlatformToolset>v145</PlatformToolset>` .

---

## 1. Installing Visual Studio

Follow these steps to install or update Visual Studio:

1. Download **Visual Studio Installer** from

https://visualstudio.microsoft.com/downloads

| Component | Purpose |
| --- | --- |
| MSVC v14.5 - VS 2022 C++ x86/x64 tools | Compiler, linker, C++ standard libraries (PlatformToolset v145) |
| Windows 10 SDK (10.0.x) | Headers, libraries for targeting Windows 10 (TargetPlatformVersion 10.0) |
| C++ CMake tools for Windows | Enables alternative builds via CMake (if desired) |


1. Launch the **Installer** and choose **Modify** (or **Install**).
2. On the **Workloads** tab, select:
3. **Desktop development with C++**
4. (Optional) Switch to the **Individual components** tab and verify:
5. Click **Modify** (or **Install**) to begin the download and installation.

> **Tip:** The Windows 10 SDK may install automatically with the workload. If you need a specific SDK version, select it explicitly under **Individual components**.

---

## 2. Verifying Your Setup ✅

After installation, confirm your environment:

```bash
# Open Developer Command Prompt for VS
cl.exe
```

You should see output similar to:

```plaintext
Microsoft (R) C/C++ Optimizing Compiler Version 19.35.32215 for x86
```

To check SDK and toolset via vswhere:

```powershell
"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" `
  -latest `
  -prerelease `
  -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 `
  -property installationPath
```

This verifies that the VC tools (including v145) are present.

---

## 3. Opening and Building the Project ⚙️

1. **Launch** Visual Studio.
2. **File → Open → Project/Solution** and select `spitonicmidiinstrumentpolysamplerswin32.vcxproj`.
3. In the **Solution Explorer**, choose your configuration:
4. **Debug | Win32** or **Release | Win32**
5. Press **F7** or click **Build → Build Solution**.

The project is a Win32 GUI application; Visual Studio will handle all include paths and library references automatically.

```xml
<PropertyGroup Label="Globals">
  <WindowsTargetPlatformVersion>10.0</WindowsTargetPlatformVersion>
</PropertyGroup>
...
<PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
  <PlatformToolset>v145</PlatformToolset>
</PropertyGroup>
```

---

```card
{
    "title": "Ensure Correct Versions",
    "content": "Visual Studio 2022 v17.5+ is required for MSVC v14.5 (PlatformToolset v145) and Windows 10 SDK 10.0."
}
```

---

## Summary of Components

| Component | Version / Identifier | Role |
| --- | --- | --- |
| Microsoft Visual Studio | 2022 v17.5+ | IDE and core build system |
| Desktop development with C++ | Latest | C++ compiler, headers, libraries |
| Windows 10 SDK | 10.0.x | Win32 target support |
| MSVC Platform Toolset | v145 | Compiler toolset specified in `.vcxproj` |


With these tools in place, your development environment is ready to compile and run the Win32 MIDI sampler instrument.