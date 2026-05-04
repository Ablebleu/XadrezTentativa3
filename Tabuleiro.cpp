#include "Tabuleiro.h"
#include <algorithm>
#include <sstream>
#include <cctype>

void Tabuleiro::calcularMovimentosPeao(int l, int c, std::vector<Lance>& lista) {
    char minhaPeca = matriz[l][c];
    char minhaCor = obterCorPeca(minhaPeca);

    int direcao = (minhaCor == 'w') ? -1 : 1;     // Brancas sobem (-1), Pretas descem (+1)
    int linhaInicial = (minhaCor == 'w') ? 6 : 1; // Onde o peão começa o jogo

    // --- MOVIMENTOS PARA A FRENTE ---
    int linhaFrente = l + direcao;

    if (posicaoValida(linhaFrente, c) && matriz[linhaFrente][c] == '.') {
        // VERIFICA PROMOÇÃO NO AVANÇO
        if (linhaFrente == 0 || linhaFrente == 7) {
            char prom = (minhaCor == 'w') ? 'Q' : 'q';
            lista.push_back(Lance(l, c, linhaFrente, c, minhaPeca, '.', false, false, prom));
        }
        else {
            lista.push_back(Lance(l, c, linhaFrente, c, minhaPeca, '.'));

            // Passo duplo (só testamos se o passo simples for válido e não for promoção)
            if (l == linhaInicial) {
                int linhaFrenteDupla = l + (2 * direcao);
                if (matriz[linhaFrenteDupla][c] == '.') {
                    lista.push_back(Lance(l, c, linhaFrenteDupla, c, minhaPeca, '.'));
                }
            }
        }
    }

    // --- CAPTURAS NA DIAGONAL (Normal e En Passant) ---
    int colunasCaptura[2] = { c - 1, c + 1 }; // Diagonal Esquerda e Diagonal Direita

    for (int i = 0; i < 2; i++) {
        int colCaptura = colunasCaptura[i];

        if (posicaoValida(linhaFrente, colCaptura)) {
            char pecaDestino = matriz[linhaFrente][colCaptura];
            char corDestino = obterCorPeca(pecaDestino);

            // Captura Normal
            if (corDestino != '.' && corDestino != minhaCor) {
                // VERIFICA PROMOÇÃO NA CAPTURA (Era aqui que estava a faltar!)
                if (linhaFrente == 0 || linhaFrente == 7) {
                    char prom = (minhaCor == 'w') ? 'Q' : 'q';
                    lista.push_back(Lance(l, c, linhaFrente, colCaptura, minhaPeca, pecaDestino, false, false, prom));
                }
                else {
                    lista.push_back(Lance(l, c, linhaFrente, colCaptura, minhaPeca, pecaDestino));
                }
            }
            // Captura En Passant
            // Captura En Passant
            else if (estado.colunaEnPassant == colCaptura) {
                // Verifica se o peão está na linha correta (rank 5 para branco, rank 4 para preto)
                if ((minhaCor == 'w' && l == 3) || (minhaCor == 'b' && l == 4)) {
                    int linhaInimigo = l;
                    char pecaInimiga = matriz[linhaInimigo][colCaptura];
                    lista.push_back(Lance(l, c, linhaFrente, colCaptura, minhaPeca, pecaInimiga, false, true));
                }
            }
        }
    }
}
void Tabuleiro::calcularMovimentosCavalo(int l, int c, std::vector<Lance>& lista) {
    char minhaPeca = matriz[l][c];
    char minhaCor = obterCorPeca(minhaPeca);

    // As 8 combinações em formato de "L" que um cavalo pode fazer
    int saltosLinha[8] = { -2, -2, -1, -1,  1,  1,  2,  2 };
    int saltosColuna[8] = { -1,  1, -2,  2, -2,  2, -1,  1 };

    for (int i = 0; i < 8; i++) {
        int destinoLinha = l + saltosLinha[i];
        int destinoColuna = c + saltosColuna[i];

        if (posicaoValida(destinoLinha, destinoColuna)) {
            char pecaDestino = matriz[destinoLinha][destinoColuna];
            char corDestino = obterCorPeca(pecaDestino);

            if (corDestino != minhaCor) {
                // Montamos o "pacote" do lance e empurramos para a lista!
                Lance novoLance(l, c, destinoLinha, destinoColuna, minhaPeca, pecaDestino);
                lista.push_back(novoLance);
            }
        }
    }
}
void Tabuleiro::calcularMovimentosBispo(int l, int c, std::vector<Lance>& lista) {
    char minhaPeca = matriz[l][c];
    char minhaCor = obterCorPeca(minhaPeca);

    // As 4 diagonais do Bispo
    int dirLinha[4] = { -1, -1,  1,  1 }; // Cima, Cima, Baixo, Baixo
    int dirColuna[4] = { -1,  1, -1,  1 }; // Esquerda, Direita, Esquerda, Direita

    for (int i = 0; i < 4; i++) {
        int linhaDestino = l + dirLinha[i];
        int colunaDestino = c + dirColuna[i];

        // O 'while' faz a peça deslizar até bater num obstáculo (igual à Torre!)
        while (posicaoValida(linhaDestino, colunaDestino)) {
            char pecaDestino = matriz[linhaDestino][colunaDestino];
            char corDestino = obterCorPeca(pecaDestino);

            if (corDestino == '.') {
                lista.push_back(Lance(l, c, linhaDestino, colunaDestino, minhaPeca, '.'));
            }
            else if (corDestino != minhaCor) {
                // Captura e para
                lista.push_back(Lance(l, c, linhaDestino, colunaDestino, minhaPeca, pecaDestino));
                break;
            }
            else {
                // Bateu numa peça da mesma cor, apenas para
                break;
            }

            linhaDestino += dirLinha[i];
            colunaDestino += dirColuna[i];
        }
    }
}
void Tabuleiro::calcularMovimentosTorre(int l, int c, std::vector<Lance>& lista) {
    char minhaPeca = matriz[l][c];
    char minhaCor = obterCorPeca(minhaPeca);

    // As 4 direções da Torre: Cima, Baixo, Esquerda, Direita
    int dirLinha[4] = { -1,  1,  0,  0 };
    int dirColuna[4] = { 0,  0, -1,  1 };

    for (int i = 0; i < 4; i++) {
        int linhaDestino = l + dirLinha[i];
        int colunaDestino = c + dirColuna[i];

        // O 'while' faz a peça deslizar até bater num obstáculo
        while (posicaoValida(linhaDestino, colunaDestino)) {
            char pecaDestino = matriz[linhaDestino][colunaDestino];
            char corDestino = obterCorPeca(pecaDestino);

            if (corDestino == '.') {
                // Casa vazia: pode mover e continua a deslizar
                lista.push_back(Lance(l, c, linhaDestino, colunaDestino, minhaPeca, '.'));
            }
            else if (corDestino != minhaCor) {
                // Bateu num inimigo: pode capturar, mas o raio PARA aqui (break)
                lista.push_back(Lance(l, c, linhaDestino, colunaDestino, minhaPeca, pecaDestino));
                break;
            }
            else {
                // Bateu num amigo: não pode capturar e o raio PARA aqui (break)
                break;
            }

            // Vai para a próxima casa na mesma direção
            linhaDestino += dirLinha[i];
            colunaDestino += dirColuna[i];
        }
    }
}
void Tabuleiro::calcularMovimentosRainha(int l, int c, std::vector<Lance>& lista) {
    char minhaPeca = matriz[l][c];
    char minhaCor = obterCorPeca(minhaPeca);

    // As 8 direções: 4 retas + 4 diagonais
    int dirLinha[8] = { -1,  1,  0,  0, -1, -1,  1,  1 };
    int dirColuna[8] = { 0,  0, -1,  1, -1,  1, -1,  1 };

    for (int i = 0; i < 8; i++) {
        int linhaDestino = l + dirLinha[i];
        int colunaDestino = c + dirColuna[i];

        // O 'while' faz a peça deslizar como um "raio" até bater em algo
        while (posicaoValida(linhaDestino, colunaDestino)) {
            char pecaDestino = matriz[linhaDestino][colunaDestino];
            char corDestino = obterCorPeca(pecaDestino);

            if (corDestino == '.') {
                // Casa vazia: adiciona o lance e continua a deslizar
                lista.push_back(Lance(l, c, linhaDestino, colunaDestino, minhaPeca, '.'));
            }
            else if (corDestino != minhaCor) {
                // Bateu num inimigo: adiciona o lance de captura, mas PARA o raio
                lista.push_back(Lance(l, c, linhaDestino, colunaDestino, minhaPeca, pecaDestino));
                break;
            }
            else {
                // Bateu numa peça da mesma cor: não pode capturar, apenas PARA o raio
                break;
            }

            // Avança para a próxima casa na mesma direção
            linhaDestino += dirLinha[i];
            colunaDestino += dirColuna[i];
        }
    }
}
void Tabuleiro::calcularMovimentosRei(int l, int c, std::vector<Lance>& lista) {
    char minhaPeca = matriz[l][c];
    char cor = obterCorPeca(minhaPeca);

    // --- 1. LANCES NORMAIS DO REI (1 passo em 8 direções) ---
    int dirLinha[8] = { -1,  1,  0,  0, -1, -1,  1,  1 };
    int dirColuna[8] = { 0,  0, -1,  1, -1,  1, -1,  1 };

    for (int i = 0; i < 8; i++) {
        int destinoLinha = l + dirLinha[i];
        int destinoColuna = c + dirColuna[i];

        if (posicaoValida(destinoLinha, destinoColuna)) {
            char pecaDestino = matriz[destinoLinha][destinoColuna];
            char corDestino = obterCorPeca(pecaDestino);

            // Pode mover-se se a casa estiver vazia ou se tiver uma peça inimiga
            if (corDestino != cor) {
                Lance novoLance(l, c, destinoLinha, destinoColuna, minhaPeca, pecaDestino);
                lista.push_back(novoLance);
            }
        }
    }

    // --- 2. LÓGICA DE ROQUE (Padrão e Xadrez 960) ---
    // Regra 1: Não pode fazer roque se já estiver em xeque!
    if (reiEmXeque(cor)) return;

    bool direitoPequeno = (cor == 'w') ? estado.roqueBrancoRei : estado.roquePretoRei;
    bool direitoGrande = (cor == 'w') ? estado.roqueBrancoRainha : estado.roquePretoRainha;

    // Destinos fixos do xadrez (mesmo no 960 o Rei e a Torre acabam sempre nestas colunas)
    int cFinalReiPequeno = 6;   // Coluna 'g'
    int cFinalTorrePequeno = 5; // Coluna 'f'
    int cFinalReiGrande = 2;    // Coluna 'c'
    int cFinalTorreGrande = 3;  // Coluna 'd'

    // Varre a linha do Rei procurando as Torres aliadas
    for (int cT = 0; cT < 8; cT++) {
        char peca = matriz[l][cT];

        // Se não for uma torre da mesma cor, ignora
        if (tolower(peca) != 'r' || obterCorPeca(peca) != cor) continue;

        bool ehRoquePequeno = (cT > c); // Torre está à direita do Rei
        bool ehRoqueGrande = (cT < c);  // Torre está à esquerda do Rei

        if (ehRoquePequeno && !direitoPequeno) continue;
        if (ehRoqueGrande && !direitoGrande) continue;

        int cDestRei = ehRoquePequeno ? cFinalReiPequeno : cFinalReiGrande;
        int cDestTorre = ehRoquePequeno ? cFinalTorrePequeno : cFinalTorreGrande;

        // Passo A: Checar se o caminho entre o Rei e a Torre está livre de outras peças
        bool caminhoLivre = true;
        int minC = std::min(c, cT);
        int maxC = std::max(c, cT);

        // Mudamos a variável para 'col' para não dar conflito com o parâmetro 'c'
        for (int col = minC + 1; col < maxC; col++) {
            if (matriz[l][col] != '.') caminhoLivre = false;
        }

        if (caminhoLivre) {
            // Passo B: Checar se as casas finais de destino estão livres 
            // (Ignoramos se o destino for a própria casa atual do Rei ou da Torre, comum no 960)
            bool destinoLivre = true;
            if (matriz[l][cDestRei] != '.' && cDestRei != c && cDestRei != cT) destinoLivre = false;
            if (matriz[l][cDestTorre] != '.' && cDestTorre != c && cDestTorre != cT) destinoLivre = false;

            if (destinoLivre) {
                // Passo C: Regra de Ouro - As casas por onde o Rei passa NÃO podem estar sob ataque (xeque)
                bool casasSeguras = true;
                int passoC = (cDestRei > c) ? 1 : -1;
                int cAtual = c;

                while (cAtual != cDestRei) {
                    cAtual += passoC;

                    // FAKE MOVE: Move o rei temporariamente na matriz para testar o xeque nessa casa
                    char pecaDestAntiga = matriz[l][cAtual];
                    matriz[l][cAtual] = matriz[l][c];
                    matriz[l][c] = '.';

                    if (reiEmXeque(cor)) casasSeguras = false;

                    // DESFAZ O FAKE MOVE (devolve o rei ao lugar original)
                    matriz[l][c] = matriz[l][cAtual];
                    matriz[l][cAtual] = pecaDestAntiga;

                    if (!casasSeguras) break; // Se foi atacado, nem precisa testar o resto do caminho
                }

                if (casasSeguras) {
                    // LANCE DE ROQUE VÁLIDO! 
                    Lance lanceRoque(l, c, l, cT, minhaPeca, peca, true, false, '.');
                    lista.push_back(lanceRoque);
                }
            }
        }
    }
}

