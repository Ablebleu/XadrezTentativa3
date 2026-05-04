#pragma once
#include "Tabuleiro.h"
#include "Simulacao.h"
#include <iostream>
#include <string>
#include <vector>

// 1. PRIMEIRO: Definimos o Nó
struct NoHistorico {
    std::string fen;         // O estado do tabuleiro NESTE momento
    std::string lanceUCI;    // O lance que FOI FEITO para chegar a este FEN (ex: "e2e4")
    std::string lanceSAN;    // O lance legível que FOI FEITO (ex: "e4")

    NoHistorico* proximo;
    NoHistorico* anterior;

    NoHistorico(std::string f, std::string uci = "", std::string san = "") {
        fen = f;
        lanceUCI = uci;
        lanceSAN = san;
        proximo = nullptr;
        anterior = nullptr;
    }
};

// 2. SEGUNDO: Definimos a Lista
class ListaPartida {
public:
    NoHistorico* cabeca;
    NoHistorico* atual;

    ListaPartida() : cabeca(nullptr), atual(nullptr) {}

    ~ListaPartida() {
        limparHistorico();
    }

    void limparHistorico() {
        NoHistorico* temp = cabeca;
        while (temp != nullptr) {
            NoHistorico* apagar = temp;
            temp = temp->proximo;
            delete apagar;
        }
        cabeca = nullptr;
        atual = nullptr;
    }

    void adicionarEstado(const std::string& novaFen, const std::string& uci, const std::string& san) {
        NoHistorico* novo = new NoHistorico(novaFen, uci, san);

        if (!cabeca) {
            cabeca = novo;
            atual = cabeca;
        }
        else if (novaFen == atual->fen) {
            NoHistorico* aux = atual;

            if (aux->anterior != nullptr) {
                aux->anterior->proximo = novo;
            }
            else {
                cabeca = novo;
            }

            novo->anterior = aux->anterior;
            atual = novo;
            delete aux;
        }
        else {
            atual->proximo = novo;
            novo->anterior = atual;
            atual = novo;
        }
    }
};

// 3. TERCEIRO: Definimos o Jogo (Agora o compilador já sabe o que é a ListaPartida!)
class Jogo {
private:
    Tabuleiro tabuleiroJogo;
    ListaPartida histLances;
    char tabuleiroBinario[64];
    char selecP[2];
    char movP[2];

public:
    Jogo() {
        while (1) {
            system("clear");
            pergunta();

            histLances.limparHistorico();
            histLances.adicionarEstado(tabuleiroJogo.obterFEN(), "", "");

            gameLoop();
        }
    }
    ~Jogo() {}

    void gameLoop() {
        checaFinalizacao();
        while (1) {
            std::cout << "\n--- TURNO: " << tabuleiroJogo.getTurno() << " ---\n";

            bool turnoFinalizado = false;

            while (!turnoFinalizado) {

                std::string fenAntiga = tabuleiroJogo.obterFEN();

                // 1. Mostra o tabuleiro normal 
                atualizarMapa(false);
                Simulacao::atualizarTabuleiro(tabuleiroBinario);

                // 2. Espera o jogador escolher uma peça válida
                do {
                    std::cout << "Escolha uma peca: ";
                    Simulacao::selecionarPeca(selecP);
                } while (!tabuleiroJogo.preLance(selecP));

                // 3. Mostra o tabuleiro com as opções
                std::cout << "\nOpcoes de movimento:\n";
                atualizarMapa(true);
                Simulacao::atualizarTabuleiro(tabuleiroBinario);

                // 4. Espera o jogador escolher um destino válido ou cancelar
                bool aguardandoDestino = true;
                do {
                    std::cout << "Escolha o destino (ou a mesma casa para cancelar): ";
                    Simulacao::lerMovimento(movP);

                    if (movP[0] == selecP[0] && movP[1] == selecP[1]) {
                        std::cout << "Selecao cancelada!\n";
                        tabuleiroJogo.cancelarSelecao();
                        aguardandoDestino = false;
                    }
                    else {
                        // Tenta executar o lance
                        if (tabuleiroJogo.executarLance(movP)) {
                            aguardandoDestino = false;
                            turnoFinalizado = true;

                            // GERAÇÃO DE PGN APÓS LANCE DE SUCESSO:
                            std::string fenNova = tabuleiroJogo.obterFEN();
                            std::string textoLances = decodificarLanceEntreFens(fenAntiga, fenNova);
                            std::string uci = "";
                            std::string san = "";

                            size_t posEspaco = textoLances.find(' ');
                            if (posEspaco != std::string::npos) {
                                uci = textoLances.substr(0, posEspaco);
                                san = textoLances.substr(posEspaco + 1);
                            }
                            if (tabuleiroJogo.ehXequeMate(tabuleiroJogo.getTurno())) {
                                san += "#";
                            }
                            else if (tabuleiroJogo.reiEmXeque(tabuleiroJogo.getTurno())) {
                                san += "+";
                            }
                            histLances.adicionarEstado(fenNova, uci, san);

                            // Imprime PGN atualizado
                            std::cout << std::endl;
                            imprimirPGN(histLances, histLances.cabeca->fen);
                        }
                        else {
                            std::cout << "Movimento invalido. Tente novamente.\n";
                        }
                    }
                } while (aguardandoDestino);

                // Checagens de Fim de Jogo
                checaFinalizacao();
            }
        }
    }

