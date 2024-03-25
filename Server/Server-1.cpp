#include "Server-1.h"
#pragma comment (lib, "WS2_32.LIB") // 32 ���� ������ 16��Ʈ ���� �������� ����

constexpr short PORT{ 2551 };
constexpr char SERVER_ADDR[]{ "127.0.0.1" }; // �ڱ� �ڽ�
constexpr int BUFSIZE{ 256 };

int main()
{
	Chess hos{ 0, 0 };

	std::wcout.imbue(std::locale("korean")); // �ѱ۷� ���� ���

	// �����쿡���� �ؾ��ϴ� ����. ������ ���� ���α׷����� ȣȯ�� ������ �ʿ���
	// �������� �濪�� ����
	WSADATA WSAData{};
	WSAStartup(MAKEWORD(2, 0), &WSAData);

	SOCKET server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, 0);
	SOCKADDR_IN server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	bind(server_s, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
	listen(server_s, SOMAXCONN);
	int addr_size = sizeof(server_addr);
	SOCKET clinet_s = WSAAccept(server_s, reinterpret_cast<sockaddr*>(&server_addr), &addr_size, nullptr, 0);

	while (1) {
		char buf[BUFSIZE]{};
		WSABUF wsabuf[1];

		// recv
		wsabuf[0].buf = buf;
		wsabuf[0].len = BUFSIZE;
		DWORD recv_size{};
		DWORD recv_flag{ 0 };
		int res = WSARecv(clinet_s, wsabuf, 1, &recv_size, &recv_flag, nullptr, nullptr);
		if (res != 0)
			print_error("WSARecv", WSAGetLastError());
		if (recv_size == 0)
			break;

		for (DWORD i = 0; i < recv_size; ++i)
			std::cout << buf[i];
		std::cout << "\n";

		MoveChess(hos, buf[0]);

		// send
		wsabuf[0].len = recv_size; 
		wsabuf[0].buf[0] = hos.x;
		wsabuf[0].buf[1] = hos.y;

		DWORD sent_size{};
		int sed = WSASend(clinet_s, wsabuf, 1, &sent_size, 0, nullptr, nullptr);
		if (sed != 0)
			print_error("WSASend", WSAGetLastError());
	}
	closesocket(server_s);
	closesocket(clinet_s);
	WSACleanup(); // �������� �̰� ���ص� ��
}