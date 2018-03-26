$Build = "$PSScriptRoot\build"
$CurrentDir = $PWD
if (!(Test-Path -Path $Build)) {
    New-Item -ItemType directory -Path $Build
}


Set-Location -Path $Build

#cmake -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake -G "Visual Studio 15 2017 Win64" ..

Set-Location -Path $CurrentDir