bool Tabuleiro::casaEstaAtacada(int l, int c, char corAtacante) {
    // 1. Ameaças de PEÃO
    // Pense comigo: se o atacante é Branco ('w'), os peões dele sobem a matriz (-1).
    // Logo, para saber se um peão branco me está a atacar agora, eu tenho que olhar para "baixo" (+1).
    int dirPeao = (corAtacante == 'w') ? 1 : -1;
    char peaoAtacante = (corAtacante == 'w') ? 'P' : 'p';

    if (posicaoValida(l + dirPeao, c - 1) && matriz[l + dirPeao][c - 1] == peaoAtacante) return true;
    if (posicaoValida(l + dirPeao, c + 1) && matriz[l + dirPeao][c + 1] == peaoAtacante) return true;

    // 2. Ameaças de CAVALO (Olhamos em formato de 'L')
    char cavaloAtacante = (corAtacante == 'w') ? 'N' : 'n';
    int saltosL[8] = { -2, -2, -1, -1, 1, 1, 2, 2 };
    int saltosC[8] = { -1, 1, -2, 2, -2, 2, -1, 1 };

    for (int i = 0; i < 8; i++) {
        int tempL = l + saltosL[i];
        int tempC = c + saltosC[i];
        if (posicaoValida(tempL, tempC) && matriz[tempL][tempC] == cavaloAtacante) return true;
    }

    // 3. Ameaças DESLIZANTES (Torre, Bispo e Rainha)
    int dirL[8] = { -1, 1, 0, 0, -1, -1, 1, 1 }; // Índices 0-3 são Retas, 4-7 são Diagonais
    int dirC[8] = { 0, 0, -1, 1, -1, 1, -1, 1 };
    char torreAtacante = (corAtacante == 'w') ? 'R' : 'r';
    char bispoAtacante = (corAtacante == 'w') ? 'B' : 'b';
    char rainhaAtacante = (corAtacante == 'w') ? 'Q' : 'q';

    for (int i = 0; i < 8; i++) {
        int tempL = l + dirL[i];
        int tempC = c + dirC[i];

        while (posicaoValida(tempL, tempC)) {
            char p = matriz[tempL][tempC];

            if (p != '.') { // Bateu em algo
                if (obterCorPeca(p) == corAtacante) { // É um inimigo?
                    bool ehReta = (i < 4); // Se i < 4, o raio está numa reta (Torre/Rainha)
                    if (ehReta && (p == torreAtacante || p == rainhaAtacante)) return true;
                    if (!ehReta && (p == bispoAtacante || p == rainhaAtacante)) return true;
                }
                break; // Se bateu noutra peça qualquer (nossa ou deles que não ataca), bloqueia a visão
            }
            tempL += dirL[i];
            tempC += dirC[i];
        }
    }

    // 4. Ameaças de REI (Impede que os dois reis fiquem encostados)
    char reiAtacante = (corAtacante == 'w') ? 'K' : 'k';
    for (int i = 0; i < 8; i++) {
        int tempL = l + dirL[i];
        int tempC = c + dirC[i];
        if (posicaoValida(tempL, tempC) && matriz[tempL][tempC] == reiAtacante) return true;
    }

    // Se olhámos em todas as direções e nada nos atacou:
    return false;
}
bool Tabuleiro::reiEmXeque(char corRei) {
    char rei = (corRei == 'w') ? 'K' : 'k';
    char corInimiga = (corRei == 'w') ? 'b' : 'w';

    // Procura o Rei na matriz
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (matriz[i][j] == rei) {
                // Achou o rei! Vê se a casa dele está sob a mira da cor inimiga
                return casaEstaAtacada(i, j, corInimiga);
            }
        }
    }

    return false; // (Por segurança, caso o rei não seja encontrado na matriz)
}

