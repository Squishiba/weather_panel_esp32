@echo off
rem CLion "Environment file" for the ESP-IDF toolchain.
rem CLion runs this file and captures the resulting environment; export.bat
rem must be invoked with "call", otherwise CLion's cmd session dies and
rem IDF_PATH never reaches CMake.
rem
rem Toolchain settings: Settings > Build, Execution, Deployment > Toolchains
rem   > ESP-IDF > Environment file:
rem   C:\Users\Jared\Desktop\weather_thing_c\weather_thing_esp32\espidf_clion_env.bat
call C:\Espressif\frameworks\esp-idf-v5.5.3\export.bat
