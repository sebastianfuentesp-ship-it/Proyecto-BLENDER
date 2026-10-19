# Librerías externas

Aquí van las dependencias ya compiladas para Windows de 32 bits (Win32), las mismas del código base del laboratorio, para que nadie tenga que instalar nada:

```
external/
├── assimp/   include/assimp/, lib/assimp-vc140-mt.lib, bin/assimp-vc140-mt.dll
├── GLEW/     include/GL/, lib/Release/Win32/glew32.lib, bin/glew32.dll
├── GLFW/     include/GLFW/, lib-vc2015/glfw3.lib
├── glm/      glm/ (solo headers)
└── SOIL2/    lib/soil2-debug.lib (los headers están en include/SOIL2/)
```

El proyecto `ProyectoFinal.vcxproj` ya tiene configuradas estas rutas de forma relativa (`$(SolutionDir)external\...`) para **Debug | Win32** y **Release | Win32**:

- *C/C++ → General → Directorios de inclusión adicionales:* `include`, `external\assimp\include`, `external\glm`, `external\GLFW\include`, `external\GLEW\include`
- *Vinculador → General → Directorios de bibliotecas adicionales:* `external\assimp\lib`, `external\SOIL2\lib`, `external\GLFW\lib-vc2015`, `external\GLEW\lib\Release\Win32`
- *Vinculador → Entrada → Dependencias adicionales:* `opengl32.lib; glew32.lib; glfw3.lib; assimp-vc140-mt.lib; soil2-debug.lib`
- Un evento posterior a la compilación copia `glew32.dll` y `assimp-vc140-mt.dll` junto al `.exe`.

Las librerías son de 32 bits, así que el proyecto debe compilarse en **Win32**, no en x64.
