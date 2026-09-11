param(
	[Parameter(
		Mandatory = $false,
		HelpMessage = "Release configuration type"
	)]
	[alias("config","c")]
	[string]$configuration = "Release"
)

$project = "NoLockpickActivate"

# pre build step for SKSE
Write-Host "Running pre-build step for SKSE64..."
. $PSScriptRoot\preBuild.ps1

# inject NLA into SKSE project
Write-Host "Injecting $project project into SKSE64..."
$builddir = Join-Path (Get-Location) "./deps/skse64/build/"
$vcproj = Join-Path (Get-Location) "./$project.vcxproj"
$reldir = [System.IO.Path]::GetRelativePath($builddir, $vcproj)
. $PSScriptRoot\mutateXml.ps1 -p $builddir/umbrella.slnx `
	-xp "/Solution" `
	-newElem "Project" `
	-attribs @{ Path = $reldir }

# fixup SKSE build type
Write-Host "Setting SKSE64 to build as a static library..."
. $PSScriptRoot\mutateXml.ps1 -p $builddir/skse64/skse64.vcxproj `
	-xp "//ms:PropertyGroup[@Condition=""'`$(Configuration)|`$(Platform)'=='$configuration|x64'""]/ms:ConfigurationType" `
	-val "StaticLibrary"

Write-Host "Building $project with MSBuild"
msbuild /m  /p:Configuration=$configuration /p:Platform=x64 -t:$project .\deps\skse64\build\umbrella.slnx