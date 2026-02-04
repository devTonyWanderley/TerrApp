# Contrato do Motor Comunicação (TerraIO)
find_package(Qt6 REQUIRED COMPONENTS Widgets Gui Core)

# Habilita o MOC (Meta-Object Compiler) do Qt, necessário para 
# que o sistema entenda sinais e slots (como o seu teclado)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Cria a biblioteca estática
add_library(TerraIO STATIC
leitor.h
)

# Faz a ligação: o View precisa "enxergar" o Core (Pontos e Faces) e o Qt
target_link_libraries(TerraIO PUBLIC TerraCore TerraView Qt6::Widgets)

# Expõe os cabeçalhos para o resto do projeto
target_include_directories(TerraIO PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
