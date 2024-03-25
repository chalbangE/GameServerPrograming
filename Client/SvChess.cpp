#include "SvChess.h"

void SvChess::PrintBoard()
{
	COORD Cur{ 0,0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);

	std::cout << "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��" << std::endl;
	for (int i = 0; i < 8; ++i) {
		for (int k = 0; k < 9; ++k) {
			std::cout << "�� ";
			if (k != 8 && x == k && y == i)
				std::cout << "��";
			else
				std::cout << "  ";
		}
		if (i != 7)
			std::cout << std::endl << "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� ��" << std::endl;
	}
	std::cout << std::endl << "�� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� �� " << std::endl;
}