param ($target, $outDir='tmp/out/')

if ($target -and $target -ne "")
{
	cmake --build "./$outDir/" --target $target
}
else
{
	cmake --build "./$outDir/"
}