# Memorial Técnico - TopETerr / TerraCore

## 1. Padrões de Dados e Precisão
- **Unidades:** Toda a matemática interna utiliza metros (double).
- **Formatador Estrito (IO):** 
  - Números: 12 caracteres (unidade: décimo de milímetro).
  - Strings (IDs/Atributos): 16 caracteres.
  - Ângulos: 7 caracteres (formato GGGMMSS).
- **Precisão:** Uso de `long long` para armazenamento de décimos de milímetro em arquivos para evitar derivas de ponto flutuante.

## 2. Topologia e Geometria
- **Ordem Canônica:** 
  - Arestas: Sempre (v0 < v1).
  - Faces: Rotação cíclica iniciando pelo menor índice (preservação da Normal).
- **Sistema de Referência:** Vínculo biunívoco entre (X, Y, Z) e (DA, Offset).
- **Segmentos:** Banco centralizado (Pool) para permitir Topologia Paramétrica (efeito cascata entre Pista -> Calçada -> Lote).

## 3. Arquitetura de Software
- **TerraCore:** Sub-projeto C++17 independente (sem dependência de Qt).
- **TopETerr:** Interface gráfica em Qt Widgets.
- **Comunicação:** Interface Serial via `LinkSerialBase` (implementação nativa via WinAPI/POSIX).

## 4. Convenções de Código
- Indices: Sempre `size_t`.
- Texto: `std::string` com passagens por `std::move`.
- Containers: `std::variant` para tipos polimórficos sem herança pesada.
