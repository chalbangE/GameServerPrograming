#include <iostream>
#include <WS2tcpip.h>
#include <unordered_map> // 검색속도가 map보다 훨~씬 빠름 O(N) = 1임

#pragma comment (lib, "WS2_32.LIB")

const short SERVER_PORT = 4000;
const int BUFSIZE = 256;


void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pover, DWORD recv_flag);
void CALLBACK send_callback(DWORD err, DWORD send_size, LPWSAOVERLAPPED pover, DWORD send_flag);
void print_error(const char* msg, int err_no);

std::unordered_map<LPWSAOVERLAPPED, int> g_session_map;

class SESSION {
    char buf[BUFSIZE];
    WSABUF wsabuf[1];
    SOCKET client_s;
    WSAOVERLAPPED over;

public:
    SESSION(SOCKET s, int my_id) : client_s(s)
    {
        g_session_map[&over] = my_id;
    }
    SESSION() {
        std::cout << "ERROR" << std::endl;
    }
    ~SESSION() { closesocket(client_s); }

    void do_recv()
    {
        wsabuf[0].buf = buf;
        wsabuf[0].len = BUFSIZE;
        DWORD recv_flag = 0;
        ZeroMemory(&over, sizeof(over));
        int res = WSARecv(client_s, wsabuf, 1, nullptr, &recv_flag, &over, recv_callback);
    }

    void do_send(int recv_size)
    {
        wsabuf[0].len = recv_size;
        int res = WSASend(client_s, wsabuf, 1, nullptr, 0, &over, send_callback);
        if (0 != res) {
            int err_no = WSAGetLastError();
            if (WSA_IO_PENDING != err_no)
                print_error("WSASend", WSAGetLastError());
        }
    }

    void print_message(DWORD recv_size)
    {
        int my_id = g_session_map[&over];
        std::cout << "Client [" << my_id << "] Sent: ";
        for (unsigned int i = 0; i < recv_size; ++i)
            std::cout << buf[i];
        std::cout << std::endl;
    }
};


std::unordered_map<int, SESSION> g_players;

bool b_shutdown{ false };

void print_error(const char* msg, int err_no)
{
    WCHAR* msg_buf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        NULL, err_no,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPTSTR>(&msg_buf), 0, NULL);
    std::cout << msg;
    std::wcout << L" 에러 " << msg_buf << std::endl;
    while (true); // 디버깅 용
    LocalFree(msg_buf);
}

void CALLBACK send_callback(DWORD err, DWORD send_size, LPWSAOVERLAPPED pover, DWORD send_flag)
{
    if (0 != err) {
        print_error("WSASend", WSAGetLastError());
    }
    g_players[g_session_map[pover]].do_recv();
}

void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pover, DWORD recv_flag)
{
    if (0 != err) {
        print_error("WSARecv", WSAGetLastError());
    }
    int my_id = g_session_map[pover];
    if (0 == recv_size) {
        g_players.erase(my_id);
        return;
    }

    g_players[my_id].print_message(recv_size);
    g_players[my_id].do_send(recv_size);
}


int main()
{
    std::wcout.imbue(std::locale("korean"));

    WSADATA WSAData;
    WSAStartup(MAKEWORD(2, 0), &WSAData);

    SOCKET server_s = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, nullptr, 0, WSA_FLAG_OVERLAPPED);
    SOCKADDR_IN server_a;
    server_a.sin_family = AF_INET;
    server_a.sin_port = htons(SERVER_PORT);
    server_a.sin_addr.S_un.S_addr = htons(INADDR_ANY);
    bind(server_s, reinterpret_cast<sockaddr*>(&server_a), sizeof(server_a));
    listen(server_s, SOMAXCONN);
    int addr_size = sizeof(server_a);
    int id{ 0 };
    while (false == b_shutdown) {
        SOCKET client_s = WSAAccept(server_s, reinterpret_cast<sockaddr*>(&server_a), &addr_size, nullptr, 0);
        g_players.try_emplace(id, client_s, id);
        g_players[id++].do_recv();
    }
    g_players.clear();
    closesocket(server_s);
    WSACleanup();
}