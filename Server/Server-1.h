#pragma once
#include <iostream>
#include <WS2tcpip.h>

#define MapSize 8
#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77

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
	switch (dir) {
	case LEFT:
		--hos.x;
		if (hos.x < 0)
			++hos.x;
		break;
	case RIGHT:
		++hos.x;
		if (hos.x >= MapSize)
			--hos.x;
		break;
	case UP:
		--hos.y;
		if (hos.y < 0)
			++hos.y;
		break;
	case DOWN:
		++hos.y;
		if (hos.y >= MapSize)
			--hos.y;
		break;
	}
}