    void checaFinalizacao() {
        if (tabuleiroJogo.ehXequeMate(tabuleiroJogo.getTurno())) {
            atualizarMapa(false);
            Simulacao::atualizarTabuleiro(tabuleiroBinario);
            std::cout << "\n*** XEQUE-MATE! ***\n";
            system("pause");
            return;
        }
        else if (tabuleiroJogo.ehEmpate(tabuleiroJogo.getTurno())) {
            atualizarMapa(false);
            Simulacao::atualizarTabuleiro(tabuleiroBinario);
            std::cout << "\n*** EMPATE! ***\n";
            system("pause");
            return;
        }
    }

    void atualizarMapa(bool mostrarLances) {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (mostrarLances && tabuleiroJogo.ehCasaSelecionada(i, j)) {
                    tabuleiroBinario[i * 8 + j] = '4';
                }
                else if (mostrarLances && tabuleiroJogo.ehCasaCaptura(i, j)) {
                    tabuleiroBinario[i * 8 + j] = '3';
                }
                else if (mostrarLances && tabuleiroJogo.ehCasaDestinoValida(i, j)) {
                    tabuleiroBinario[i * 8 + j] = '2';
                }
                else {
                    char peca = tabuleiroJogo.obterPeca(i, j);
                    if (tabuleiroJogo.obterCorPeca(peca) != '.') {
                        tabuleiroBinario[i * 8 + j] = '1';
                    }
                    else {
                        tabuleiroBinario[i * 8 + j] = '0';
                    }
                }
            }
        }
    }

    std::vector<std::string> expandirMatrizFen(const std::string& fen) {
        std::vector<std::string> matriz(8, "........");
        int l = 0, c = 0;
        for (char ch : fen) {
            if (ch == ' ') break;
            if (ch == '/') {
                l++; c = 0;
            }
            else if (isdigit(ch)) {
                c += (ch - '0');
            }
            else {
                matriz[l][c] = ch;
                c++;
            }
        }
        return matriz;
    }

    /*
    [FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"]

    1. e3 a5 2. Qh5 Ra6 3. Qxa5 h5 4. h4 Rh6 5. Qxc7 f6 6. Qxd7+ Kf7 7. Qxb7 Qd3 8. Qxb8 Qh7 9. Qxc8 Kg6
    PGN error: Can't play Rh6 at move 4, ply 8 no lichess pega outra partida melhor
    Movendo a torre no lichess fica assim: 1. e3 a5 2. Qh5 Ra6 3. Qxa5 h5 4. h4 Rhh6; Você errou a lógica dos lances
    */
    // Função auxiliar de "Visão de Raio-X" para desambiguação de lances
    bool podeAlcancar(const std::vector<std::string>& b, int lO, int cO, int lD, int cD, char peca) {
        char p = tolower(peca);
        int dl = abs(lO - lD);
        int dc = abs(cO - cD);

        // Caminho do Cavalo
        if (p == 'n') return (dl == 2 && dc == 1) || (dl == 1 && dc == 2);

        // Caminho da Torre (ou Rainha em linha reta)
        if (p == 'r' || p == 'q') {
            if (lO == lD) {
                int step = (cD > cO) ? 1 : -1;
                for (int c = cO + step; c != cD; c += step) if (b[lO][c] != '.') return false;
                return true;
            }
            if (cO == cD) {
                int step = (lD > lO) ? 1 : -1;
                for (int l = lO + step; l != lD; l += step) if (b[l][cO] != '.') return false;
                return true;
            }
        }
        // Caminho do Bispo (ou Rainha em diagonal)
        if (p == 'b' || p == 'q') {
            if (dl == dc && dl > 0) {
                int stepL = (lD > lO) ? 1 : -1;
                int stepC = (cD > cO) ? 1 : -1;
                int l = lO + stepL, c = cO + stepC;
                while (l != lD && c != cD) {
                    if (b[l][c] != '.') return false;
                    l += stepL; c += stepC;
                }
                return true;
            }
        }
        return false;
    }

    std::string decodificarLanceEntreFens(const std::string& fenAntiga, const std::string& fenNova) {
        std::vector<std::string> bAntigo = expandirMatrizFen(fenAntiga);
        std::vector<std::string> bNovo = expandirMatrizFen(fenNova);

        int lOrigem = -1, cOrigem = -1, lDest = -1, cDest = -1;
        char pecaMovida = '.', pecaNoDestino = '.';

        // 1. Identifica o que mudou
        for (int l = 0; l < 8; l++) {
            for (int c = 0; c < 8; c++) {
                if (bAntigo[l][c] != '.' && bNovo[l][c] == '.') {
                    lOrigem = l; cOrigem = c;
                    pecaMovida = bAntigo[l][c];
                }
                else if (bNovo[l][c] != bAntigo[l][c] && bNovo[l][c] != '.') {
                    lDest = l; cDest = c;
                    pecaNoDestino = bNovo[l][c];
                }
            }
        }

        if (lOrigem == -1 || lDest == -1) return "err err";

        char colO = 'a' + cOrigem;  char linO = '8' - lOrigem;
        char colD = 'a' + cDest;    char linD = '8' - lDest;

        std::string uci = "";
        uci += colO; uci += linO; uci += colD; uci += linD;

        // 2. Roque
        if (tolower(pecaMovida) == 'k' && abs(cOrigem - cDest) > 1) {
            return uci + (cDest > cOrigem ? " O-O" : " O-O-O");
        }

        // 3. Desambiguação (Para não dar erro no Lichess com Torres e Cavalos)
        bool conflitoColuna = false;
        bool conflitoLinha = false;
        char tipoPeca = toupper(pecaMovida);

        if (tipoPeca != 'P' && tipoPeca != 'K') {
            for (int l = 0; l < 8; l++) {
                for (int c = 0; c < 8; c++) {
                    if ((l != lOrigem || c != cOrigem) && bAntigo[l][c] == pecaMovida) {
                        if (podeAlcancar(bAntigo, l, c, lDest, cDest, pecaMovida)) {
                            if (c == cOrigem) conflitoLinha = true;
                            else conflitoColuna = true;
                        }
                    }
                }
            }
        }

        std::string san = "";
        bool captura = (bAntigo[lDest][cDest] != '.');

        // Correção para captura En Passant
        if (tipoPeca == 'P' && cOrigem != cDest && bAntigo[lDest][cDest] == '.') {
            captura = true;
        }

        // 4. Montagem do SAN legível
        if (tipoPeca != 'P') {
            san += tipoPeca;
            if (conflitoColuna) san += colO;
            else if (conflitoLinha) san += linO;
            if (captura) san += "x";
        }
        else {
            if (captura) {
                san += colO;
                san += "x";
            }
        }

        san += colD;
        san += linD;

        // 5. Promoção
        if (tolower(pecaMovida) == 'p' && tolower(pecaNoDestino) != 'p') {
            san += "=";
            san += toupper(pecaNoDestino);
            uci += tolower(pecaNoDestino);
        }

        return uci + " " + san;
    }

    void imprimirPGN(ListaPartida& historico, const std::string& fenInicial) {
        std::cout << "[FEN \"" << fenInicial << "\"]\n\n";

        NoHistorico* temp = historico.cabeca;
        if (temp != nullptr) temp = temp->proximo; // Pula o estado inicial

        int numeroTurno = 1;
        bool turnoBrancas = true;

        while (temp != nullptr) {
            if (turnoBrancas) {
                std::cout << numeroTurno << ". " << temp->lanceSAN << " ";
            }
            else {
                std::cout << temp->lanceSAN << " ";
                numeroTurno++;
            }
            turnoBrancas = !turnoBrancas;
            temp = temp->proximo;
        }
        std::cout << "\n" << std::endl;
    }

    void pergunta() {
        char resp;
        std::cout << "Deseja personalizar o tabuleiro y/n:" << std::endl;
        std::cin >> resp;
        if (resp == 'Y' || resp == 'y') {
            std::string fen;
            std::cout << "Digite o codigo FEN:" << std::endl;
            std::cin.ignore();
            std::getline(std::cin, fen);
            tabuleiroJogo.carregarFEN(fen);
        }
        else {
            tabuleiroJogo.carregarFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
        }
    }
};