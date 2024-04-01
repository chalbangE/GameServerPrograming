#pragma once

#include <iostream>
#include <WS2tcpip.h>
#include <algorithm>

#define MapSize 8
#define UP 38
#define DOWN 40
#define LEFT 37
#define RIGHT 39

#define LOGIN_TYPE 0
#define LOGOUT_TYPE 1
#define KEY_TYPE 2
#define POS_TYPE 3
#define NEW_LOGIN_TYPE 4
#define NEW_LOGOUT_TYPE 5

const static short SERVER_PORT = 2551;
const static int BUFSIZE = 256;

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
	while (true) {
		if (err_no == 0)
			return;
	};
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
	std::cout << hos.x << "   " << hos.y << std::endl;
}
