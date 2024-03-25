#include <iostream>
#include <WS2tcpip.h>
#pragma comment (lib, "WS2_32.LIB")
constexpr char SERVER_ADDR[] = "127.0.0.1";
constexpr short SERVER_PORT = 4000;
constexpr int BUFSIZE = 256;


WSABUF wsabuf[1]; // �Ź� ȣ���ϴ� �ͺ��� �������� �����ؼ� ���
char buf[BUFSIZE];
SOCKET server_s;
WSAOVERLAPPED wsaover;   // read_n_send ���� ���������� ������� �Լ��� ����Ǹ�
// �ּҰ��� �����ذ��� �ǹ̰� ����
bool bshutdown{ false };


void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag);

void read_n_send()
{
    std::cout << "Enter Message: ";
    std::cin.getline(buf, BUFSIZE);
    wsabuf[0].buf = buf;
    wsabuf[0].len = static_cast<int>(strlen(buf)) + 1;
    if (wsabuf[0].len == 1) {
        bshutdown = true;
        return;
    }
    ZeroMemory(&wsaover, sizeof(wsaover));   // �ʱ�ȭ
    WSASend(server_s, wsabuf, 1, nullptr, 0, &wsaover, send_callback);
}

void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
    // ���� wsarecv ������ �ϴ� ���� ���⼭ ó������
    for (unsigned int i = 0; i < recv_size; ++i) {
        std::cout << buf[i];
    }
    std::cout << std::endl;
    read_n_send();
}
void CALLBACK send_callback(DWORD err, DWORD sent_size, LPWSAOVERLAPPED pwsaover, DWORD sendflag)
{
    // ���� wsasend ������ �ϴ� ���� ���⼭ ó������
    ZeroMemory(pwsaover, sizeof(pwsaover));   // recv���� ����ϱ� ���� �ʱ�ȭ
    wsabuf[0].len = BUFSIZE;
    DWORD recv_flag = 0;
    WSARecv(server_s, wsabuf, 1, nullptr, &recv_flag, pwsaover, recv_callback);
}

int main()
{
    std::wcout.imbue(std::locale("Korean"));

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);
    server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    SOCKADDR_IN server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_ADDR, &server_addr.sin_addr);
    connect(server_s, reinterpret_cast<sockaddr*>(&server_addr), sizeof(server_addr));
    read_n_send();
    while (false == bshutdown) {
        SleepEx(0, TRUE);
    }
    closesocket(server_s);
    WSACleanup();

}