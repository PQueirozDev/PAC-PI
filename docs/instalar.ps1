
& {
    $ErrorActionPreference = 'Stop'
    [Net.ServicePointManager]::SecurityProtocol = [Net.ServicePointManager]::SecurityProtocol -bor [Net.SecurityProtocolType]::Tls12

    $repositorio = 'https://github.com/PQueirozDev/PAC-PI'
    $areaDeTrabalho = [Environment]::GetFolderPath('Desktop')
    if ($env:ONDE_FICA_PASTA) {
        $destino = $env:ONDE_FICA_PASTA
    }
    else {
        $destino = Join-Path $areaDeTrabalho 'Onde-Fica'
    }

    Write-Host ''
    Write-Host '=== Onde Fica? - instalador ===' -ForegroundColor Yellow
    Write-Host "Pasta de destino: $destino"
    Write-Host ''

    $temGit = [bool](Get-Command git -ErrorAction SilentlyContinue)

    if (Test-Path $destino) {
        if ($temGit -and (Test-Path (Join-Path $destino '.git'))) {
            Write-Host 'A pasta ja existe. Atualizando com git pull...' -ForegroundColor Cyan
            git -C $destino pull --ff-only
            if ($LASTEXITCODE -ne 0) {
                Write-Host 'Nao consegui atualizar. Veja a mensagem do git acima.' -ForegroundColor Red
                return
            }
        }
        else {
            Write-Host 'A pasta ja existe e nao veio do git.' -ForegroundColor Red
            Write-Host 'Apague ou renomeie essa pasta e rode o comando de novo.'
            return
        }
    }
    elseif ($temGit) {
        Write-Host 'Clonando o repositorio com o git...' -ForegroundColor Cyan
        git clone "$repositorio.git" $destino
        if ($LASTEXITCODE -ne 0) {
            Write-Host 'O git clone falhou. Veja a mensagem acima.' -ForegroundColor Red
            return
        }
    }
    else {
        Write-Host 'Git nao encontrado. Baixando o .zip do GitHub...' -ForegroundColor Cyan
        $zip = Join-Path $env:TEMP 'onde-fica.zip'
        $extraido = Join-Path $env:TEMP 'onde-fica-extraido'
        if (Test-Path $extraido) { Remove-Item $extraido -Recurse -Force }
        Invoke-WebRequest "$repositorio/archive/refs/heads/main.zip" -OutFile $zip -UseBasicParsing
        Expand-Archive $zip -DestinationPath $extraido -Force
        $pastaDoZip = Get-ChildItem $extraido -Directory | Select-Object -First 1
        Move-Item $pastaDoZip.FullName $destino
        Remove-Item $zip, $extraido -Recurse -Force
    }

    Write-Host ''
    Write-Host 'Pronto! O projeto esta em:' -ForegroundColor Green
    Write-Host "  $destino"
    Write-Host ''
    Write-Host 'Para jogar: abra JogoAllegro.sln no Visual Studio 2022 e aperte F5.'
    Write-Host 'O Visual Studio baixa o Allegro sozinho pelo NuGet na primeira vez.'
    Write-Host ''

    Start-Process explorer.exe $destino
}
