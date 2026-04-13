#pragma once
#include "Tabuleiro.h"
#include "Simulacao.h"
#include <iostream>
#include <string>

class Jogo {
private:
    Tabuleiro tabuleiroJogo;
    char tabuleiroBinario[64];
    char selecP[2];
    char movP[2];

public:
    Jogo() {
        while (1) { //ne2e4e7e5d1h5a7a6f1c4a6a5h5f7
            system("clear");
            pergunta();
            gameLoop();
        }
    }
    ~Jogo() {}

    void gameLoop() {
        while (1) {
            std::cout << "\n--- TURNO: " << tabuleiroJogo.getTurno() << " ---\n";

            bool turnoFinalizado = false;

            // Fica preso neste loop até o jogador realmente executar um lance válido
            while (!turnoFinalizado) {

                // 1. Mostra o tabuleiro normal (sem lances)
                atualizarMapa(false);
                Simulacao::atualizarTabuleiro(tabuleiroBinario);

                // 2. Espera o jogador escolher uma peça válida
                do {
                    std::cout << "Escolha uma peca: ";
                    Simulacao::selecionarPeca(selecP);
                } while (!tabuleiroJogo.preLance(selecP));

                // 3. Mostra o tabuleiro com as opções de movimento marcadas com '2'
                std::cout << "\nOpcoes de movimento:\n";
                atualizarMapa(true);
                Simulacao::atualizarTabuleiro(tabuleiroBinario);

                // 4. Espera o jogador escolher um destino válido ou cancelar
                bool aguardandoDestino = true;
                do {
                    std::cout << "Escolha o destino (ou a mesma casa para cancelar): ";
                    Simulacao::lerMovimento(movP);

                    // Verifica se o jogador escolheu a mesma casa da origem (Cancelar!)
                    if (movP[0] == selecP[0] && movP[1] == selecP[1]) {
                        std::cout << "Selecao cancelada!\n";
                        tabuleiroJogo.cancelarSelecao();
                        aguardandoDestino = false; // Sai do loop de destino
                        // Como turnoFinalizado continua false, ele vai voltar a pedir uma nova peça!
                    }
                    else {
                        // Tenta executar o lance
                        if (tabuleiroJogo.executarLance(movP)) {
                            aguardandoDestino = false; // Sai do loop de destino
                            turnoFinalizado = true;    // Sai do loop do turno (Passa a vez!)
                        }
                        /* else {
                            std::cout << "Movimento invalido. Tente novamente.\n";
                        }*/
                    }
                } while (aguardandoDestino);
                if (tabuleiroJogo.ehXequeMate(tabuleiroJogo.getTurno())) {
                    atualizarMapa(false);
                    Simulacao::atualizarTabuleiro(tabuleiroBinario);
                    std::cout << "\n*** XEQUE-MATE! O jogo acabou. ***\n";
                    system("pause");
                    return;
                }
                else if (tabuleiroJogo.ehEmpate(tabuleiroJogo.getTurno())) {
                    atualizarMapa(false);
                    Simulacao::atualizarTabuleiro(tabuleiroBinario);
                    std::cout << "\n*** EMPATE! O jogo acabou. ***\n";
                    system("pause");
                    return;
                }
            }
        }
    }

    // Função única que constrói o array binário que a Simulacao.h gosta
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
                        tabuleiroBinario[i * 8 + j] = '0'; // 0: Vazio
                    }
                }
            }
        }
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
        else tabuleiroJogo.carregarFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    }
};