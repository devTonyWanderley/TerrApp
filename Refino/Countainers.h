/**
	1. O Ponto (A Unidade Atômica) Variant de Dados: O Ponto agora é um "camaleão".
	Ele armazena coordenadas cartesianas \((X,Y,Z)\) e usa std::variant para guardar
	ou dados de Levantamento (ID string + Atributo) ou dados de Projeto (Estaca + Offset).
	Integridade: O uso de std::move e construtores específicos garante que um ponto de
	levantamento nunca fique sem ID e um de projeto nunca perca sua referência de traçado.
	Sincronia: Adicionamos o método atualizarPeloEixo, que mantém a coerência biunívoca
	entre (X,Y) e (text{Estaca},text{Offset}).
	
	2. A Topologia (Malha e Superfície) Indexação Canônica: Arestas e Faces não guardam
	objetos Ponto, mas sim índices (size_t). Isso economiza memória e facilita a integração
	com a GPU (OpenGL/Qt).Ordenação e Unicidade:Arestas: Usam std::array<size_t, 2> sempre
	ordenados (v0<v1), eliminando duplicatas e ambiguidades.Faces: Usam Rotação Cíclica para
	manter a orientação da Normal (importante para saber se o talude sobe ou desce), mas
	garantindo que o menor índice comece a sequência.
	
	3. O Traçado Horizontal (O Eixo Universal)
	Tríade da Engenharia: A estrutura agora suporta Reta, Curva Circular e Espiral (Clotoide).
	Espelhamento: Decidimos tratar as espirais de entrada e saída como uma única função
	matemática espelhada, simplificando o cálculo de transição.
	Universalidade: O traçado serve tanto para eixos de rodovias quanto para perímetros de
	áreas industriais ou divisas de lotes.
	
	4. Gestão de Dados e Dependências (A Camada de Inteligência) Distância Acumulada (DA):
	O motor de cálculo usará apenas a DA como valor numérico puro. Estacas (ex: 10+0,00) e KM
	serão apenas máscaras de exibição no Qt.Topologia Paramétrica: Estabelecemos que Regiões
	(pistas, calçadas, lotes) compartilham segmentos. Se um eixo muda, o efeito cascata atualiza
	as bordas vinculadas e, consequentemente, os lotes adjacentes.Pool de Pontos: A estratégia de
	Append-Only (não apagar pontos, apenas criar/referenciar) garante que a malha nunca quebre
	referências de índice durante o processamento

*/

#pragma once
#include <variant>
#include <string>
#include <utility> // Obrigatório para o std::move
#include <array>
#include <algorithm> // para std::sort e std::min_element
#include <vector>
#include <optional>
#include <memory>

struct idAmostra {
    std::string id;
    std::string attr;
};

struct vNoTracado {
    double DA;
    double offset;
};

struct Ponto {
    double x, y, z;
    std::variant<std::monostate, idAmostra, vNoTracado> dados;

    Ponto(double x, double y, double z) 
        : x(x), y(y), z(z), dados(std::monostate{}) {}

    Ponto(double x, double y, double z, std::string id, std::string attr)
        : x(x), y(y), z(z), dados(idAmostra{std::move(id), std::move(attr)}) {}

    Ponto(double x, double y, double z, double DA, double offset)
        : x(x), y(y), z(z), dados(vNoTracado{DA, offset}) {}

    void atualizarPeloEixo(double novaDA, double novoOffset, double novoX, double novoY)
	{
        // Se já for projeto, apenas atualiza os valores
        if (auto* proj = std::get_if<vNoTracado>(&dados))
		{
            proj->DA = novaDA;
            proj->offset = novoOffset;
        } 
        // Se for monostate (genérico), transforma em projeto
        else if (std::holds_alternative<std::monostate>(dados))
		{
            dados = vNoTracado{novaDA, novoOffset};
        }
        
        // Coordenadas cartesianas são atualizadas em ambos os casos
        this->x = novoX;
        this->y = novoY;
    }
};

struct Aresta {
    std::array<size_t, 2> v;
    bool isBreakline = false;

    Aresta(size_t a, size_t b, bool breakline = false) : isBreakline(breakline) {
        v[0] = std::min(a, b);
        v[1] = std::max(a, b);
    }

    // Operador para ordenar/agrupar arestas (essencial para performance)
    bool operator<(const Aresta& other) const {
        return v < other.v; // std::array já tem comparação lexicográfica nativa
    }
};

struct Face {
    std::array<size_t, 3> v;

