# Memorial Técnico - TopETerr / TerraCore

## 1. Padrões de Dados e Precisão
- **Unidades:** Toda a matemática interna utiliza metros (double).
- **Formatador Estrito (IO):** 
  - Números: 12 caracteres (unidade: décimo de milímetro).
  - Strings (IDs/Atributos): 16 caracteres.
  - Ângulos: 7 caracteres (formato GGGMMSS).
- **Precisão:** Uso de `long long` para armazenamento de décimos de
  milímetro em arquivos para evitar derivas de ponto flutuante.

## 2. Topologia e Geometria
- **Ordem Canônica:** 
  - Arestas: Sempre (v0 < v1).
  - Faces: Rotação cíclica iniciando pelo menor índice (preservação da Normal).
- **Sistema de Referência:** Vínculo biunívoco entre (X, Y, Z) e (DA, Offset).
- **Segmentos:** Banco centralizado (Pool) para permitir Topologia Paramétrica
  (efeito cascata entre Pista -> Calçada -> Lote).

## 3. Arquitetura de Software
- **TerraCore:** Sub-projeto C++17 independente (sem dependência de Qt).
- **TopETerr:** Interface gráfica em Qt Widgets.
- **Comunicação:** Interface Serial via `LinkSerialBase` (implementação nativa via WinAPI/POSIX).

## 4. Convenções de Código
- Indices: Sempre `size_t`.
- Texto: `std::string` com passagens por `std::move`.
- Containers: `std::variant` para tipos polimórficos sem herança pesada.
## Política de Sanidade de Dados (Data Cleansing)

### Strings de Identificação (idAmostra)
- **Regra de Ouro:** O dado em memória (RAM) é sempre "limpo" (sem espaços de preenchimento).
- **Implementação:** A limpeza (trim) ocorre exclusivamente no **Construtor** da struct `idAmostra`.
- **Justificativa:** Garantir que operadores de comparação (`==`, `<`) funcionem de forma previsível
  e que a busca no pool de pontos seja matematicamente exata.
- **IO:** A remontagem dos espaços para o formato de 16 caracteres é responsabilidade exclusiva da
  camada de exportação (`formatEstrito`).

### Motor de IO - Reutilização e Procedimento
- **Modularidade:** O método `importarParaMatriz` é um consumidor do `fatiarLinha`. 
- **Bypass de Comentários:** Implementado suporte a comentários (linhas iniciadas por `;`), permitindo
  que arquivos de instância como `.pdw` contenham metadados ignoráveis pelo motor.
- **Eficiência:** O uso de `std::vector::push_back` combinado com o fatiamento direto mantém o custo de
  processamento próximo ao limite do hardware.
  ## Estratégia de Desenvolvimento: Camada de Laboratório (Sandbox)
  
  ### Diretriz de Design
  - **Isolamento de Instabilidade:** Implementação de um ambiente temporário (`laboratorio.h`) para
    centralizar as rotinas de adaptação e integração entre o Motor de IO e as Entidades Geométricas.
  - **Justificativa:** Prevenir a proliferação de arquivos e o "engessamento" precoce das classes core
    (`Ponto`, `Aresta`, `Traçado`) enquanto a lógica de conversão de formatos externos (Excel, CAD, .PDW)
      estiver em fase de maturação.
  - **Responsabilidade:** O laboratório atua como um "tanque de areia" para testes de estresse e depuração
    de precisão numérica (décimos de mm), sendo o único local sujeito a refatorações frequentes.
  - **Consolidação:** Uma funcionalidade só será extraída do laboratório para um arquivo definitivo após
    comprovar estabilidade matemática e utilidade sistêmica.

## Registro de Homologação: Motor de IO e Carga de Dados

### Data: [Data de Hoje]
- **Status:** Concluído e Validado (531 pontos processados via instância .pdw).

### Decisões Técnicas Consolidadas:
1. **Suporte a Unicode (Acentuação):** 
   - Adotado o uso de `std::filesystem::u8path()` na abertura de streams (`ifstream`).
   - Justificativa: Garantir interoperabilidade com caminhos de diretórios do Windows que contenham caracteres
   especiais (ex: "Instâncias"), resolvendo falhas críticas de acesso ao arquivo.
2. **Sanidade de RAM vs. Persistência:**
   - Confirmado o funcionamento do "Funil" no construtor de `idAmostra`. Espaços de preenchimento do formato de
   largura fixa são removidos no nascimento do objeto, garantindo que as variáveis de trabalho contenham apenas dados puros.
3. **Precisão Numérica:**
   - Validação da conversão de décimos de milímetro para `double`. O erro de arredondamento foi mitigado pelo uso de
   `std::round` e `long long` no parser.

### Performance:
- Carga de ~500 pontos realizada em tempo imperceptível, validando a estratégia de alocação antecipada via 
`std::vector::reserve`.
