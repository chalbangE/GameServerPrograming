#pragma once

#include <unordered_map> // 검색속도가 map보다 훨~씬 빠름 O(N) = 1임

#pragma comment (lib, "WS2_32.LIB")

#include "stdafx.h"
#include "PACKET.h"

void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pover, DWORD recv_flag);
void CALLBACK send_callback(DWORD err, DWORD send_size, LPWSAOVERLAPPED pover, DWORD send_flag);

class SESSION;
class EXP_OVER;

std::unordered_map<LPWSAOVERLAPPED, int> g_session_map;
std::unordered_map<int, SESSION> g_players;

KEY_PACKET key_p{ KEY_TYPE };
POS_PACKET pos_p{ POS_TYPE };

class EXP_OVER
{
public:
    WSAOVERLAPPED over;
    WSABUF wsabuf[1];
    char buf[BUFSIZE]{};
    int id;

    EXP_OVER(int s_id, char* mess, int m_size) : id{ s_id }
    {
        ZeroMemory(&over, sizeof(over));
        wsabuf[0].buf = buf;
        wsabuf[0].len = m_size;

        memcpy(buf, mess, m_size);
    }
};

class SESSION {
    char buf[BUFSIZE];
    SOCKET client_s;
    WSAOVERLAPPED over;

public:
    Chess hos;
    int id = -1;
    WSABUF send_wsabuf[1];
    WSABUF recv_wsabuf[1];
    char recv_buf[BUFSIZE];

    SESSION(SOCKET s, int my_id) : client_s(s), id(my_id)
    {
        recv_wsabuf[0].buf = reinterpret_cast<char*>(&key_p);
        recv_wsabuf[0].len = sizeof(KEY_PACKET);
        send_wsabuf[0].buf = reinterpret_cast<char*>(&pos_p);
        send_wsabuf[0].len = sizeof(POS_PACKET);

        g_session_map[&over] = my_id;
    }
    SESSION() 
    {
        std::cout << "ERROR" << std::endl;
    }
    ~SESSION() { closesocket(client_s); }

    void do_recv()
    {
        DWORD recv_flag = 0;
        ZeroMemory(&over, sizeof(over));
        int res = WSARecv(client_s, recv_wsabuf, 1, nullptr, &recv_flag, &over, recv_callback);
        if (0 != res) {
            int err_no = WSAGetLastError();
            if (WSA_IO_PENDING != err_no)
                print_error("do_recv - WSARecv", WSAGetLastError());
        }
    }

    void do_send(int s_id, char* mess, int recv_size)
    {
        auto b = new EXP_OVER(s_id, mess, recv_size);
        
        int res = WSASend(client_s, b->wsabuf, 1, nullptr, 0, &b->over, send_callback);
        if (0 != res) {
            int err_no = WSAGetLastError();
            if (WSA_IO_PENDING != err_no)
                print_error("do_send - WSASend", WSAGetLastError());
        }
    }

    void print_message(DWORD recv_size)
    {
        int my_id = g_session_map[&over];
        std::cout << "Client [" << my_id << "] Sent: " << key_p.key << std::endl;
    }

    void broadcast(int m_size)
    {
        for (auto& p : g_players) {
            p.second.do_send(g_session_map[&over], send_wsabuf[0].buf, m_size);
        }
    }
};

bool b_shutdown{ false };

void CALLBACK send_callback(DWORD err, DWORD send_size, LPWSAOVERLAPPED pover, DWORD send_flag)
{
    if (0 != err) {
        print_error("WSASend", WSAGetLastError());
    }

    // 할당한 메모리 지우기
    auto b = reinterpret_cast<EXP_OVER*>(pover);
    delete b;
}

void CALLBACK recv_callback(DWORD err, DWORD recv_size, LPWSAOVERLAPPED pover, DWORD recv_flag)
{
    if (0 != err) {
        print_error("WSARecv", WSAGetLastError());
    }
    int my_id = g_session_map[pover];
    key_p.id = my_id;

    switch (key_p.type) {
    case LOGIN_TYPE: {
        pos_p.id = 0;
        while (my_id >= pos_p.id) {
            pos_p.type = LOGIN_TYPE;
            if (g_players[pos_p.id].id != -1) {
                pos_p.x = g_players[pos_p.id].hos.x;
                pos_p.y = g_players[pos_p.id].hos.y;

                g_players[my_id].do_send(g_session_map[pover], g_players[my_id].send_wsabuf[0].buf, sizeof(POS_PACKET));
            }
            ++pos_p.id;

            std::cout << pos_p.id << std::endl;
        }

        pos_p.id = -1;
        g_players[my_id].do_send(g_session_map[pover], g_players[my_id].send_wsabuf[0].buf, sizeof(POS_PACKET));

        pos_p.id = my_id;
        pos_p.type = NEW_LOGIN_TYPE;
        g_players[my_id].broadcast(sizeof(POS_PACKET));
        g_players[my_id].do_recv();

        break;
    }
    case LOGOUT_TYPE: {

        break;
    }
    case KEY_TYPE: {
        MoveChess(g_players[key_p.id].hos, key_p.key);

        pos_p.type = POS_TYPE;
        pos_p.id = my_id;
        pos_p.x = g_players[key_p.id].hos.x;
        pos_p.y = g_players[key_p.id].hos.y;

        g_players[my_id].print_message(recv_size);
        g_players[my_id].broadcast(sizeof(POS_PACKET));
        g_players[my_id].do_recv();
        break;
    }
    default:
        break;
    }
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