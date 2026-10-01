$sh = New-Object -ComObject Shell.Application
$folder = $sh.Namespace("C:\Users\hafte\.gemini\antigravity\scratch\harbour-thakir")
$files = @("001001.mp3", "001002.mp3", "002001.mp3", "002002.mp3", "002003.mp3")
foreach ($file in $files) {
    $item = $folder.ParseName($file)
    $len = $folder.GetDetailsOf($item, 27)
    Write-Host "$file : $len"
}
