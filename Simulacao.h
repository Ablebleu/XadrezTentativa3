#ifndef SIMULACAO_H
#define SIMULACAO_H

#include <iostream>

class Simulacao {
public:
	static void atualizarTabuleiro(const char binario[64]) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 8; j++) {
				std::cout << binario[8 * i + j] << " ";
			}
			std::cout << std::endl;
		}
	}

	static void selecionarPeca(char coord[2]) {
		char coordX[2];
		std::cin >> coordX[0] >> coordX[1];
		transfCoords(coordX, coord);
	}

	static void lerMovimento(char coord[2]) {
		char coordX[2];
		std::cin >> coordX[0] >> coordX[1];
		transfCoords(coordX, coord);
	}

	static void transfCoords(const char ln[2], char on[2]) {
		int col = ln[0] - 'a';
		int lin = '8' - ln[1];
		on[0] = lin + '0';
		on[1] = col + '0';
	}
};

#endif
