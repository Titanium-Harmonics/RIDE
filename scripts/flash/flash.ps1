param(
    [string]$Port,
    [int]$Baud = 460800,
    [switch]$Yes
)
$ErrorActionPreference = "Stop"
$python = Get-Command py -ErrorAction SilentlyContinue
if ($python) {
    $exe = "py"
    $baseArgs = @("-3")
} elseif (Get-Command python -ErrorAction SilentlyContinue) {
    $exe = "python"
    $baseArgs = @()
} else {
    throw "Python 3 is required. Install it from https://www.python.org/downloads/ and rerun."
}
$script = Join-Path $PSScriptRoot "flash.py"
$params = @($script, "--baud", "$Baud")
if ($Port) { $params += @("--port", $Port) }
if ($Yes) { $params += "--yes" }
& $exe @baseArgs @params
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
