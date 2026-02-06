# Compila y ejecuta 1-Punteros\ejercicio1.cpp con entrada automatizada
$src = "1-Punteros\ejercicio1.cpp"
$exe = "1-Punteros\ejercicio1.exe"

Write-Host "Compilando..."
if (Get-Command g++ -ErrorAction SilentlyContinue) {
    g++ $src -o $exe
} elseif (Get-Command clang++ -ErrorAction SilentlyContinue) {
    clang++ $src -o $exe
} else {
    Write-Host "No se encontró un compilador C++ (g++/clang++)."; exit 1
}

Write-Host "Ejecutando test..."
$inputFile = "1-Punteros\test\input.txt"
# Ejecutar y capturar salida
$output = Get-Content $inputFile | & .\$exe 2>&1 | Out-String
Write-Host "---Salida del programa---"
Write-Host $output
Write-Host "---Fin salida---"

if ($output -match "El número mayor es:.*5") {
    Write-Host "TEST PASSED"
    exit 0
} else {
    Write-Host "TEST FAILED"
    exit 2
}