#include "ChessGame.h"
#include "Chess.h"

ChessGame::ChessGame()
{
	// backgroud.Draw(mdc, 0, 0, 800, 800, 0, 0, 800, 800); // 기본 시작화면
	backgroud.Load(TEXT("IMG/Background.png"));

	w = backgroud.GetWidth();
	h = backgroud.GetHeight();
}

void ChessGame::Drow(HDC& mdc)
{
	backgroud.Draw(mdc, 0, 0, w, h, 0, 0, w, h);

	for (Chess* ch : chesses) {
		ch->Drow(mdc);
	}
}

void ChessGame::AddChess()
{
	const std::string str{ "IMG/Momonga" + std::to_string(chesses.size() + 1) + ".png"};
	RECT rt{ 0, 0, 100, 100 };
	Chess* ch = new Chess{ str, rt };

	chesses.push_back(ch);
}

void ChessGame::MinusChess(int my_id)
{
	// chesses[my_id] = nullptr;
}

void ChessGame::Move(int my_id, int add_x, int add_y)
{
	chesses[my_id]->Move(add_x, add_y);
}

void ChessGame::PressKey(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_RIGHT: {
		Move(0, 1, 0);
		break;
	}
	case VK_LEFT: {
		Move(0, -1, 0);
		break;
	}
	case VK_UP: {
		Move(0, 0, -1);
		break;
	}
	case VK_DOWN: {
		Move(0, 0, 1);
		break;
	}
	case VK_ESCAPE: {
		exit(829);
	}
	default: {
		break;
	}
	}
}