    Face(size_t a, size_t b, size_t c) {
        // Rotação cíclica para manter a Normal, mas iniciar pelo menor índice
        // Isso garante a forma canônica que você sugeriu
        std::array<size_t, 3> temp = {a, b, c};
        auto it = std::min_element(temp.begin(), temp.end());
        
        if (it == temp.begin())      { v = {temp[0], temp[1], temp[2]}; }
        else if (it == temp.begin()+1) { v = {temp[1], temp[2], temp[0]}; }
        else                         { v = {temp[2], temp[0], temp[1]}; }
    }
};

enum class DirecaoEspiral
{ 
    Entrada, // Raio diminui (Infinito -> R)
    Saida    // Raio aumenta (R -> Infinito)
};

struct SegmentoReta
{
    double x0, y0;
    double azimute;
    double comprimento;
};

struct SegmentoCurva
{
    double x0, y0; // Início do arco
    double azimute; // Início do arco
    double raio;
    double aCentral; 
};

struct SegmentoEspiral
{
    double xt, yt;					// Onde a espiral toca a reta (TS ou ST)
    double azimuteT;       // Azimute da reta suporte
    double raio;           // Raio da curva circular conectada
    double comprimento_total;      // Ls (comprimento da transição)
    Direcao direcao;        // Facilita o espelhamento
};

struct ElementoHorizontal
{
    double DA_ini;
    double DA_fim;
    bool travaTangenciaFinal = true;
    
    // Em vez de guardar a geometria aqui, guardamos o ID do Banco
    size_t idSegmento; 

    // Opcional: Se este elemento for um "filho" (ex: bordo da pista)
    // ele guarda o ID do eixo central e o offset
    size_t idSegmentoMestre = 0; 
    double offsetLateral = 0.0;
};

// O Banco de Segmentos (Segment Pool)
class BancoSegmentos {
public:
    struct Item {
        size_t id;
        std::variant<SegmentoReta, SegmentoCurva, SegmentoEspiral> geo;
        
        // Metadados de dependência
        size_t idSegmentoMestre = 0; // 0 se for original, ID do pai se for offset
        double offsetRelativo = 0.0; 
    };

private:
    std::vector<Item> pool;

public:
    // Retorna o ID do novo segmento criado
    size_t adicionar(std::variant<SegmentoReta, SegmentoCurva, SegmentoEspiral> g) {
        size_t novoId = pool.size() + 1;
        pool.push_back({novoId, std::move(g)});
        return novoId;
    }

    Item& obter(size_t id) { return pool[id - 1]; }
};


// Container para o resultado de buscas no traçado
struct ResultadoBusca
{
    size_t indiceElemento;
    double DALocal; // Distância relativa ao início do elemento
};

class Tracado2D
{
public:
    // O pool de elementos ordenados por DA
    std::vector<ElementoHorizontal> elementos;

    // Metadados do Traçado
    std::string nome;
    double DAInicial = 0.0;

    // Métodos de Estrutura (Interface)
    
    // Adiciona elemento garantindo a continuidade da DA
    void anexarElemento(ElementoHorizontal novo)
	{
        if (elementos.empty()) {
            novo.DA_ini = DAInicial;
        } else {
            novo.DA_ini = elementos.back().DA_fim;
        }
        elementos.push_back(std::move(novo));
    }

    // Busca rápida: em qual elemento está essa DA?
    std::optional<ResultadoBusca> localizarDA(double DAAlvo) const {
        if (elementos.empty() || DAAlvo < DAInicial) return std::nullopt;
        
        // Como as DAs são crescentes, usamos busca binária (O(log n))
        // Ideal para performance em traçados longos
        // ... lógica de busca ...
        return ResultadoBusca{0, 0.0}; 
    }
};

class GerenciadorGeometria {
public:
    // Múltiplos traçados (Eixos, Bordos, Divisas)
    std::vector<Tracado2D> eixos;
    
    // Acesso ao pool central de pontos (que discutimos antes)
    // std::vector<Ponto>& poolPontos; 
};

enum class TipoRegiao { 
    Lote, 
    Quadra, 
    PlatoIndustrial, 
    FaixaRodagem, 
    AreaPreservacao 
};

struct Contorno2D {
    std::string nome;
    TipoRegiao tipo;
    
    // O contorno é composto por um traçado (que já tem retas, curvas e espirais)
    Tracado2D perimetro;

    // Metadados úteis para terraplenagem
    double areaProjetada; // Calculada automaticamente
    bool sentidoHorario;  // Importante para algoritmos de "offset" (offset interno vs externo)

    // Garantia de fechamento: o último ponto do último elemento 
    // deve coincidir com o primeiro ponto do primeiro elemento.
    bool estaFechado() const; 
};