void Tabuleiro::fazerLanceInterno(const Lance& lance) {
    // Guarda o estado para poder desfazer
    historicoEstados.push_back(estado);

    char pecaMovida = lance.pecaMovida;
    char pecaCapturada = lance.pecaCapturada;
    int linO = lance.linOrigem, colO = lance.colOrigem;
    int linD = lance.linDestino, colD = lance.colDestino;

    // Remove a peça da origem
    matriz[linO][colO] = '.';

    // Coloca a peça no destino (ou a peça promovida)
    if (lance.pecaPromocao != '.') {
        matriz[linD][colD] = lance.pecaPromocao;
    }
    else {
        matriz[linD][colD] = pecaMovida;
    }

    // --- TRATAMENTO DO ROQUE ---
    if (lance.ehRoque) {
        // Move também a torre
        bool roquePequeno = (colD > colO);
        int colTorreOrigem, colTorreDestino;
        if (roquePequeno) {
            colTorreOrigem = 7;          // coluna 'h'
            colTorreDestino = 5;         // coluna 'f'
        }
        else {
            colTorreOrigem = 0;          // coluna 'a'
            colTorreDestino = 3;         // coluna 'd'
        }
        matriz[linO][colTorreOrigem] = '.';
        matriz[linO][colTorreDestino] = (turnoAtual == 'w') ? 'R' : 'r';
    }

    // --- TRATAMENTO DO EN PASSANT ---
    if (lance.ehEnPassant) {
        // O peão capturado está na mesma linha da origem, coluna de destino
        matriz[linO][colD] = '.';
    }

    // --- ACTUALIZAÇÃO DOS DIREITOS DE ROQUE ---
    // Se o rei se moveu, perdem-se ambos os roques dessa cor
    if (tolower(pecaMovida) == 'k') {
        if (turnoAtual == 'w') {
            estado.roqueBrancoRei = false;
            estado.roqueBrancoRainha = false;
        }
        else {
            estado.roquePretoRei = false;
            estado.roquePretoRainha = false;
        }
    }
    // Se uma torre se moveu, perde-se o roque desse lado
    if (tolower(pecaMovida) == 'r') {
        if (turnoAtual == 'w') {
            if (colO == 7) estado.roqueBrancoRei = false;
            if (colO == 0) estado.roqueBrancoRainha = false;
        }
        else {
            if (colO == 7) estado.roquePretoRei = false;
            if (colO == 0) estado.roquePretoRainha = false;
        }
    }
    // Se uma torre foi capturada na sua casa original, também se perde o roque
    if (pecaCapturada == 'R') {
        if (colD == 7) estado.roqueBrancoRei = false;
        if (colD == 0) estado.roqueBrancoRainha = false;
    }
    else if (pecaCapturada == 'r') {
        if (colD == 7) estado.roquePretoRei = false;
        if (colD == 0) estado.roquePretoRainha = false;
    }

    // --- ACTUALIZAÇÃO DA COLUNA DE EN PASSANT ---
    // Só fica activo se um peão avançou duas casas
    if (tolower(pecaMovida) == 'p' && abs(linD - linO) == 2) {
        estado.colunaEnPassant = colD;
    }
    else {
        estado.colunaEnPassant = -1;
    }

    // --- ACTUALIZAÇÃO DO CONTADOR DE MEIAS-JOGADAS (regra dos 50 lances) ---
    if (tolower(pecaMovida) == 'p' || pecaCapturada != '.') {
        estado.meiaJogadas = 0;   // reset porque foi um avanço de peão ou captura
    }
    else {
        estado.meiaJogadas++;
    }

    // ⬇️⬇️⬇️ MUDA O TURNO (aqui estava o problema) ⬇️⬇️⬇️
    turnoAtual = (turnoAtual == 'w') ? 'b' : 'w';
}
void Tabuleiro::desfazerLanceInterno(const Lance& lance) {
    // 1. Destrocar o turno
    turnoAtual = (turnoAtual == 'w') ? 'b' : 'w';

    // 2. Restaurar a matriz
    if (lance.ehRoque) {
        bool ehRoquePequeno = (lance.colDestino > lance.colOrigem);
        int cFinalRei = ehRoquePequeno ? 6 : 2;
        int cFinalTorre = ehRoquePequeno ? 5 : 3;

        // Limpa as posições onde eles pararam no roque
        matriz[lance.linOrigem][cFinalRei] = '.';
        matriz[lance.linOrigem][cFinalTorre] = '.';

        // Restaura o Rei e a Torre às posições originais
        matriz[lance.linOrigem][lance.colOrigem] = lance.pecaMovida;
        matriz[lance.linDestino][lance.colDestino] = lance.pecaCapturada; // No roque, 'pecaCapturada' armazena a Torre
    }
    else {
        // Devolver a peça movida à origem
        matriz[lance.linOrigem][lance.colOrigem] = lance.pecaMovida;

        // Devolver a peça capturada ao destino (se não comeu nada, devolve o '.')
        matriz[lance.linDestino][lance.colDestino] = lance.pecaCapturada;

        // CORREÇÃO DO EN PASSANT: Ressuscita o peão inimigo no lugar certo
        if (lance.ehEnPassant) {
            // A casa de destino real volta a ficar vazia
            matriz[lance.linDestino][lance.colDestino] = '.';

            // O peão inimigo reaparece na linha de origem do nosso peão, mas na coluna de destino
            char peaoInimigo = (obterCorPeca(lance.pecaMovida) == 'w') ? 'p' : 'P';
            matriz[lance.linOrigem][lance.colDestino] = peaoInimigo;
        }
    }

    // 3. Voltar o estado invisível para a "fotografia" anterior
    estado = historicoEstados.back();
    historicoEstados.pop_back();
}

