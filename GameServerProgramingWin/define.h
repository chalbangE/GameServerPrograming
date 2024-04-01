#pragma once
#include "stdafx.h"
#include "ChessGame.h"

constexpr short SERVER_PORT{ 2551 };
constexpr int BUFSIZE{ 256 };
constexpr char SERVER_ADDR[] = "127.0.0.1";

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

static WSABUF recv_wsabuf[1]{}; // �Ź� ȣ���ϴ� �ͺ��� �������� �����ؼ� ���
static WSABUF send_wsabuf[1]{}; // �Ź� ȣ���ϴ� �ͺ��� �������� �����ؼ� ���
static char buf[BUFSIZE]{};
static SOCKET server_s{};

// read_n_send ���� ���������� ������� �Լ��� ����Ǹ� �ּҰ��� �����ذ��� �ǹ̰� ����
static WSAOVERLAPPED wsaover{};

static bool bshutdown{ false }; // ���� ���� ����