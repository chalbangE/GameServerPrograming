#pragma once
#include <iostream>
#include <WS2tcpip.h>
#include <algorithm>

#define MapSize 8
#define UP 38
#define DOWN 40
#define LEFT 37
#define RIGHT 39

class Chess {
public:
	int x = 0, y = 0;
};

void print_error(const char* msg, int err_no)
{
	WCHAR* msg_buf{};
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, err_no,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		reinterpret_cast<LPWSTR>(&msg_buf), 0, NULL);
	std::cout << msg;
	std::wcout << L"\t¿¡·¯ : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}

void MoveChess(Chess& hos, char dir) {
	int add_x{}, add_y{};

	switch (dir) {
	case LEFT:
		add_x = -1;
		break;
	case RIGHT:
		add_x = 1;
		break;
	case UP:
		add_y = -1;
		break;
	case DOWN:
		add_y = 1;
		break;
	}

	hos.x = std::clamp(hos.x + add_x, 0, 7);
	hos.y = std::clamp(hos.y + add_y, 0, 7);
}
