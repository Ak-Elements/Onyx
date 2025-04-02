Function ConvertTo-Boolean {
    param($Variable)
    If ($Variable -eq "Y") {
        $True
    }
    If ($Variable -eq "N") {
        $False
    }
}
$BuildTests = Read-Host -Prompt "Build Tests? (Y/N)"
$BuildEditor = Read-Host -Prompt "Build Editor? (Y/N)"

$BuildTests = ConvertTo-Boolean -Variable $BuildTests
$BuildEditor = ConvertTo-Boolean -Variable $BuildEditor

.\scripts\generate_all_projects.ps1 -buildTests $BuildTests -buildEditor $BuildEditor
Pause