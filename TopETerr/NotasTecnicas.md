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

###  Protocolo de Estabilidade Numérica e Escala de Trabalho
1.  **O Envelope de Segurança (Magnitude vs. Precisão) Raio de Operação Local:**
  Limitado a \(10^{5}\) metros (100 km).Justificativa:
  Ao manter as coordenadas locais nesta ordem de grandeza,
  garantimos que a mantissa do double (52 bits) preserve a precisão de \(10^{-5}\) mesmo após o "estresse" do
  determinante de Delaunay (que opera com o quadrado das distâncias, \(L^{2}\)).
  
  Margem de Manobra:
  O décimo de milímetro (\(10^{-4}\)) permanece isolado de ruídos de arredondamento e "rasgos" topológicos.
  
2.  **A Resolução de Conflito (O Critério Highlander) Tolerância de Proximidade (\(\epsilon \)):**
  Fixada em \(10^{-3}\) metros (1 mm).Regra de Unicidade:
  Dois pontos em um raio inferior a 1 mm são tratados como Entidade Única.
  Impacto:
  Elimina o risco de divisões por zero e loops infinitos na Quadtree, além de estabilizar o determinante do
  "Francês" em áreas de alta densidade de pontos.
  
3. **Hierarquia de Processamento (Quadtree)**
  Profundidade:
  24 níveis de partição.
  
  Resolução de Célula:
  Discretização do espaço em quadrantes de ordem centimétrica/milimétrica.
  
  Garantia:
  O "estouro" da árvore é fisicamente impossível devido ao limite de 1 mm de proximidade imposto na carga de dados.

### Limites do Universo Binário:
  **Resolução Estrita:**
    (0,1)mm (10E-4 m).
    
  **Quadro Master 2^26:**
    Alcance de 6,71km a partir da origem local. Neste raio, o motor opera com Zero Erro de Arredondamento em
    predicados geométricos.
    
  **Protocolo de Transbordamento:**
    Para projetos de infraestrutura linear que excedem 6,7km, o sistema adotará o quadro de 2^27 (13,42km),
    mantendo uma estabilidade topológica com precisão sub-milimétrica.
    

Mapa sobre o pântano:00
1. Filosofia de Projeto
Abstração Independente: O motor de engenharia (TerraCore) permanece agnóstico à interface (TopETerr).
Mecânica de Baixo Nível: Prioridade para estruturas contíguas em memória (std::vector) e manipulação direta de bits
(Shift/Mask) para performance "Assembly-like".
Sanidade na Fronteira: O dado entra "sujo" (comprimento fixo), mas é limpo e validado no Construtor (O Funil).
2. Decisões de Design (Os Pilares)
Estabilidade Numérica (Localização):
Implementação de Origem de Trabalho Automática (Menor X e Y da instância).
Uso de xLoc e yLoc para todos os cálculos geométricos (Proteção da Mantissa do double).Resolução e Tolerância:A malha
opera na escala de Décimo de Milímetro (\(10^{-4}\)m).Tolerância de Coincidência (\(\epsilon \)): Fixada em 1 milímetro.
Pontos dentro deste raio são fundidos.Indexação Espacial (A Quadtree Highlander):Particionamento por Resolução Fixa
(24 níveis) em vez de baldes de pontos.Tratamento de Fronteira: Implementação de consulta trans-fronteira para evitar
"rasgos" topológicos em limites de quadrantes.
3. Fila de Execução (O Caminho Crítico)
Fase A: Preparação de Terreno
Refinar ponto.h: Adicionar xLoc, yLoc e método centralizarNaOrigem(Xo, Yo).
Consolidar BoundingBox: Criar rotina que varre o poolPontos para extrair os limites e definir a Origem de Trabalho.
Fase B: O Indexador (A Quadtree)
Estruturar quadtree.h: Criar o Pool de Nós linear (sem new/delete esparsos).
Implementar Inserção: Incluir a trava de segurança dos 24 níveis para matar o loop infinito de pontos coincidentes.
Fase C: O Olho do Francês (Delaunay)
Predicados Robustos: Implementar orientar2d e testeInCircle (Parabolóide) no Laboratório usando Coordenadas Locais.
O Caminhador: Criar a navegação entre triângulos usando o Mapa de Adjacência.

Reflexão de Equipe (O "Check")
"Não estamos apenas acumulando pontos; estamos construindo um tecido matemático. A Quadtree organiza, o Francês conecta,
e as Breaklines (nossa 'mão grande') dão o veredito final."
