#pragma once
#include "stdafx.h"

class Chess;

class ChessGame
{
public: 
	int w{}, h{}, id{};
	CImage backgroud{};
	std::unordered_map<int, Chess*> chesses;
	SOCKET sev_s{};

	KEY_PACKET key_p{ 0 };
	POS_PACKET pos_p{ 3 };

	ChessGame();
	ChessGame(SOCKET& server);
	void init(SOCKET& server);
	void Drow(HDC&);
	void AddChess();
	void MinusChess();
	void Move();
	void PressKey(WPARAM wParam);
	void do_recv();
};

