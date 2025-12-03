# Script para reemplazar (Color){} por Color{} en archivos .cpp

# Obtener la ruta actual o especificar una ruta
$rutaBase = Get-Location

# Buscar todos los archivos .cpp recursivamente
$archivosCpp = Get-ChildItem -Path $rutaBase -Filter "*.cpp" -Recurse -File

# Contador de archivos modificados
$archivosModificados = 0

Write-Host "Buscando archivos .cpp en: $rutaBase" -ForegroundColor Cyan
Write-Host "Archivos encontrados: $($archivosCpp.Count)" -ForegroundColor Cyan
Write-Host ""

foreach ($archivo in $archivosCpp) {
    Write-Host "Procesando: $($archivo.FullName)" -ForegroundColor Yellow
    
    # Leer el contenido del archivo
    $contenido = Get-Content -Path $archivo.FullName -Raw -Encoding UTF8
    
    # Realizar el reemplazo manteniendo el contenido dentro de {}
    $contenidoModificado = $contenido -replace '\(Color\)(\{[^}]*\})', 'Color$1'
    
    # Verificar si hubo cambios
    if ($contenido -ne $contenidoModificado) {
        # Guardar los cambios
        Set-Content -Path $archivo.FullName -Value $contenidoModificado -Encoding UTF8 -NoNewline
        Write-Host "  ✓ Modificado" -ForegroundColor Green
        $archivosModificados++
    } else {
        Write-Host "  - Sin cambios" -ForegroundColor Gray
    }
}

Write-Host ""
Write-Host "Proceso completado." -ForegroundColor Cyan
Write-Host "Archivos modificados: $archivosModificados de $($archivosCpp.Count)" -ForegroundColor Green