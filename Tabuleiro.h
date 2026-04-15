#pragma once
#include <iostream>
#include <vector>
#include <string>

struct Lance {
    int linOrigem, colOrigem;
    int linDestino, colDestino;

    char pecaMovida;
    char pecaCapturada; // '.' se não comeu ninguém

    bool ehRoque;
    bool ehEnPassant;
    char pecaPromocao;  // Se promoveu, guarda a letra (ex: 'Q'). Se não, guarda '.'

    // Construtor para facilitar a criação
    Lance(int lo, int co, int ld, int cd, char pm, char pc = '.', bool roq = false, bool ep = false, char prom = '.')
        : linOrigem(lo), colOrigem(co), linDestino(ld), colDestino(cd),
        pecaMovida(pm), pecaCapturada(pc), ehRoque(roq), ehEnPassant(ep), pecaPromocao(prom) {
    }
};

struct EstadoAuxiliar {
    bool roqueBrancoRei, roqueBrancoRainha;
    bool roquePretoRei, roquePretoRainha;
    int colunaEnPassant; // -1 se não houver, ou a coluna (0-7) do peão que acabou de pular duas casas
    int meiaJogadas;     // Para a regra de empate dos 50 movimentos
};

class Tabuleiro {
private:
    char matriz[8][8];
    char turnoAtual; // 'w' ou 'b'

    int linhaSelecionada = -1;
    int colunaSelecionada = -1;
    std::vector<Lance> lancesPecaSelecionada;

    EstadoAuxiliar estado;

    // Histórico para podermos desfazer lances perfeitos (O SEGREDO DO MOTOR)
    std::vector<EstadoAuxiliar> historicoEstados;

    // --- 1. GERADORES DE MOVIMENTOS "BURROS" (Pseudo-Legais) ---
    // Eles geram o movimento da peça, mas não conferem se o próprio rei fica em xeque.
    void calcularMovimentosPeao(int l, int c, std::vector<Lance>& lista);
    void calcularMovimentosCavalo(int l, int c, std::vector<Lance>& lista);//Feito
    void calcularMovimentosBispo(int l, int c, std::vector<Lance>& lista);
    void calcularMovimentosTorre(int l, int c, std::vector<Lance>& lista);
    void calcularMovimentosRainha(int l, int c, std::vector<Lance>& lista);
    void calcularMovimentosRei(int l, int c, std::vector<Lance>& lista);

    // --- 2. O CORAÇÃO DO MOTOR (Make / Unmake) ---
    // Faz o lance na matriz e salva o estado antigo no histórico
    void fazerLanceInterno(const Lance& lance);
    // Desfaz o último lance, ressuscitando a peça capturada e voltando o estado
    void desfazerLanceInterno(const Lance& lance);

    // --- 3. VALIDAÇÃO DE AMEAÇAS ---
    // Retorna true se a casa (l, c) está na mira de alguma peça da 'corAtacante'
    bool casaEstaAtacada(int l, int c, char corAtacante);

    

public:
    Tabuleiro();
    ~Tabuleiro();

    void iniciarNovoJogo(); // Apenas chama carregarFEN com a string inicial

    // --- 4. MOVIMENTOS LEGAIS ("INTELIGENTES") ---

    // Retorna TODOS os lances válidos de uma peça.
    // É aqui que ele gera os "burros", FAZ o lance, testa se deu xeque, e DESFAZ.
    std::vector<Lance> obterMovimentosLegaisDaPeca(int linha, int coluna);

    // Procura onde está o rei da cor informada e usa a função acima
    bool reiEmXeque(char corRei);
    // --- 5. FIM DE JOGO ---
    bool semLances(char cor);
    bool ehXequeMate(char cor);
    bool ehEmpate(char cor); // Afogamento, material insuficiente ou 50 movimentos

    // --- 6. COMUNICAÇÃO COM O ARDUINO (BLUETOOTH) ---

    // O Arduino envia: "L 6 4" (Levantou peça). Você chama isso.
    // Retorna: "M 5 4 4 4" para acender os LEDs dos destinos.
    std::string obterMovimentosParaArduino(int linha, int coluna);

    // O Arduino envia "P 6 4 4 4" (Pousou a peça). Você tenta executar.
    // Se o movimento for legal na lista, executa e troca o turno. Devolve "OK" ou "ERRO".
    std::string tentarExecutarLanceArduino(int lOrigem, int cOrigem, int lDestino, int cDestino);

    // Retorna 'w' para brancas, 'b' para pretas, e '.' para casa vazia
    char obterCorPeca(char peca);
    //Checa se está dentro do tabuleiro
    bool posicaoValida(int linha, int coluna);
    // Utilitários de Console para você debugar
    void imprimirNoConsole();
    // Carrega qualquer posição de xadrez a partir de um código FEN
    void carregarFEN(const std::string& fen);
    // Obtem o código FEN de qualquer posição
    std::string obterFEN();

    bool preLance(char coord[2]);
    bool executarLance(char coord[2]);

    // Funções auxiliares para desenhar o tabuleiro de 64 posições
    char obterPeca(int l, int c) const { return matriz[l][c]; }
    bool ehCasaDestinoValida(int l, int c) const;

    const char getTurno() { return turnoAtual; };

    void cancelarSelecao();

    // Retorna true se esta for a exata casa da peça que o jogador está segurando na mão
    bool ehCasaSelecionada(int l, int c) const { return l == linhaSelecionada && c == colunaSelecionada; }
    bool ehCasaCaptura(int l, int c) const;
};