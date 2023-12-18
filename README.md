# Как запустить

1. Скачайте этот репозиторий.
2. Создайте внутри него папку `Dependencies`, создайте в ней папки `include` и `libs`, и установите следующие библиотеки:

---

#### GLFW

1. На сайте https://www.glfw.org/download.html в области **Windows pre-compiled binaries** по кнопке **64-bit Windows binaries** скачайте архив.
2. В папке репозитория `Dependencies/include` создайте папку `GLFW`.
   - В папку `Dependencies/include/GLFW` перенесите файлы из папки `include/GLFW/` из архива.
   - В папку `Dependencies/libs` перенести содержимое папки `lib-vc2022/` из архива. (Для других версий Visual Studio не проверял как работает)

---

#### GLM

1. С репозитория https://github.com/g-truc/glm/releases/ скачайте архив последней версии.
2. Внутри папки `Dependencies/include` создайте папку `GLM`.
3. В созданную папку `Dependencies/include/GLM` перенесите содержимое папки `glm/glm` из архива (в которой находятся С++ .h файлы).

--- 

#### GLAD

1. Перейдите на сайт https://glad.dav1d.de/.
2. Выберите:
   - Language - C/C++;
   - Specification - OpenGL;
   - API -> gl - Version 3.3;
   - Profile - Core;
   - Options - Generate a loader;
3. Нажмите кнопку **Generate** и скачайте архив glad.zip и распакуйте его.
4. Из папки архива `glad/include` переместите папки `glad` и `KHR` в папку `Dependencies/include`.
5. Переименуйте папку `glad` в `GLAD`.

---

3. Итого у вас должны быть такие папки:
```text
3Dlabs/
    Dependencies/
         include/
            GLAD/
            GLFW/
            GLM/
            KHR/
         libs/
            glfw3.dll
            glfw3.lib
            ...
        
```

4. Откройте решение в Visual Studio, запустив файл `3DLabs/3Dlabs.sln`.

В **обозревателе решений (Solution Explorer)** будут лабы (или проекты). Разворачиваете проект и, внутри папки **Исходные Файлы (Source Files)**, открываете файл `main.cpp`. ПКМ кликаете по проекту и нажимаете **"Установить как стартовый проект" (Set as startup project)**. После этого можно запускать лабу.
