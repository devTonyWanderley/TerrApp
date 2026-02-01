cmake_minimum_required(VERSION 3.16)

project(TerraCore LANGUAGES CXX)
# Adicione isso logo após a definição do projeto
if(MSVC)
    # Para compiladores Microsoft (se um dia mudar)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
else()
    # Para o seu MinGW / GCC
    # -mbmi2: Ativa as instruções PDEP/PEXT
    # -march=native: Otimiza para o seu processador atual
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mbmi2 -march=native")
endif()


set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)

add_library(TerraCore STATIC
  terracore.cpp
  terracore.h
  ponto.h
  geometria_base.h
  banco_segmentos.h
  tracado.h
  superficie.h superficie.cpp
  terraio.h terraio.cpp
  linkserialbase.h linkserialbase.cpp
  arquiteturaterracore.qmt
  formatEstrito.h
  motorArquivoFixo.h
  laboratorio.h
  adaptadorPontos.h
  quadTree.h
  quadTree.cpp
)

target_link_libraries(TerraCore PRIVATE Qt${QT_VERSION_MAJOR}::Core)

target_compile_definitions(TerraCore PRIVATE TERRACORE_LIBRARY)

# Faz com que quem use a biblioteca encontre os headers automaticamente
target_include_directories(TerraCore INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
