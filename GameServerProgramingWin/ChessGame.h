#pragma once
#include "stdafx.h"
class Chess;

class ChessGame
{
	int w{}, h{};
	CImage backgroud{};
	std::vector<Chess*> chesses;

public: 
	ChessGame();
	void Drow(HDC&);
	void AddChess();
	void MinusChess(int my_id);
	void Move(int my_id, int add_x, int add_y);
	void PressKey(WPARAM wParam);
};

