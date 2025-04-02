param ($generator='Visual Studio 17', $outDir='tmp/out/', $buildEditor=$TRUE, $buildTests=$TRUE, $buildGame=$TRUE)

$prevDirectory = Get-Location

$cmakeArgs = @('USE_SDL2=OFF')
IF ($buildEditor)
{
    $cmakeArgs += 'ONYX_BUILD_EDITOR=ON'
}
IF ($buildTests)
{
    $cmakeArgs += 'ONYX_BUILD_TESTS=ON'
}
IF ($buildGame)
{
    $cmakeArgs += 'ONYX_BUILD_GAME=ON'
}


.$PSScriptRoot/generate_vs_projects.ps1 -additonalCMakeArgs $cmakeArgs -generator "$generator" -outDir "$outDir"

Set-Location -Path $prevDirectory