Tabuleiro::Tabuleiro() {
    //Só para ter certeza que não vai dar merda.
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            matriz[i][j] = '.';
    carregarFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}
Tabuleiro::~Tabuleiro() {}

std::vector<Lance> Tabuleiro::obterMovimentosLegaisDaPeca(int linha, int coluna) {
    std::vector<Lance> movimentosPossiveis; // Lances "burros" (podem ser ilegais)
    std::vector<Lance> movimentosLegais;    // Lances testados e aprovados

    char peca = matriz[linha][coluna];
    char minhaCor = obterCorPeca(peca);

    // Se o jogador levantou uma casa vazia, não há lances
    if (minhaCor == '.') {
        return movimentosLegais;
    }

    // 1. Gera todos os movimentos pseudo-legais dependendo da peça
    switch (peca) {
        case 'P': case 'p': calcularMovimentosPeao(linha, coluna, movimentosPossiveis); break;
        case 'N': case 'n': calcularMovimentosCavalo(linha, coluna, movimentosPossiveis); break;
        case 'B': case 'b': calcularMovimentosBispo(linha, coluna, movimentosPossiveis); break;
        case 'R': case 'r': calcularMovimentosTorre(linha, coluna, movimentosPossiveis); break;
        case 'Q': case 'q': calcularMovimentosRainha(linha, coluna, movimentosPossiveis); break;
        case 'K': case 'k': calcularMovimentosRei(linha, coluna, movimentosPossiveis); break;
    }

    // 2. Filtra os movimentos testando o futuro
    for (int i = 0; i < movimentosPossiveis.size(); i++) {
        Lance lanceTeste = movimentosPossiveis[i];

        fazerLanceInterno(lanceTeste); // Viaja para o futuro

        // O nosso rei sobreviveu a este lance?
        // Se NÃO estamos em xeque neste futuro, o lance é legal!
        if (reiEmXeque(minhaCor) == false) {
            movimentosLegais.push_back(lanceTeste);
        }

        desfazerLanceInterno(lanceTeste); // Volta ao presente
    }

    return movimentosLegais;
}

