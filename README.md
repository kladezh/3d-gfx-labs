# Как запустить

1. Скачайте этот репозиторий.
2. Создайте внутри него папку `Dependencies` и установите следующие библиотеки:

---

#### GLFW

1. На сайте https://www.glfw.org/download.html в области **Windows pre-compiled binaries** по кнопке **64-bit Windows binaries** скачайте архив.
2. В папке репозитория `Dependencies` создайте папку `GLFW`, а в ней создайте папки `include` и `libs`.
   1. В папку `GLFW/include` перенесите содержимое папки `include` из архива.
   2. В папку `GLFW/libs` перенести содержимое папки `lib-vc2022` из архива. (Для других версий Visual Studio не проверял как работает)

---

#### GLM

1. С репозитория https://github.com/g-truc/glm/releases/ скачайте архив последней версии.
2. Перенесите содержимое архива (папку `glm`) в папку `Dependencies`.

--- 

3. Итого у вас должны быть такие папки:
```text
3Dlabs/
    Dependencies/
        GLFW/
            include/
            libs/
        glm/
            detail/
            ext/
            ...
```

4. Откройте решение в Visual Studio, запустив файл `3DLabs/3Dlabs.sln`.

В **обозревателе решений (Solution Explorer)** будут лабы (или проекты). Разворачиваете проект и, внутри папки **Исходные Файлы (Source Files)**, открываете файл `main.cpp`. ПКМ кликаете по проекту и нажимаете **"Установить как стартовый проект" (Set as startup project)**. После этого можно запускать лабу.
