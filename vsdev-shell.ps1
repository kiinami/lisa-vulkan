# vsdev-shell.ps1
param([string]$Command)

$vsInstallPath = if ($env:VS_INSTALL_PATH) { $env:VS_INSTALL_PATH }
                 else { "C:\Program Files\Microsoft Visual Studio\18\Community" }

Import-Module "$vsInstallPath\Common7\Tools\Microsoft.VisualStudio.DevShell.dll"
Enter-VsDevShell -VsInstallPath $vsInstallPath -SkipAutomaticLocation -Arch amd64 -HostArch amd64 | Out-Null

Invoke-Expression $Command