bool Tabuleiro::semLances(char cor) {
    // Percorre o tabuleiro inteiro à procura das peças da cor atual
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (obterCorPeca(matriz[i][j]) == cor) {
                // Pede os movimentos legais (já com a verificação de xeque) para esta peça
                std::vector<Lance> lances = obterMovimentosLegaisDaPeca(i, j);

                // Se esta peça tiver pelo menos 1 lance válido, o jogador não está sem lances
                if (!lances.empty()) {
                    return false;
                }
            }
        }
    }
    // Se varreu o tabuleiro inteiro e nenhuma peça gerou lances válidos:
    return true;
}
bool Tabuleiro::ehXequeMate(char cor) {
    // É Mate apenas se: O Rei está sob ataque E o jogador não tem lances legais
    return reiEmXeque(cor) && semLances(cor);
}
bool Tabuleiro::ehEmpate(char cor) {
    // O Empate por afogamento (Stalemate) acontece quando:
    // O Rei NÃO está em xeque, MAS o jogador não tem lances legais.
    if (!reiEmXeque(cor) && semLances(cor)) {
        return true;
    }

    // Regra dos 50 movimentos (50 lances = 100 meias-jogadas)
    if (estado.meiaJogadas >= 100) {
        return true;
    }

    return false;
}

char Tabuleiro::obterCorPeca(char peca) {
    if (peca == '.') return '.'; // Casa vazia não tem cor
    if (peca >= 'A' && peca <= 'Z') return 'w'; // Maiúsculas são brancas
    return 'b'; // Se não é vazio nem maiúscula, só pode ser minúscula (preta)
}
bool Tabuleiro::posicaoValida(int linha, int coluna) {
    return (linha >= 0 && linha < 8 && coluna >= 0 && coluna < 8);
}
void Tabuleiro::imprimirNoConsole() {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            std::cout << matriz[i][j];
        }
        std::cout << std::endl;
    }
}
void Tabuleiro::carregarFEN(const std::string& fen) {
    // Cria um "leitor de string" que facilita quebrar o texto pelos espaços
    std::istringstream leitor(fen);
    std::string pecas, turno, roque, enPassant, meiasJogadasStr, numJogadasStr;

    // 1. Extrai os 6 blocos do FEN (ele corta automaticamente em cada espaço)
    leitor >> pecas >> turno >> roque >> enPassant >> meiasJogadasStr >> numJogadasStr;

    // --- PARTE 1: PREENCHER A MATRIZ DE PEÇAS ---
    int linha = 0;   // 0 representa a linha 8 do tabuleiro real (o topo)
    int coluna = 0;  // 0 representa a coluna 'a' (a esquerda)

    for (char c : pecas) {
        if (c == '/') {
            // Pula para a próxima linha do tabuleiro
            linha++;
            coluna = 0;
        }
        else if (isdigit(c)) {
            // Se for um número (1 a 8), significa que são casas vazias
            int casasVazias = c - '0'; // Truque ninja do C++ para converter char em int
            for (int i = 0; i < casasVazias; i++) {
                matriz[linha][coluna] = '.';
                coluna++;
            }
        }
        else {
            // Se for uma letra (p, P, k, K...), é uma peça
            matriz[linha][coluna] = c;
            coluna++;
        }
    }

    // --- PARTE 2: DE QUEM É A VEZ ---
    turnoAtual = turno[0];

    // --- PARTE 3: DIREITOS DE ROQUE ---
    // Primeiro, assumimos que ninguém pode fazer Roque
    estado.roqueBrancoRei = false;
    estado.roqueBrancoRainha = false;
    estado.roquePretoRei = false;
    estado.roquePretoRainha = false;

    // Se a string for diferente de "-", ativamos os direitos que estiverem lá
    if (roque != "-") {
        for (char c : roque) {
            if (c == 'K') estado.roqueBrancoRei = true;
            if (c == 'Q') estado.roqueBrancoRainha = true;
            if (c == 'k') estado.roquePretoRei = true;
            if (c == 'q') estado.roquePretoRainha = true;
        }
    }

    // --- PARTE 4: EN PASSANT ---
    // O FEN passa a casa de destino do En Passant (ex: "e3" ou "c6").
    // Para a nossa lógica, só precisamos saber qual é a COLUNA.
    if (enPassant != "-") {
        char letraColuna = enPassant[0]; // Pega apenas a primeira letra (ex: o 'e' do "e3")
        estado.colunaEnPassant = letraColuna - 'a'; // Converte 'a' em 0, 'b' em 1, 'c' em 2...
    }
    else {
        estado.colunaEnPassant = -1; // -1 significa "não há En Passant disponível neste turno"
    }

    // --- PARTE 5: REGRA DOS 50 MOVIMENTOS (Meias Jogadas) ---
    // Converte a string numérica para inteiro (std::stoi)
    if (!meiasJogadasStr.empty()) {
        estado.meiaJogadas = std::stoi(meiasJogadasStr);
    }
    else {
        estado.meiaJogadas = 0;
    }

    // A Parte 6 (Número total de lances do jogo) geralmente é ignorada na lógica interna 
    // do motor durante a busca de lances, então não precisamos guardá-la na struct.

    // Por fim, como carregamos um tabuleiro novo, limpamos o histórico de lances passados
    historicoEstados.clear();
}
std::string Tabuleiro::obterFEN() {
    std::string fen = "";

    // --- PARTE 1: POSIÇÃO DAS PEÇAS ---
    for (int linha = 0; linha < 8; linha++) {
        int casasVazias = 0;

        for (int coluna = 0; coluna < 8; coluna++) {
            char peca = matriz[linha][coluna];

            if (peca == '.') {
                casasVazias++; // Conta as casas vazias consecutivas
            }
            else {
                // Se encontrámos uma peça, e tínhamos casas vazias antes, escrevemos o número primeiro
                if (casasVazias > 0) {
                    fen += std::to_string(casasVazias);
                    casasVazias = 0; // Reseta o contador
                }
                fen += peca; // Adiciona a letra da peça (P, r, K, etc.)
            }
        }

        // Se a linha terminou com casas vazias, adicionamos esse número ao final da linha
        if (casasVazias > 0) {
            fen += std::to_string(casasVazias);
        }

        // Adiciona a barra '/' para separar as linhas, exceto na última linha (linha 7)
        if (linha < 7) {
            fen += "/";
        }
    }

    // --- PARTE 2: DE QUEM É A VEZ ---
    fen += " ";
    if (turnoAtual == 'w') {
        fen += "w";
    }
    else {
        fen += "b";
    }

    // --- PARTE 3: DIREITOS DE ROQUE ---
    fen += " ";
    std::string roque = "";
    if (estado.roqueBrancoRei) roque += "K";
    if (estado.roqueBrancoRainha) roque += "Q";
    if (estado.roquePretoRei) roque += "k";
    if (estado.roquePretoRainha) roque += "q";

    if (roque.empty()) {
        fen += "-"; // Se ninguém pode fazer roque, adiciona o hífen
    }
    else {
        fen += roque;
    }

    // --- PARTE 4: EN PASSANT ---
    fen += " ";
    if (estado.colunaEnPassant != -1) {
        // Converte o número da coluna (0 a 7) para a letra correspondente (a até h)
        char letraColuna = 'a' + estado.colunaEnPassant;

        // Se for a vez das Brancas, significa que as Pretas acabaram de mover um peão, 
        // logo a casa alvo do en passant é na linha algébrica '6'
        char letraLinha = (turnoAtual == 'w') ? '6' : '3';

        fen += letraColuna;
        fen += letraLinha;
    }
    else {
        fen += "-";
    }

    // --- PARTE 5 E 6: MEIAS JOGADAS E NÚMERO DO LANCE ---
    fen += " ";
    fen += std::to_string(estado.meiaJogadas);

    // O número de lances totais não afeta a lógica do motor de procura, 
    // por isso os motores simples costumam deixar sempre "1" para fechar a string.
    fen += " 1";

    return fen;
}

