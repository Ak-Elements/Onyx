param ($additonalCMakeArgs, $generator='Visual Studio 17', $outDir='tmp/out/')

$prevDirectory = Get-Location

mkdir $outDir -ea 0
Set-Location $outDir

$args = ""
if ($additonalCMakeArgs -ne $null)
{
	$args = $additonalCMakeArgs.foreach({"-D $PSItem"})
	
}

echo $args

cmake $prevDirectory -U'ONYX_BUILD*' $args -G "$generator"

Set-Location -Path $prevDirectory