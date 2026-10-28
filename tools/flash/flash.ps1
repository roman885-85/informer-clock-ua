# Informer Clock UA — прошивка годинника через USB (Windows). EN: flash the clock over USB (Windows).
#
#   flash.bat update [COMx]   оновлення: лише програма, налаштування й Wi-Fi зберігаються (і при переході з прошивки
#                             автора Clock Informer) / update: firmware only, settings and Wi-Fi are kept
#   flash.bat full   [COMx]   чиста установка: стерти все й записати заново / clean install: erase and write all
#
# Потрібен драйвер USB-UART плати (CP210x або CH340). esptool завантажується сам (офіційна збірка Espressif),
# файли прошивки — з теки firmware поруч (архів випуску), з docs\firmware репозиторію або з GitHub.
param([string]$Mode = "", [string]$Port = "", [int]$Baud = 230400)
$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"   # PowerShell 5.1: смуга прогресу Invoke-WebRequest у десятки разів сповільнює завантаження
$Raw = "https://raw.githubusercontent.com/roman885-85/informer-clock-ua/main/docs/firmware"
$EsptoolZip = "https://github.com/espressif/esptool/releases/download/v4.12.0/esptool-v4.12.0-windows-amd64.zip"
$Here = Split-Path -Parent $MyInvocation.MyCommand.Path
$Fw = Join-Path $Here "..\..\docs\firmware"                  # у репозиторії
if (Test-Path (Join-Path $Here "firmware\informer-clock-ua-esp32.bin")) { $Fw = Join-Path $Here "firmware" }   # архів випуску
$Cache = Join-Path $env:LOCALAPPDATA "informer-clock-ua"
function Say($t) { Write-Host ""; Write-Host "> $t" -ForegroundColor Cyan }
function Die($t) { Write-Host ""; Write-Host "X $t" -ForegroundColor Red; exit 1 }

if ($Mode -eq "") {
  Write-Host "Informer Clock UA — прошивка через USB / USB flashing"
  Write-Host "  1 — оновлення, налаштування зберігаються / update, settings kept"
  Write-Host "  2 — чиста установка, стерти все / clean install, erase all"
  $a = Read-Host "Ваш вибір / your choice [1/2]"
  if ($a -eq "1") { $Mode = "update" } elseif ($a -eq "2") { $Mode = "full" }
}
if ($Mode -ne "update" -and $Mode -ne "full") { Write-Host "Використання / usage: flash.bat update|full [COMx]"; exit 1 }
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

# --- esptool.exe
New-Item -ItemType Directory -Force -Path $Cache | Out-Null
$Esptool = Get-ChildItem -Path $Cache -Recurse -Filter "esptool.exe" -ErrorAction SilentlyContinue | Select-Object -First 1
if (-not $Esptool) {
  Say "Завантажую esptool (Espressif) / downloading esptool"
  $Zip = Join-Path $Cache "esptool.zip"
  Invoke-WebRequest -UseBasicParsing -Uri $EsptoolZip -OutFile $Zip
  Expand-Archive -Force -Path $Zip -DestinationPath (Join-Path $Cache "esptool")
  $Esptool = Get-ChildItem -Path $Cache -Recurse -Filter "esptool.exe" | Select-Object -First 1
  if (-not $Esptool) { Die "esptool не знайдено після розпакування / esptool not found" }
}
$E = $Esptool.FullName

# --- файли прошивки
$Files = @("bootloader.bin", "partitions.bin", "boot_app0.bin", "informer-clock-ua-esp32.bin")
if (-not (Test-Path (Join-Path $Fw "informer-clock-ua-esp32.bin"))) {
  $Fw = Join-Path $Cache "firmware"
  New-Item -ItemType Directory -Force -Path $Fw | Out-Null
  Say "Завантажую прошивку з GitHub / downloading firmware"
  foreach ($f in $Files) { Invoke-WebRequest -UseBasicParsing -Uri "$Raw/$f" -OutFile (Join-Path $Fw $f) }
}
foreach ($f in $Files) { if (-not (Test-Path (Join-Path $Fw $f))) { Die "немає файла $f / missing file" } }

# --- порт
if ($Port -eq "") {
  $Ports = @([System.IO.Ports.SerialPort]::GetPortNames() | Sort-Object)   # @() — масив і для одного порту ($Ports[0] — «COM3», не «C»)
  if ($Ports.Count -eq 0) { Die "годинник не знайдено: під'єднайте USB-кабель і встановіть драйвер CP210x/CH340 / clock not found" }
  if ($Ports.Count -eq 1) { $Port = $Ports[0] }
  else {
    Write-Host "Знайдено порти / ports found: $($Ports -join ', ')"
    $Port = Read-Host "Введіть порт годинника, напр. COM3 / enter the clock port"
  }
}
Say "Порт / port: $Port, швидкість / baud: $Baud"
$B = { param($n) Join-Path $Fw $n }
$Hint = "запис не вдався: інший кабель чи порт; тримайте кнопку BOOT на платі, поки не почнеться запис; або повільніше: flash.bat update COM3 -Baud 115200 / write failed: try another cable or port; hold BOOT until writing starts; or slower: flash.bat update COM3 -Baud 115200"
if ($Mode -eq "full") {
  Say "Стираю флеш-пам'ять / erasing flash"
  & $E --chip esp32 --port $Port --baud $Baud erase_flash
  if ($LASTEXITCODE -ne 0) { Die $Hint }
  Say "Записую все / writing all parts"
  & $E --chip esp32 --port $Port --baud $Baud write_flash -z --flash_mode dio --flash_freq 40m --flash_size detect `
    0x1000 (& $B "bootloader.bin") 0x8000 (& $B "partitions.bin") 0xe000 (& $B "boot_app0.bin") 0x10000 (& $B "informer-clock-ua-esp32.bin")
} else {
  Say "Записую програму (налаштування зберігаються) / writing firmware, settings kept"
  & $E --chip esp32 --port $Port --baud $Baud write_flash -z 0xe000 (& $B "boot_app0.bin") 0x10000 (& $B "informer-clock-ua-esp32.bin")
}
if ($LASTEXITCODE -ne 0) { Die $Hint }
Say "Готово. Годинник перезапуститься сам. / Done, the clock restarts by itself."
if ($Mode -eq "full") { Write-Host "Далі: Wi-Fi «Clock ESP» → http://192.168.4.1 / then join Wi-Fi ""Clock ESP"" and open http://192.168.4.1" }
