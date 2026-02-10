$VerLine = Get-Content -Path "dual-key-remap.c" | Select-Object -First 1
$Version = ($VerLine | Select-String -Pattern '"(.+)"').Matches.Groups[1].Value
Write-Output "Releasing version $Version..."
$Folder = "releases\dual-key-remap-v$Version"

if (Test-Path -Path $Folder) {
    rm -r $Folder
}
mkdir $Folder

Copy-Item .\dual-key-remap.exe $Folder
Copy-Item .\README.md $Folder\README.txt
Copy-Item .\CHANGELOG.md $Folder\CHANGELOG.txt
Copy-Item .\LICENSE $Folder\LICENSE.txt
Copy-Item .\config.example.txt $Folder\config.txt

$Zip = "$Folder.zip"
if (Test-Path -Path $Zip) { Remove-Item $Zip }
Compress-Archive -Path "$Folder\*" -DestinationPath $Zip
Write-Output "Created $Zip"

explorer $Folder
