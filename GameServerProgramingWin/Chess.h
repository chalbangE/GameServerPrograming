#pragma once
#include "stdafx.h"

class Chess
{
	RECT imgsize{};
	RECT drowimgsize{};
	int x{}, y{};
	CImage img{};

public:
	Chess();
	Chess(const std::string& str, RECT rt);
	void Drow(HDC& mdc);
	void Move(int add_x, int add_y);
};

