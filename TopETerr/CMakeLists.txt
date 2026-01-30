cmake_minimum_required(VERSION 3.19)
project(TopETerr LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

# No executável principal, vincule a biblioteca
#add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PRIVATE TerraCore)
# Adiciona o executável
#add_executable(TopETerr main.cpp terrapleno.cpp terrapleno.h)

qt_add_executable(TopETerr
    WIN32 MACOSX_BUNDLE
    main.cpp
    terrapleno.cpp
    terrapleno.h
    terrapleno.ui
)

qt_add_translations(
    TARGETS TopETerr
    TS_FILES TopETerr_pt_BR.ts
)

target_link_libraries(TopETerr
    PRIVATE
        Qt::Core
        Qt::Widgets
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
add_subdirectory(TerraCore)