bool Tabuleiro::preLance(char coord[2]) {
    int l = coord[0] - '0';
    int c = coord[1] - '0';

    if (!posicaoValida(l, c)) return false;

    char peca = matriz[l][c];
    char corPeca = obterCorPeca(peca);

    // Só deixa selecionar se for a peça do jogador atual
    if (corPeca != turnoAtual) {
        return false;
    }

    // Calcula os lances reais (com defesa de xeque e tudo!)
    lancesPecaSelecionada = obterMovimentosLegaisDaPeca(l, c);

    if (lancesPecaSelecionada.empty()) {
        return false; // Peça bloqueada, não pode ser selecionada
    }

    linhaSelecionada = l;
    colunaSelecionada = c;
    return true; // Seleção válida!
}
bool Tabuleiro::executarLance(char coord[2]) {
    int lDestino = coord[0] - '0';
    int cDestino = coord[1] - '0';

    // Procura se a casa de destino escolhida está na lista de lances válidos
    for (int i = 0; i < lancesPecaSelecionada.size(); i++) {
        if (lancesPecaSelecionada[i].linDestino == lDestino &&
            lancesPecaSelecionada[i].colDestino == cDestino) {

            // Achou o lance! Executa a jogada na matriz
            fazerLanceInterno(lancesPecaSelecionada[i]);

            // Limpa a seleção para a próxima jogada
            linhaSelecionada = -1;
            colunaSelecionada = -1;
            lancesPecaSelecionada.clear();
            return true;
        }
    }
    return false; // Movimento inválido
}
bool Tabuleiro::ehCasaDestinoValida(int l, int c) const {
    for (int i = 0; i < lancesPecaSelecionada.size(); i++) {
        if (lancesPecaSelecionada[i].linDestino == l &&
            lancesPecaSelecionada[i].colDestino == c) {
            return true;
        }
    }
    return false;
}

void Tabuleiro::cancelarSelecao() {
    // Limpa a memória da peça que estava levantada
    linhaSelecionada = -1;
    colunaSelecionada = -1;
    lancesPecaSelecionada.clear();
}
bool Tabuleiro::ehCasaCaptura(int l, int c) const {
    for (int i = 0; i < lancesPecaSelecionada.size(); i++) {
        if (lancesPecaSelecionada[i].linDestino == l && lancesPecaSelecionada[i].colDestino == c) {
            // É captura se houver uma peça inimiga OU se for um En Passant!
            if (lancesPecaSelecionada[i].pecaCapturada != '.' || lancesPecaSelecionada[i].ehEnPassant) {
                return true;
            }
        }
    }
    return false;
}