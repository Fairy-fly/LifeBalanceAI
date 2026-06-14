param(
    [string]$EnvFile = ".env",
    [string]$Package = "org.qtproject.example.LifeBalanceAI_demo",
    [string]$Adb = "D:\AndroidSDK\platform-tools\adb.exe"
)

$ErrorActionPreference = "Stop"

if (-not (Test-Path -LiteralPath $EnvFile)) {
    throw "Env file not found: $EnvFile"
}

$envText = Get-Content -LiteralPath $EnvFile -Raw
$hasDeepSeek = $false
foreach ($line in ($envText -split "`r?`n")) {
    $trimmed = $line.Trim().TrimStart([char]0xFEFF)
    if ($trimmed -match '^DEEPSEEK_API_KEY\s*=\s*.+$') {
        $value = ($trimmed -replace '^DEEPSEEK_API_KEY\s*=\s*', '').Trim().Trim('"').Trim("'")
        if (-not [string]::IsNullOrWhiteSpace($value)) {
            $hasDeepSeek = $true
            break
        }
    }
}

if (-not $hasDeepSeek) {
    throw "DEEPSEEK_API_KEY is missing or empty in $EnvFile"
}

if (-not (Test-Path -LiteralPath $Adb)) {
    $Adb = "adb"
}

$tmp = [System.IO.Path]::GetTempFileName()
try {
    $lines = @()
    foreach ($line in ($envText -split "`r?`n")) {
        $cleanLine = $line.TrimStart([char]0xFEFF)
        if ($cleanLine.TrimStart().StartsWith("DEEPSEEK_API_KEY=")) {
            $lines += $cleanLine
        }
    }
    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($tmp, ($lines -join "`n"), $utf8NoBom)

    & $Adb shell "run-as $Package sh -c 'mkdir -p files files/.local/share/$Package .local/share/$Package'"
    if ($LASTEXITCODE -ne 0) {
        throw "adb run-as failed. Install a debug build for package $Package first."
    }

    & $Adb push $tmp "/data/local/tmp/lifebalance_env.tmp" | Out-Null
    if ($LASTEXITCODE -ne 0) {
        throw "adb push failed."
    }

    & $Adb shell "run-as $Package sh -c 'cp /data/local/tmp/lifebalance_env.tmp files/.env && cp /data/local/tmp/lifebalance_env.tmp files/.local/share/$Package/.env && cp /data/local/tmp/lifebalance_env.tmp .local/share/$Package/.env && chmod 600 files/.env files/.local/share/$Package/.env .local/share/$Package/.env'"
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to copy .env into app private directories."
    }

    & $Adb shell rm /data/local/tmp/lifebalance_env.tmp | Out-Null

    Write-Host "DEEPSEEK_API_KEY written without printing the key."
    Write-Host "Targets:"
    Write-Host "  /data/data/$Package/files/.env"
    Write-Host "  /data/data/$Package/files/.local/share/$Package/.env"
    Write-Host "  /data/data/$Package/.local/share/$Package/.env"
    Write-Host "Restart the app before testing AI requests."
}
finally {
    Remove-Item -LiteralPath $tmp -Force -ErrorAction SilentlyContinue
}
