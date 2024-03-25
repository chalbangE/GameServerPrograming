#include "SvChess.h"

void SvChess::PrintBoard()
{
	COORD Cur{ 0,0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);

	std::cout << "忙 式 成 式 成 式 成 式 成 式 成 式 成 式 成 式 忖" << std::endl;
	for (int i = 0; i < 8; ++i) {
		for (int k = 0; k < 9; ++k) {
			std::cout << "弛 ";
			if (k != 8 && x == k && y == i)
				std::cout << "Ｏ";
			else
				std::cout << "  ";
		}
		if (i != 7)
			std::cout << std::endl << "戍 式 托 式 托 式 托 式 托 式 托 式 托 式 托 式 扣" << std::endl;
	}
	std::cout << std::endl << "戌 式 扛 式 扛 式 扛 式 扛 式 扛 式 扛 式 扛 式 戎 " << std::endl;
}