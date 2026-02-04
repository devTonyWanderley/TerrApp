cmake_minimum_required(VERSION 3.19)
project(TopETerr LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

# 1. Primeiro adicionamos a pasta da biblioteca (TerraCore)
# Isso faz o CMake ler o projeto de engenharia primeiro
add_subdirectory(TerraCore)
add_subdirectory(TerraView)
add_subdirectory(TerraIO)

# 2. Criamos o executável com todos os arquivos
qt_add_executable(TopETerr
    WIN32 MACOSX_BUNDLE
    main.cpp
    terrapleno.cpp
    terrapleno.h
    terrapleno.ui
    NotasTecnicas.md
)

# 3. Agora que o TopETerr EXISTE, vinculamos as bibliotecas (Qt + Sua Engenharia)
target_link_libraries(TopETerr
    PRIVATE
        Qt6::Core
        Qt6::Widgets
        TerraCore
        TerraView
        TerraIO
)

# --- O restante permanece igual (Traduções e Deploy) ---

qt_add_translations(
    TARGETS TopETerr
    TS_FILES TopETerr_pt_BR.ts
)

include(GNUInstallDirs)
install(TARGETS TopETerr
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET TopETerr
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
