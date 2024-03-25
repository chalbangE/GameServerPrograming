#include <iostream>
#include <WS2tcpip.h>
#include <conio.h>
#include <String>
#pragma comment (lib, "WS2_32.LIB") // 32 쓰는 이유는 16비트 쓰던 구시절의 잔재

constexpr short PORT{ 2551 };
constexpr int BUFSIZE{ 256 };

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
	std::wcout << L"\t에러 : " << msg_buf;
	while (true);
	LocalFree(msg_buf);
}

void MoveCursor() { // 커서를 맨 앞으로 이동시켜 콘솔창 클리어
	COORD Cur{ 0,0 };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

void PrintBoard(const Chess& hos);

int main()
{
	std::wcout.imbue(std::locale("korean")); // 한글로 오류 출력

	// 윈도우에서만 해야하는 삽질. 옛날에 만든 프로그램과의 호환성 때문에 필요함
	// 빌게이츠 흑역사 ㅋㅋ
	WSADATA WSAData{}; 
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	char SERVER_ADDR[BUFSIZE]{ }; // 127.0.0.1  자기 자신
	std::cout << "Enter Server ID : ";
	std::cin >> SERVER_ADDR;
	inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);

	// connect
	connect(server_s, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));

	while (1) {
		char buf[BUFSIZE]{};
		while (1) {
			if (_kbhit()) {        //키보드 입력 확인 (true / false)
				char dir = _getch();      // 방향키 입력시 224 --이 들어오게 되기에 앞에 있는 값 224를 없앰
				if (dir == -32) {    // -32로 입력되면
					dir = _getch();  // 새로 입력값을 판별하여 상하좌우 출력
					buf[0] = dir;
					buf[1] = '\0';
					break;
				}
			}
		}

		MoveCursor();

		// send
		WSABUF wsabuf[1];
		wsabuf[0].buf = buf;
		wsabuf[0].len = static_cast<int>(strlen(buf)) + 1; // 끝에 0까지 보내야함 \0
		if (wsabuf[0].len == 1)
			break;
		DWORD sent_size{};
		int sed = WSASend(server_s, wsabuf, 1, &sent_size, 0, nullptr, nullptr);
		if (sed != 0)
			print_error("WSASend", WSAGetLastError());

		// recv
		wsabuf[0].len = BUFSIZE;
		DWORD recv_size{};
		DWORD recv_flag{ 0 };
		int res = WSARecv(server_s, wsabuf, 1, &recv_size, &recv_flag, nullptr, nullptr);		
		if (res != 0)
			print_error("WSARecv", WSAGetLastError());
		Chess hos{ wsabuf[0].buf[0],  wsabuf[0].buf[1] };
		PrintBoard(hos);
	}

	closesocket(server_s);
	WSACleanup(); // 리눅스는 이거 안해도 됨
}

void PrintBoard(const Chess& hos)
{
	std::cout << "┌ ─ ┬ ─ ┬ ─ ┬ ─ ┬ ─ ┬ ─ ┬ ─ ┬ ─ ┐" << std::endl;
	for (int i = 0; i < 8; ++i) {
		for (int k = 0; k < 9; ++k) {
			std::cout << "│ ";
			if (k != 8 && hos.x == k && hos.y == i)
				std::cout << "♬";
			else
				std::cout << "  ";
		}
		if (i != 7)
			std::cout << std::endl << "├ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┼ ─ ┤" << std::endl;
	}
	std::cout << std::endl << "└ ─ ┴ ─ ┴ ─ ┴ ─ ┴ ─ ┴ ─ ┴ ─ ┴ ─ ┘ " << std::endl;
}