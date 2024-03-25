#define _CRT_NON_CONFORMINGS_SWPRINTFS
#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")
#pragma comment(lib,"winmm.lib")
#pragma comment(lib,"msimg32.lib")
#pragma comment (lib, "fmod_vc.lib")

#include <windows.h> //--- ������ ��� ����
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string.h>
#include <atlImage.h> // cimg ������ �ʿ��� ���
#include <vector>
#include <string>
#include <fstream>
#include "fmod.hpp"
#include "fmod_errors.h" 
#include <random>
#include <time.h>

#define g 10 // �߷°��ӵ�
#define t 0.19 // �ӵ� ���� ����
#define side 60 // �� �� ���� ����
#define rd 12.5 // �� ������

// �ִ� vx = 21
// �ִ� vy = 43
// ax = 7
// x�� �ִ� �̵��Ÿ� = 180
// x�� �ּ� �̵��Ÿ� = 90
// �¿������� vx = 60
// ��º� vy = -65
// ������ �� �� vy = 5 �� vy = 5.1
// ��þ����� vx = 50
// ���������� vy = 50

using namespace std;

HINSTANCE g_hInst;
LPCTSTR lpszClass = L"Window Class Name";
LPCTSTR lpszWindowName = L"Trip of a Ball";
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam);

enum State {
	Normal, CantEat, ShortTelpo, Dash, HighJmp
};
enum Direction {
	dirRight, dirLeft, dirDown, dirUp
};
enum BlockType {
	JumpBk, RStraightBk, LStraightBk,
	RectBHBk, CircleBHBk, RectWHBk, CircleWHBk,
	BreakBk, ClimbBK, MusicBk,
	MvBkStopBk, MoveBk,
	Star,
	// ������� Fuction�� �ȵ�
	BasicBk, LauncherBk, OnceMvBk, LightBk, Bullet, Item, SwitchBk, ElectricBk
};
enum Game {
	start, stage, survival, custom, play, stop, clear, death, customplay, customdeath, survivalready, survivalstop, survivaldeath
};
enum SoundCheck {
	X, ballcrach, telpo, eatstar, balldeath, click, gameclear, music
};

struct Ball {
	double x = 30, y = 10, vx, vy, ax;
	int item; // ������ ���� ����
	int state; // �� ���� �� �ִ� ����
};
struct Block {
	int x, y, type, subtype, ani = 0;
};
struct CrashedBk {
	int dir, i, j;
	double x, y;
	int quality;
};
struct doubleRECT {
	double left, top, right, bottom;
};

#define SVMAPCNT 24

Ball ball = { 30, 12.5, 0, 0, 0, Normal, Normal };
bool isLeftPressed, isRightPressed;
int GamePlay = start;
vector <Block> block[15], bullet, Readyblock[4];
vector <Block> animation;
vector <CrashedBk> crash;
RECT window; // ȭ�� ũ��
doubleRECT ballrc;
Block list[43];
int Map[15][25], SurvivalMap[SVMAPCNT][4][9], starcnt = 0;
bool isSwitchOff;
int Scheck = 0, score = 0, blockDown = 0, random, PrintLc = 3;
double remx, remy;

void CrashExamin();
int MyIntersectRect(const doubleRECT* ballrc, const doubleRECT* blockrc);
int isCrashed(const doubleRECT* ballrc, const doubleRECT* blockrc);
void Crash(int dir, int i, int y);
int BlockQuality(const Block* block);
Block* Search(const int type);
void CrashBasicRight(const Block* block);
void CrashBasicLeft(const Block* block);
void CrashBasicBottom(const Block* block);
void CrashBasicTop(const Block* block);
void MakeBullet(const Block* block, int BulletType);
void MoveBullet();
void CrashBullet();
bool OnceMvBkGo(const Block* b);
void MoveOnceMvBk(int y, int i);
void MoveBall();
bool MoveMoveBk(Block* b);
void TurnMoveBk(Block* b);
void UseItem();
void MakeBlockList();
void ClearVector();
void MakeReadyVector();
void ClearReadyVector();
void MakeVector();

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdParam, int nCmdShow)
{
	srand((unsigned int)time(NULL));
	HWND hwnd;
	MSG Message;
	WNDCLASSEX WndClass;
	g_hInst = hInstance;
	WndClass.cbSize = sizeof(WndClass);
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = (WNDPROC)WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = lpszClass;
	WndClass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	RegisterClassEx(&WndClass);
	hwnd = CreateWindow(lpszClass, lpszWindowName, WS_OVERLAPPEDWINDOW, 0, 0, 1516, 939, NULL, (HMENU)NULL, hInstance, NULL);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&Message, 0, 0, 0)) {
		TranslateMessage(&Message);
		DispatchMessage(&Message);
	}
	return Message.wParam;
}
LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc; HDC mdc; HBITMAP HBitmap, OldBitmap;
	HFONT hFont, OldFont;
	static CImage imgBall, imgBasicBlock, imgFuctionBlock, imgOnceMvBlock, imgBullet, imgLauncherBlock, imgLightBlock, imgItem, imgSwitchBk, imgElectricBk,
		imgStartScreen, imgStageScreen, imgStopScreen, imgClearScreen, imgPlayScreen, imgMaptoolScreen, imgSurvivalScreen, imgSurvivalReady, imgSurvivalStop,
		imgHomeButton, imgResetButton, imgLoadButton, imgSaveButton, imgEraseButton, imgPlayButton,
		imgBlockList, imgOutline,
		imgStarAni, imgDeadAni, imgBreakAni, imgElectricAni;
	static POINT BallStartLC, MouseLC;
	static OPENFILENAME OFN;
	static TCHAR filter[] = L"Every File(*.*)\0*.*\0Text File\0*.txt;*.doc\0";
	static TCHAR lpstrFile[100], str[20];
	static bool drag = false;
	static int selection = 0, electictimer = 0, bestscore = 0;
	int ani;
	SIZE size;
	const wchar_t* fontPath = L"��ɵ���ü+OTF.otf";
	AddFontResource(fontPath);

	static FMOD::System* ssystem;
	static FMOD::Sound* ballCrach_Sound, * Telpo_Sound, * EatStar_Sound, * ballDeath_Sound, * Click_Sound, * GameClear_Sound, * MusicBk_Sound;
	static FMOD::Channel* channel = 0;
	static FMOD_RESULT result;
	static void* extradriverdata = 0;


	switch (uMsg) {
	case WM_CREATE: {
		SetTimer(hwnd, 1, 10, NULL);
		GetClientRect(hwnd, &window);
		MakeBlockList();
		// �̹��� �ε�
		{
			imgBall.Load(TEXT("�ٿ�� PNG/��.png"));
			imgBasicBlock.Load(TEXT("�ٿ�� PNG/�⺻��.png"));
			imgFuctionBlock.Load(TEXT("�ٿ�� PNG/��ɺ�.png"));
			imgOnceMvBlock.Load(TEXT("�ٿ�� PNG/��ĭ�̵���.png"));
			imgBullet.Load(TEXT("�ٿ�� PNG/��ź.png"));
			imgLauncherBlock.Load(TEXT("�ٿ�� PNG/��ź�߻��.png"));
			imgLightBlock.Load(TEXT("�ٿ�� PNG/��ȣ���.png"));
			imgItem.Load(TEXT("�ٿ�� PNG/������.png"));
			imgSwitchBk.Load(TEXT("�ٿ�� PNG/���⽺��ġ��.png"));
			imgElectricBk.Load(TEXT("�ٿ�� PNG/�����.png"));

			imgStarAni.Load(TEXT("�ٿ�� PNG/�� ��������Ʈ.png"));
			imgDeadAni.Load(TEXT("�ٿ�� PNG/�� ��������Ʈ.png"));
			imgBreakAni.Load(TEXT("�ٿ�� PNG/�Ҹ�� ��������Ʈ.png"));
			imgElectricAni.Load(TEXT("�ٿ�� PNG/���� ��������Ʈ.png"));

			imgStartScreen.Load(TEXT("�ٿ�� PNG/����ȭ��.png"));
			imgStageScreen.Load(TEXT("�ٿ�� PNG/��������.png"));
			imgStopScreen.Load(TEXT("�ٿ�� PNG/�Ͻ�����.png"));
			imgClearScreen.Load(TEXT("�ٿ�� PNG/����Ŭ����.png"));
			imgPlayScreen.Load(TEXT("�ٿ�� PNG/�����÷��̹��.png"));
			imgMaptoolScreen.Load(TEXT("�ٿ�� PNG/����.png"));
			imgSurvivalScreen.Load(TEXT("�ٿ�� PNG/�����̹���� ���.png"));
			imgSurvivalReady.Load(TEXT("�ٿ�� PNG/�����̹���� �����غ�.png"));
			imgSurvivalStop.Load(TEXT("�ٿ�� PNG/�����̹���� �Ͻ�����.png"));

			imgHomeButton.Load(TEXT("�ٿ�� PNG/Ȩ��ư.png"));
			imgResetButton.Load(TEXT("�ٿ�� PNG/����_����.png"));
			imgLoadButton.Load(TEXT("�ٿ�� PNG/����_�ҷ�����.png"));
			imgSaveButton.Load(TEXT("�ٿ�� PNG/����_����.png"));
			imgEraseButton.Load(TEXT("�ٿ�� PNG/����_���찳.png"));
			imgPlayButton.Load(TEXT("�ٿ�� PNG/����_�÷���.png"));

			imgBlockList.Load(TEXT("�ٿ�� PNG/����_�� ���� ���.png"));
			imgOutline.Load(TEXT("�ٿ�� PNG/����_�� ���� �׵θ�.png"));
		}
		// ���� �ε�
		{
			result = FMOD::System_Create(&ssystem); //--- ���� �ý��� ����
			if (result != FMOD_OK)
				exit(0);
			ssystem->init(32, FMOD_INIT_NORMAL, extradriverdata); //--- ���� �ý��� �ʱ�ȭ
			ssystem->createSound("Sound/ball.ogg", FMOD_LOOP_OFF, 0, &ballCrach_Sound); //--- �� Ƣ��� �Ҹ�
			ssystem->createSound("Sound/telpo.mp3", FMOD_LOOP_OFF, 0, &Telpo_Sound); //--- �� �����ϴ� �Ҹ� (����, ��Ȧ)
			ssystem->createSound("Sound/eatStar.mp3", FMOD_LOOP_OFF, 0, &EatStar_Sound); //--- �� ������ ���� �Ҹ�
			ssystem->createSound("Sound/balldeath.wav", FMOD_LOOP_OFF, 0, &ballDeath_Sound); //--- �� ������ ���� �Ҹ�
			ssystem->createSound("Sound/Click.mp3", FMOD_LOOP_OFF, 0, &Click_Sound); //--- Ŭ��
			ssystem->createSound("Sound/GameClear.mp3", FMOD_LOOP_OFF, 0, &GameClear_Sound); //--- ���� Ŭ����
			ssystem->createSound("Sound/musicbk.mp3", FMOD_LOOP_OFF, 0, &MusicBk_Sound); //--- ���� Ŭ����
		}
		// �����̹���� �ε�
		{
			ifstream in{ "�ٿ�� ��/Survival_Map.txt" };

			int cnt = 0;
			while (cnt < SVMAPCNT) {
				for (int y = 0; y < 4; ++y) {
					for (int x = 0; x < 9; ++x) {
						in >> SurvivalMap[cnt][y][x];
					}
				}

				cnt++;
			}

			in.close();
		}

		MouseLC = { 0, 0 };
		break;
	}
	case WM_CHAR: {
		switch (wParam)
		{
		case 'a':
			Scheck = ballcrach;
			break;
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			break;
		case VK_SPACE: { // ������ ���
			if (ball.item) {
				UseItem();
				Scheck = telpo;
			}
			else if (GamePlay == survivalready) {
				GamePlay = survival;
				score = 0;
				random = 0;
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_KEYDOWN: {
		switch (wParam)
		{
		case VK_ESCAPE: {
			if (GamePlay == play)
				GamePlay = stop;
			else if (GamePlay == stop)
				GamePlay = play;
			else if (GamePlay == custom || GamePlay == stage)
				GamePlay = start;
			else if (GamePlay == customplay)
				GamePlay = custom;
			else if (GamePlay == clear)
				GamePlay = stage;
			else if (GamePlay == survival || GamePlay == survivalready)
				GamePlay = survivalstop;
			else if (GamePlay == survivalstop)
				GamePlay = survivalready;
			break;
		}
		case VK_RIGHT: {
			if (isRightPressed == false) {
				if (ball.vy == 5) {
					Scheck = telpo;
					ball.vx = -21;
					ball.vy = -40;
				}
				else if (ball.vy == 5.1) {
					Scheck = telpo;
					ball.vy = -43;
					ball.vx = 21;
				}
			}
			break;
		}
		case VK_LEFT: {
			if (isLeftPressed == false) {
				if (ball.vy == 5) {
					Scheck = telpo;
					Scheck = telpo;
					ball.vy = -43;
					ball.vx = -21;
				}
				else if (ball.vy == 5.1) {
					Scheck = telpo;
					Scheck = telpo;
					ball.vx = 21;
					ball.vy = -40;
				}
			}
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_KEYUP: {
		switch (wParam) {
		case VK_RIGHT: {
			if (ball.vy == 5 || ball.vy == 5.1)
				isRightPressed = false;
			break;
		}
		case VK_LEFT: {
			if (ball.vy == 5 || ball.vy == 5.1)
				isLeftPressed = false;
			break;
		}
		default:
			break;
		}
		break;
	}
	case WM_PAINT: {
		hdc = BeginPaint(hwnd, &ps);
		mdc = CreateCompatibleDC(hdc);
		HBitmap = CreateCompatibleBitmap(hdc, window.right, window.bottom);
		OldBitmap = (HBITMAP)SelectObject(mdc, (HBITMAP)HBitmap);
		FillRect(mdc, &window, WHITE_BRUSH);

		//���� �� ��ġ
		if (GamePlay == custom && drag == true && MouseLC.x >= 21 && MouseLC.x <= 21 + 1200 && MouseLC.y >= 21 && MouseLC.y <= 21 + 720) {
			if (selection > 0) {// ���� ���õǾ��� ���
				if ((MouseLC.x - 21) / 48 == BallStartLC.x && (MouseLC.y - 21) / 48 == BallStartLC.y) // ���� ���� ���
					BallStartLC = { -1, -1 };

				if (selection == 11 || selection == 13) {
					for (int y = 0; y < 15; ++y) {
						for (int x = 0; x < 25; ++x) {
							if (Map[y][x] == selection) {
								Map[y][x] = 0;
							}
						}
					}
				}
				Map[(MouseLC.y - 21) / 48][(MouseLC.x - 21) / 48] = selection;
				if (list[selection - 1].type == SwitchBk)
					isSwitchOff = list[selection - 1].subtype;
			}
			else if (selection == 0) { // ���� ���õǾ��� ���
				if (Map[(MouseLC.y - 21) / 48][(MouseLC.x - 21) / 48]) // ���� ���� ���
					Map[(MouseLC.y - 21) / 48][(MouseLC.x - 21) / 48] = 0;
				BallStartLC = { (MouseLC.x - 21) / 48, (MouseLC.y - 21) / 48 };
			}
			else {// ���찳�� ���õǾ��� ���
				if ((MouseLC.x - 21) / 48 == BallStartLC.x && (MouseLC.y - 21) / 48 == BallStartLC.y) // �� �����
					BallStartLC = { -1, -1 };
				else
					Map[(MouseLC.y - 21) / 48][(MouseLC.x - 21) / 48] = 0; // �� �����
			}
		}

		// ���� ���� ȭ��
		if (GamePlay == start) {
			if (MouseLC.x <= 430 && MouseLC.y >= 515 && MouseLC.y <= 615)
				imgStartScreen.Draw(mdc, 0, 0, window.right, window.bottom, 1500, 0, window.right, window.bottom); // �������� ��� �� Ŀ��
			else if (MouseLC.x <= 430 && MouseLC.y >= 640 && MouseLC.y <= 740)
				imgStartScreen.Draw(mdc, 0, 0, window.right, window.bottom, 3000, 0, window.right, window.bottom); // �����̹� ��� �� Ŀ��
			else if (MouseLC.x <= 430 && MouseLC.y >= 763 && MouseLC.y <= 863)
				imgStartScreen.Draw(mdc, 0, 0, window.right, window.bottom, 4500, 0, window.right, window.bottom); // Ŀ���͸���¡ �� Ŀ��
			else
				imgStartScreen.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom); // �⺻ ����ȭ��
		}
		else if (GamePlay == stage) {
			if (MouseLC.x >= 93 && MouseLC.x <= 442 && MouseLC.y >= 365 && MouseLC.y <= 715)
				imgStageScreen.Draw(mdc, 0, 0, window.right, window.bottom, 1500, 0, window.right, window.bottom); // �������� ��� �� Ŀ��
			else if (MouseLC.x >= 574 && MouseLC.x <= 923 && MouseLC.y >= 365 && MouseLC.y <= 715)
				imgStageScreen.Draw(mdc, 0, 0, window.right, window.bottom, 3000, 0, window.right, window.bottom); // �����̹� ��� �� Ŀ��
			else if (MouseLC.x >= 1060 && MouseLC.x <= 1408 && MouseLC.y >= 365 && MouseLC.y <= 715)
				imgStageScreen.Draw(mdc, 0, 0, window.right, window.bottom, 4500, 0, window.right, window.bottom); // Ŀ���͸���¡ �� Ŀ��
			else
				imgStageScreen.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom); // �⺻ ����ȭ��

			if (MouseLC.x >= 1368 && MouseLC.x <= 1448 && MouseLC.y >= 48 && MouseLC.y <= 128)
				imgHomeButton.Draw(mdc, 1368, 48, 80, 80, 80, 0, 80, 80); // Ȩ��ư �� Ŀ��
			else
				imgHomeButton.Draw(mdc, 1368, 48, 80, 80, 0, 0, 80, 80); // �⺻ Ȩ��ư
		}
		else if (GamePlay == custom) {
			imgMaptoolScreen.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom); // �⺻ ����ȭ��

			//��ư
			if (MouseLC.x >= 1239 && MouseLC.x <= 1239 + 164 && MouseLC.y >= 16 && MouseLC.y <= 16 + 78)
				imgPlayButton.Draw(mdc, 1239, 16, 164, 78, 164, 0, 164, 78); // �÷��̹�ư �� Ŀ��
			else
				imgPlayButton.Draw(mdc, 1239, 16, 164, 78, 0, 0, 164, 78); // �⺻ �÷��̹�ư
			if (MouseLC.x >= 1410 && MouseLC.x <= 1410 + 78 && MouseLC.y >= 16 && MouseLC.y <= 16 + 78)
				imgSaveButton.Draw(mdc, 1410, 16, 78, 78, 78, 0, 78, 78); // �����ư �� Ŀ��
			else
				imgSaveButton.Draw(mdc, 1410, 16, 78, 78, 0, 0, 78, 78); // �⺻ �����ư
			if ((MouseLC.x >= 1239 && MouseLC.x <= 1239 + 78 && MouseLC.y >= 105 && MouseLC.y <= 105 + 78))
				imgEraseButton.Draw(mdc, 1239, 105, 78, 78, 78, 0, 78, 78); // ���찳��ư �� Ŀ��
			else
				imgEraseButton.Draw(mdc, 1239, 105, 78, 78, 0, 0, 78, 78); // �⺻ ���찳��ư
			if (MouseLC.x >= 1325 && MouseLC.x <= 1325 + 78 && MouseLC.y >= 105 && MouseLC.y <= 105 + 78)
				imgResetButton.Draw(mdc, 1325, 105, 78, 78, 78, 0, 78, 78); // ���¹�ư �� Ŀ��
			else
				imgResetButton.Draw(mdc, 1325, 105, 78, 78, 0, 0, 78, 78); // �⺻ ���¹�ư
			if (MouseLC.x >= 1410 && MouseLC.x <= 1410 + 78 && MouseLC.y >= 105 && MouseLC.y <= 105 + 78)
				imgLoadButton.Draw(mdc, 1410, 105, 78, 78, 78, 0, 78, 78); // �ҷ������ư �� Ŀ��
			else
				imgLoadButton.Draw(mdc, 1410, 105, 78, 78, 0, 0, 78, 78); // �⺻ �ҷ������ư

			// �� ���
			for (int i = 0; i < 44; i++) {
				if (i == selection)
					imgOutline.Draw(mdc, 17 + 60 * (i % 22) + 7 * (i % 22), 756 + 68 * (i / 22), 60, 60, 60, 0, 60, 60); // �����׵θ�
				else
					imgOutline.Draw(mdc, 17 + 60 * (i % 22) + 7 * (i % 22), 756 + 68 * (i / 22), 60, 60, 0, 0, 60, 60); // �����׵θ�
			}
			imgBlockList.Draw(mdc, 20, 759, 1461, 122, 0, 0, 1461, 122); // ��

			//��
			for (int i = 0; i < 15; i++) {
				for (int j = 0; j < 25; j++) {
					if (Map[i][j]) {
						if (list[Map[i][j] - 1].type < BasicBk) { // ��ɺ� 
							imgFuctionBlock.Draw(mdc, 21 + j * 48, 21 + i * 48, 48, 48, list[Map[i][j] - 1].type * side, 0, side, side);
						}
						else {
							switch (list[Map[i][j] - 1].type) {
							case BasicBk: // �⺻��
								imgBasicBlock.Draw(mdc, 21 + j * 48, 21 + i * 48, 48, 48, list[Map[i][j] - 1].subtype * side, 0, side, side);
								break;
							case LauncherBk: // ��ź�߻��
								imgLauncherBlock.Draw(mdc, 21 + j * 48, 21 + i * 48, 48, 48, 8 * side, list[Map[i][j] - 1].subtype * side, side, side);
								break;
							case OnceMvBk: // ��ĭ�̵���
								imgOnceMvBlock.Draw(mdc, 21 + j * 48, 21 + i * 48, 48, 48, list[Map[i][j] - 1].subtype * side, 0, side, side);
								break;
							case LightBk: // ��ȣ���
								imgLightBlock.Draw(mdc, 21 + j * 48, 21 + i * 48, 48, 48, list[Map[i][j] - 1].subtype * side, 0, side, side);
								break;
							case Item: // ������
								imgItem.Draw(mdc, 21 + j * 48, 21 + i * 48, 48, 48, list[Map[i][j] - 1].subtype * side, 0, side, side);
								break;
							case SwitchBk:
								imgSwitchBk.Draw(mdc, 21 + j * 48, 21 + i * 48, 48, 48, isSwitchOff * side, 0, side, side);
								break;
							case ElectricBk:
								imgElectricBk.Draw(mdc, 21 + j * 48, 21 + i * 48, 48, 48, isSwitchOff * side, 0, side, side);
								break;
							}
						}
					}
				}
			}

			//��
			if (BallStartLC.x >= 0 && BallStartLC.y >= 0)
				imgBall.Draw(mdc, 21 + BallStartLC.x * 48 + 14, 21 + BallStartLC.y * 48 + 14, 20, 20, 0, 0, rd * 2, rd * 2);

			//���õ� ��
			if (selection > 0) { // ��
				if (list[selection - 1].type < BasicBk) { // ��ɺ� 
					imgFuctionBlock.Draw(mdc, 1315, 307, 80, 80, list[selection - 1].type * side, 0, side, side);
				}
				else {
					switch (list[selection - 1].type) {
					case BasicBk: // �⺻��
						imgBasicBlock.Draw(mdc, 1315, 307, 80, 80, list[selection - 1].subtype * side, 0, side, side);
						break;
					case LauncherBk: // ��ź�߻��
						imgLauncherBlock.Draw(mdc, 1315, 307, 80, 80, 8 * side, list[selection - 1].subtype * side, side, side);
						break;
					case OnceMvBk: // ��ĭ�̵���
						imgOnceMvBlock.Draw(mdc, 1315, 307, 80, 80, list[selection - 1].subtype * side, 0, side, side);
						break;
					case LightBk: // ��ȣ���
						imgLightBlock.Draw(mdc, 1315, 307, 80, 80, list[selection - 1].subtype * side, 0, side, side);
						break;
					case Item: // ������
						imgItem.Draw(mdc, 1315, 307, 80, 80, list[selection - 1].subtype * side, 0, side, side);
						break;
					case SwitchBk:
						imgSwitchBk.Draw(mdc, 1315, 307, 80, 80, isSwitchOff * side, 0, side, side);
						break;
					case ElectricBk:
						imgElectricBk.Draw(mdc, 1315, 307, 80, 80, isSwitchOff * side, 0, side, side);
						break;
					}
				}
			}
			else if (selection == 0) // ��
				imgBall.Draw(mdc, 1315 + 15, 307 + 15, 50, 50, 0, 0, rd * 2, rd * 2);
			else // ���찳
				imgEraseButton.Draw(mdc, 1315, 307, 78, 78, 0, 0, 78, 78);
		}

		// ���� �÷��� ȭ��
		else if (GamePlay == play || GamePlay == clear || GamePlay == stop || GamePlay == customplay || GamePlay == death || GamePlay == customdeath) { // �׾��� ��ƼŬ �ִϸ��̼� ���� �� �׸����� �߰���
			imgPlayScreen.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom);

			//��ź ���
			for (int i = 0; i < bullet.size(); i++) {
				imgBullet.Draw(mdc, bullet[i].x, bullet[i].y, 40, 40, bullet[i].subtype * 42, 0, 42, 42);
			}

			//�� ���
			for (int y = 0; y < 15; ++y) {
				for (int i = 0; i < block[y].size(); ++i) {
					if (block[y][i].type < BasicBk) { // ��ɺ� 
						if (block[y][i].type == MoveBk)
							imgFuctionBlock.Draw(mdc, block[y][i].x, block[y][i].y, side, side, block[y][i].type * side, 0, side, side); // �̵���
						else
							imgFuctionBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].type * side, 0, side, side);
					}
					else {
						switch (block[y][i].type) {
						case BasicBk: // �⺻��
							imgBasicBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].subtype * side, 0, side, side);
							break;
						case LauncherBk: // ��ź�߻��
							if (block[y][i].ani >= 63 && block[y][i].ani < 90) // ���
								imgLauncherBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, (block[y][i].ani - 63) / 3 * side, block[y][i].subtype * side, side, side);
							else if (block[y][i].ani >= 90) // �׶��̼�
								imgLauncherBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, 8 * side, block[y][i].subtype * side, side, side);
							else // ���
								imgLauncherBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, 0, block[y][i].subtype * side, side, side);
							break;
						case OnceMvBk: // ��ĭ�̵���
							imgOnceMvBlock.AlphaBlend(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].subtype * side, 0, side, side, block[y][i].ani, AC_SRC_OVER);
							break;
						case LightBk: // ��ȣ���
							imgLightBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].subtype * side, 0, side, side);
							break;
						case Item: // ������
							imgItem.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].subtype * side, 0, side, side);
							break;
						case SwitchBk:
							imgSwitchBk.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, isSwitchOff * side, 0, side, side);
							break;
						case ElectricBk:
							imgElectricBk.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, isSwitchOff * side, 0, side, side);
							break;
						}
					}
				}
			}

			//�� ���
			if (GamePlay != death && GamePlay != customdeath) { // ������ ��� ���ϰ�
				if (ball.state)
					imgBall.AlphaBlend(mdc, ball.x - rd, ball.y - rd, rd * 2, rd * 2, ball.item * (rd * 2), 0, rd * 2, rd * 2, 125, AC_SRC_OVER); // ��Ȱ��ȭ��
				else
					imgBall.AlphaBlend(mdc, ball.x - rd, ball.y - rd, rd * 2, rd * 2, ball.item * (rd * 2), 0, rd * 2, rd * 2, 255, AC_SRC_OVER); // Ȱ��ȭ��
			}

			// ��ƼŬ ���
			for (int i = 0; i < animation.size(); i++) {
				switch (animation[i].type) {
				case customdeath: // �̳� ���ļ� �� �̰ž�
					imgDeadAni.AlphaBlend(mdc, animation[i].x, animation[i].y, 180, 180, 180 * (animation[i].ani / 2), 180 * animation[i].subtype, 180, 180, 170, AC_SRC_OVER);
					break;
				case BreakBk:
					imgBreakAni.AlphaBlend(mdc, animation[i].x, animation[i].y, 180, 180, 180 * (animation[i].ani / 2), 180 * animation[i].subtype, 180, 180, 170, AC_SRC_OVER);
					break;
				case Star:
					imgStarAni.AlphaBlend(mdc, animation[i].x, animation[i].y, 180, 180, 180 * (animation[i].ani / 2), 180 * animation[i].subtype, 180, 180, 170, AC_SRC_OVER);
					break;
				}
				animation[i].ani++;
				if (animation[i].ani == 40) {
					if (animation[i].type == Star) { // �ִϸ��̼� ������ clear�� �ٲ�
						starcnt--;
						if (starcnt == 0) {
							if (GamePlay == play || GamePlay == death) {// �� �԰� �׾��� ���� Ŭ����ǰ�,,, ���ÿ� �Ͼ�� WM_TIMER������ ���ư��� �Ƹ� death�� ���� �ɰŶ� �������Ű����ѵ� ���׳��鹹,, �ƽ������
								Scheck = gameclear;
								GamePlay = clear;
							}
							else if (GamePlay == customplay || GamePlay == customdeath)
								GamePlay = custom;
						}
					}
					animation.erase(animation.begin() + i);
				}
			}

			// ȭ�� ���
			if (GamePlay == stop) {
				if (MouseLC.x >= 928 && MouseLC.x <= 1217 && MouseLC.y >= 284 && MouseLC.y <= 381)
					imgStopScreen.Draw(mdc, 0, 0, window.right, window.bottom, 1500, 0, window.right, window.bottom); // ����ȭ�� ��ư �� Ŀ��
				else if (MouseLC.x >= 928 && MouseLC.x <= 1217 && MouseLC.y >= 397 && MouseLC.y <= 494)
					imgStopScreen.Draw(mdc, 0, 0, window.right, window.bottom, 3000, 0, window.right, window.bottom); // �������� ��ư �� Ŀ��
				else if (MouseLC.x >= 928 && MouseLC.x <= 1217 && MouseLC.y >= 509 && MouseLC.y <= 606)
					imgStopScreen.Draw(mdc, 0, 0, window.right, window.bottom, 4500, 0, window.right, window.bottom); // ����� �� Ŀ��
				else
					imgStopScreen.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom); // �⺻ ����ȭ��
			}
			else if (GamePlay == clear) {
				if (MouseLC.x >= 587 && MouseLC.x <= 587 + 674 && MouseLC.y >= 530 && MouseLC.y <= 530 + 155)
					imgClearScreen.Draw(mdc, 0, 0, window.right, window.bottom, 1500, 0, window.right, window.bottom);
				else
					imgClearScreen.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom);
			}
		}
		else if (GamePlay == survival || GamePlay == survivalready || GamePlay == survivalstop || GamePlay == survivaldeath) {
			// ��� ���
			imgPlayScreen.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom);

			//��ź ���
			for (int i = 0; i < bullet.size(); i++) {
				imgBullet.Draw(mdc, bullet[i].x, bullet[i].y, 40, 40, bullet[i].subtype * 42, 0, 42, 42);
			}

			//�� ���
			for (int y = 0; y < 15; ++y) {
				for (int i = 0; i < block[y].size(); ++i) {
					if (block[y][i].type < BasicBk) { // ��ɺ� 
						if (block[y][i].type == MoveBk)
							imgFuctionBlock.Draw(mdc, block[y][i].x, block[y][i].y, side, side, block[y][i].type * side, 0, side, side); // �̵���
						else
							imgFuctionBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].type * side, 0, side, side);
					}
					else {
						switch (block[y][i].type) {
						case BasicBk: // �⺻��
							imgBasicBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].subtype * side, 0, side, side);
							break;
						case LauncherBk: // ��ź�߻��
							if (block[y][i].ani >= 63 && block[y][i].ani < 90) // ���
								imgLauncherBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, (block[y][i].ani - 63) / 3 * side, block[y][i].subtype * side, side, side);
							else if (block[y][i].ani >= 90) // �׶��̼�
								imgLauncherBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, 8 * side, block[y][i].subtype * side, side, side);
							else // ���
								imgLauncherBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, 0, block[y][i].subtype * side, side, side);
							break;
						case OnceMvBk: // ��ĭ�̵���
							imgOnceMvBlock.AlphaBlend(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].subtype * side, 0, side, side, block[y][i].ani, AC_SRC_OVER);
							break;
						case LightBk: // ��ȣ���
							imgLightBlock.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].subtype * side, 0, side, side);
							break;
						case Item: // ������
							imgItem.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, block[y][i].subtype * side, 0, side, side);
							break;
						case SwitchBk:
							imgSwitchBk.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, isSwitchOff * side, 0, side, side);
							break;
						case ElectricBk:
							imgElectricBk.Draw(mdc, block[y][i].x * side, block[y][i].y * side, side, side, isSwitchOff * side, 0, side, side);
							break;
						}
					}
				}
			}

			//�� ���
			if (GamePlay != survivaldeath) { // ������ ��� ���ϰ�
				if (ball.state)
					imgBall.AlphaBlend(mdc, ball.x - rd, ball.y - rd, rd * 2, rd * 2, ball.item * (rd * 2), 0, rd * 2, rd * 2, 125, AC_SRC_OVER); // ��Ȱ��ȭ��
				else
					imgBall.AlphaBlend(mdc, ball.x - rd, ball.y - rd, rd * 2, rd * 2, ball.item * (rd * 2), 0, rd * 2, rd * 2, 255, AC_SRC_OVER); // Ȱ��ȭ��
			}

			// ��ƼŬ ���
			for (int i = 0; i < animation.size(); i++) {
				switch (animation[i].type) {
				case customdeath: // �̳� ���ļ� �� �̰ž�
					imgDeadAni.AlphaBlend(mdc, animation[i].x, animation[i].y, 180, 180, 180 * (animation[i].ani / 2), 180 * animation[i].subtype, 180, 180, 170, AC_SRC_OVER);
					break;
				case BreakBk:
					imgBreakAni.AlphaBlend(mdc, animation[i].x, animation[i].y, 180, 180, 180 * (animation[i].ani / 2), 180 * animation[i].subtype, 180, 180, 170, AC_SRC_OVER);
					break;
				case Star:
					imgStarAni.AlphaBlend(mdc, animation[i].x, animation[i].y, 180, 180, 180 * (animation[i].ani / 2), 180 * animation[i].subtype, 180, 180, 170, AC_SRC_OVER);
					break;
				}
				animation[i].ani++;
				if (animation[i].ani == 40) {
					animation.erase(animation.begin() + i);
				}
			}

			// �ִϸ��̼� ���
			{
				ani = electictimer >= 122 ? 0 : electictimer;
				imgSurvivalScreen.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom);
				imgElectricAni.Draw(mdc, 462, 801, 576, 18, 0, (ani / 2) * 18, 576, 18);
				electictimer++;
				if (electictimer == 180)
					electictimer = 0;
			}

			//���� ���
			{
				hFont = CreateFont(-40, 0, 0, 0, 400, NULL, NULL, NULL, NULL, 10, 2, 1, 50, L"��ɵ���ü");
				OldFont = (HFONT)SelectObject(mdc, hFont);
				SetTextColor(mdc, RGB(255, 255, 255));
				SetBkMode(mdc, TRANSPARENT);
				wsprintf(str, L"%d", score);
				GetTextExtentPoint32(mdc, str, lstrlen(str), &size);
				TextOut(mdc, 750 - size.cx / 2, 7, str, lstrlen(str));
				SelectObject(mdc, OldFont);
				DeleteObject(hFont);
			}

			// ȭ�� ���
			if (GamePlay == survivalready && animation.size() == 0) {
				imgSurvivalReady.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom);

				hFont = CreateFont(-60, 0, 0, 0, 400, NULL, NULL, NULL, NULL, 10, 2, 1, 50, L"��ɵ���ü");
				OldFont = (HFONT)SelectObject(mdc, hFont);
				SetTextColor(mdc, RGB(255, 255, 255));
				SetBkMode(mdc, TRANSPARENT);
				wsprintf(str, L"Score  %d          Best  %d", score, bestscore);
				GetTextExtentPoint32(mdc, str, lstrlen(str), &size);
				TextOut(mdc, 750 - size.cx / 2, 385, str, lstrlen(str));
				SelectObject(mdc, OldFont);
				DeleteObject(hFont);
			}
			else if (GamePlay == survivalstop) {
				if (MouseLC.x >= 927 && MouseLC.x <= 1217 && MouseLC.y >= 337 && MouseLC.y <= 434) {
					imgSurvivalStop.Draw(mdc, 0, 0, window.right, window.bottom, 1500, 0, window.right, window.bottom);
				}
				else if (MouseLC.x >= 927 && MouseLC.x <= 1217 && MouseLC.y >= 451 && MouseLC.y <= 550) {
					imgSurvivalStop.Draw(mdc, 0, 0, window.right, window.bottom, 3000, 0, window.right, window.bottom);
				}
				else
					imgSurvivalStop.Draw(mdc, 0, 0, window.right, window.bottom, 0, 0, window.right, window.bottom);
			}
		}

		BitBlt(hdc, 0, 0, window.right, window.bottom, mdc, 0, 0, SRCCOPY);

		SelectObject(mdc, OldBitmap);
		DeleteObject(HBitmap);
		DeleteDC(mdc);
		EndPaint(hwnd, &ps);
		break;
	}
	case WM_TIMER: {
		// ������
		if (GamePlay == death && animation.size() == 0) { // ������ �ִϸ��̼� ������ ��������
			MakeVector();
			ball = { (double)BallStartLC.x, (double)BallStartLC.y, 0, 0, 0, Normal, Normal };
			GamePlay = play;
		}
		else if (GamePlay == customdeath && animation.size() == 0) {
			MakeVector();
			ball = { (double)BallStartLC.x * side + 30, (double)BallStartLC.y * side + 30, 0, 0, 0, Normal, Normal };
			GamePlay = customplay;
		}
		else if (GamePlay == survivaldeath && animation.size() == 0) {
			GamePlay = survivalready;
			MakeVector();
			blockDown = 0;
			PrintLc = 3;
			ball.x = window.right / 2;
			ball.y = 580;

			if (bestscore < score) {
				bestscore = score;

				ofstream out{ "BestScore.txt" };
				out << bestscore;
				out.close();
			}
		}

		// �� ���� ȿ���� ���
		switch (Scheck)
		{
		case ballcrach: {
			ssystem->playSound(ballCrach_Sound, 0, false, &channel);
			channel->setVolume(0.35);
			Scheck = X;
			break;
		}
		case telpo: {
			ssystem->playSound(Telpo_Sound, 0, false, &channel);
			channel->setVolume(0.5);
			Scheck = X;
			break;
		}
		case eatstar: {
			ssystem->playSound(EatStar_Sound, 0, false, &channel);
			channel->setVolume(1);
			Scheck = X;
			break;
		}
		case balldeath: {
			ssystem->playSound(ballDeath_Sound, 0, false, &channel);
			channel->setVolume(0.5);
			Scheck = X;
			break;
		}
		case click: {
			ssystem->playSound(Click_Sound, 0, false, &channel);
			channel->setVolume(1);
			Scheck = X;
			break;
		}
		case gameclear: {
			ssystem->playSound(GameClear_Sound, 0, false, &channel);
			channel->setVolume(1);
			Scheck = X;
			break;
		}
		case music: {
			ssystem->playSound(MusicBk_Sound, 0, false, &channel);
			channel->setVolume(1);
			Scheck = X;
			break;
		}
		}

		//��/�� �̵�, �浹üũ
		if (GamePlay == play || GamePlay == customplay || GamePlay == death || GamePlay == customdeath || GamePlay == survival || GamePlay == survivaldeath) { // �׾ �ִϸ��̼� �ϰ� ���� �� �� �������ߵż� ��

			// �� �ִϸ��̼� & �����̱�
			for (int y = 0; y < 15; y++) {
				for (int i = 0; i < block[y].size(); i++) {
					switch (block[y][i].type) {
					case MoveBk: {
						block[y][i].x += block[y][i].subtype;
						MoveMoveBk(&block[y][i]);
						break;
					}
					case LauncherBk: {
						block[y][i].ani++;
						if (block[y][i].ani == 100) { // 1�� �ֱ�� �߻�
							block[y][i].ani = 0;
							MakeBullet(&block[y][i], 0);
						}
						break;
					}
					case OnceMvBk: {
						if (block[y][i].ani != 255) {
							block[y][i].ani -= 15;
							if (block[y][i].ani <= 0) {
								block[y][i].ani = 255;
								MoveOnceMvBk(y, i);
							}
						}
						break;
					}
					case LightBk: {
						block[y][i].ani++;
						if (block[y][i].ani == 50) {
							block[y][i].ani = 0;
							block[y][i].subtype = block[y][i].subtype == 5 ? 0 : block[y][i].subtype + 1;
						}
						break;
					}
					}
				}
			}

			//��ź �̵�
			MoveBullet();
			//�� �̵�
			MoveBall();

			ballrc = { (double)ball.x - rd, (double)ball.y - rd, (double)ball.x + rd, (double)ball.y + rd };

			//��ź �浹üũ
			CrashBullet(); // �׾ �ִϸ��̼� �ϰ� ���� ���� �۵��ϰ��ϰ�; �Լ��� ��

			// �� �浹üũ
			if (GamePlay == play || GamePlay == customplay || GamePlay == survival) { // �̹� �������� �ȵ��ư��� �Ϸ���. ��� ���ư��� �ִϸ��̼� ���Ϳ� �ڲٵ�
				// �ٴڰ� �浹
				if (GamePlay == survival && ball.y + rd >= 804) {
					animation.emplace_back(Block{ (int)ball.x - 90, (int)ball.y - 90, customdeath, rand() % 4, 0 });
					Scheck = balldeath;
					GamePlay = survivaldeath;
				}
				else if (ball.y + rd >= window.bottom) {
					animation.emplace_back(Block{ (int)ball.x - 90, (int)ball.y - 90, customdeath, rand() % 4, 0 });
					Scheck = balldeath;
					if (GamePlay == play)
						GamePlay = death;
					else if (GamePlay == customplay)
						GamePlay = customdeath;
					else if (GamePlay == survival)
						GamePlay = survivaldeath;
				}

				// ���� �浹
				else CrashExamin();
			}
		}

		// �����̹� ��� �� �̵�
		if (GamePlay == survival) {
			if (blockDown == 200) {
				blockDown = 0;
				ball.y += side;
				score += 2;

				if (PrintLc < 0) {
					PrintLc = 3;
					MakeReadyVector();
				}

				// �� ĭ�� ������
				for (int i = 0; i < bullet.size(); i++) {
					bullet[i].y += side;
				}
				for (int i = 0; i < animation.size(); i++) {
					animation[i].y += side;
				}
				for (int y = 13; y >= 0; --y) {
					block[y + 1].clear();
					for (int x = 0; x < block[y].size(); ++x) {
						if (block[y][x].type == MoveBk)
							block[y][x].y += side;
						else
							block[y][x].y += 1;

						block[y + 1].push_back(block[y][x]);
					}
				}

				block[0].clear();
				for (int x = 0; x < Readyblock[PrintLc].size(); ++x) {
					block[0].push_back(Readyblock[PrintLc][x]);
				}
				PrintLc--;
			}
			blockDown++;
		}

		InvalidateRect(hwnd, NULL, FALSE);
		break;
	}
	case WM_LBUTTONDOWN: {
		if (GamePlay == start) {
			if (MouseLC.x <= 430 && MouseLC.y >= 515 && MouseLC.y <= 615) { // �������� ��ư
				Scheck = click;
				GamePlay = stage;
			}
			else if (MouseLC.x <= 430 && MouseLC.y >= 640 && MouseLC.y <= 740) { // �����̹� ��ư
				Scheck = click;
				GamePlay = survivalready;
				MakeVector();
				blockDown = 0;
				PrintLc = -1;
				ball.x = window.right / 2;
				ball.y = 580;

				ifstream in{ "BestScore.txt" };
				in >> bestscore;
				in.close();

				if (bestscore < score) {
					bestscore = score;

					ofstream out{ "BestScore.txt" };
					out << bestscore;
					out.close();
				}
			}
			else if (MouseLC.x <= 430 && MouseLC.y >= 763 && MouseLC.y <= 863) { // ���� ��ư
				Scheck = click;
				GamePlay = custom;
				BallStartLC = { -1, -1 };
				isSwitchOff = 0;
				memset(Map, 0, sizeof(Map));
				selection = 0;
			}
		}
		else if (GamePlay == stage) {
			if (MouseLC.x >= 93 && MouseLC.x <= 442 && MouseLC.y >= 365 && MouseLC.y <= 715) {
				Scheck = click; // ���� ���⿡ Ŭ���ϸ� 1 2 3���� �ذ����� �������� ���� ���� �ҷ��ͼ� ���͹迭�� �־��ִ� �Լ� ¥�� ������ �� ��
				ifstream in{ "�ٿ�� ��/Stage1.txt" };

				for (int y = 0; y < 15; ++y) {
					for (int x = 0; x < 25; ++x) {
						in >> Map[y][x];
					}
				}

				in >> BallStartLC.x;
				in >> BallStartLC.y;
				in >> isSwitchOff;
				BallStartLC.x = BallStartLC.x * side + 30;
				BallStartLC.y = BallStartLC.y * side + 30;

				GamePlay = death;
				in.close();
			}
			else if (MouseLC.x >= 574 && MouseLC.x <= 923 && MouseLC.y >= 365 && MouseLC.y <= 715) {
				Scheck = click; // ���� ���⿡ Ŭ���ϸ� 1 2 3���� �ذ����� �������� ���� ���� �ҷ��ͼ� ���͹迭�� �־��ִ� �Լ� ¥�� ������ �� ��
				ifstream in{ "�ٿ�� ��/Stage2.txt" };

				for (int y = 0; y < 15; ++y) {
					for (int x = 0; x < 25; ++x) {
						in >> Map[y][x];
					}
				}

				in >> BallStartLC.x;
				in >> BallStartLC.y;
				in >> isSwitchOff;
				BallStartLC.x = BallStartLC.x * side + 30;
				BallStartLC.y = BallStartLC.y * side + 30;

				GamePlay = death;
				in.close();
			}
			else if (MouseLC.x >= 1060 && MouseLC.x <= 1408 && MouseLC.y >= 365 && MouseLC.y <= 715) {
				Scheck = click; // ���� ���⿡ Ŭ���ϸ� 1 2 3���� �ذ����� �������� ���� ���� �ҷ��ͼ� ���͹迭�� �־��ִ� �Լ� ¥�� ������ �� ��
				ifstream in{ "�ٿ�� ��/Stage3.txt" };

				for (int y = 0; y < 15; ++y) {
					for (int x = 0; x < 25; ++x) {
						in >> Map[y][x];
					}
				}

				in >> BallStartLC.x;
				in >> BallStartLC.y;
				in >> isSwitchOff;
				BallStartLC.x = BallStartLC.x * side + 30;
				BallStartLC.y = BallStartLC.y * side + 30;

				GamePlay = death;
				in.close();
			}
			else if (MouseLC.x >= 1368 && MouseLC.x <= 1448 && MouseLC.y >= 48 && MouseLC.y <= 128) {
				Scheck = click;
				GamePlay = start;
			}
			else if (MouseLC.x >= 1490 && MouseLC.x <= 1500 && MouseLC.y >= 850 && MouseLC.y <= 900) {
				Scheck = click; // ���� ���⿡ Ŭ���ϸ� 1 2 3���� �ذ����� �������� ���� ���� �ҷ��ͼ� ���͹迭�� �־��ִ� �Լ� ¥�� ������ �� ��
				ifstream in{ "�ٿ�� ��/Stage4.txt" };

				for (int y = 0; y < 15; ++y) {
					for (int x = 0; x < 25; ++x) {
						in >> Map[y][x];
					}
				}

				in >> BallStartLC.x;
				in >> BallStartLC.y;
				in >> isSwitchOff;
				BallStartLC.x = BallStartLC.x * side + 30;
				BallStartLC.y = BallStartLC.y * side + 30;

				GamePlay = death;
				in.close();
			}
		}
		else if (GamePlay == stop) {
			if (MouseLC.x >= 928 && MouseLC.x <= 1217 && MouseLC.y >= 284 && MouseLC.y <= 381) { // ����ȭ�� ��ư �� Ŀ�� 
				Scheck = click;
				GamePlay = start;
			}
			else if (MouseLC.x >= 928 && MouseLC.x <= 1217 && MouseLC.y >= 397 && MouseLC.y <= 494) { // �������� ��ư �� Ŀ�� 
				Scheck = click;
				GamePlay = stage;
			}
			else if (MouseLC.x >= 928 && MouseLC.x <= 1217 && MouseLC.y >= 509 && MouseLC.y <= 606) { // ����� ��ư �� Ŀ��
				Scheck = click;
				GamePlay = death;
				MakeVector();
				ball = { (double)BallStartLC.x, (double)BallStartLC.y, 0, 0, 0, Normal, Normal }; // ����� �����ɷ� �ϸ� death�� �ٲ�� �ִϸ��̼� ������ �Ѿ�ߵż� �� �ٷ� ��������Ŵ
			}
		}
		else if (GamePlay == custom) {
			drag = true;
			//�� ����
			if (MouseLC.y >= 756 && MouseLC.y <= 756 + 60) {
				Scheck = click;
				for (int i = 0; i < 22; i++) {
					if (MouseLC.x >= 17 + 60 * i + 7 * i && MouseLC.x <= 17 + 60 * i + 7 * i + 60)
						selection = i;
				}
			}
			else if (MouseLC.y >= 756 + 60 + 7 && MouseLC.y <= 756 + 60 + 7 + 60) {
				Scheck = click;
				for (int i = 0; i < 22; i++) {
					if (MouseLC.x >= 17 + 60 * i + 7 * i && MouseLC.x <= 17 + 60 * i + 7 * i + 60)
						selection = i + 22;
				}
			}
			// �÷��� ��ư
			else if (MouseLC.x >= 1239 && MouseLC.x <= 1239 + 164 && MouseLC.y >= 16 && MouseLC.y <= 16 + 78) {
				Scheck = click;
				if (BallStartLC.x == -1 || BallStartLC.y == -1) {
					TCHAR a[100];
					wsprintf(a, L"�� ��ġ�� �������ּ���.");
					MessageBox(hwnd, a, L"�˸�", MB_OK);
					drag = false;
					break;
				}
				ball = { (double)BallStartLC.x * side + 30, (double)BallStartLC.y * side + 30, 0, 0, 0, Normal, Normal };
				GamePlay = customplay;
				MakeVector();
			}
			// ���찳 ��ư
			else if (MouseLC.x >= 1239 && MouseLC.x <= 1239 + 78 && MouseLC.y >= 105 && MouseLC.y <= 105 + 78) {
				Scheck = click;
				selection = -1;
			}
			// ���� ��ư
			else if (MouseLC.x >= 1325 && MouseLC.x <= 1325 + 78 && MouseLC.y >= 105 && MouseLC.y <= 105 + 78) {
				Scheck = click;
				memset(Map, 0, sizeof(Map));
				BallStartLC = { -1, -1 };
			}
			// �ҷ����� ��ư
			else if (MouseLC.x >= 1410 && MouseLC.x <= 1410 + 78 && MouseLC.y >= 105 && MouseLC.y <= 105 + 78) {
				Scheck = click;
				memset(&OFN, 0, sizeof(OPENFILENAME)); //--- ����ü �ʱ�ȭ
				OFN.lStructSize = sizeof(OPENFILENAME);
				OFN.hwndOwner = hwnd;
				OFN.lpstrFilter = filter;
				OFN.lpstrFile = lpstrFile;
				OFN.nMaxFile = 256;
				OFN.lpstrInitialDir = L".";

				if (GetOpenFileNameW(&OFN) != 0) { //--- ���� �Լ� ȣ��
					TCHAR a[100];
					wsprintf(a, L"%s ������ ���ðڽ��ϱ� ?", OFN.lpstrFile);
					MessageBox(hwnd, a, L"���� ����", MB_OK);

					ifstream in{ OFN.lpstrFile };

					for (int y = 0; y < 15; ++y) {
						for (int x = 0; x < 25; ++x) {
							in >> Map[y][x];
						}
					}

					in >> BallStartLC.x;
					in >> BallStartLC.y;
					in >> isSwitchOff;

					in.close();
				}
				drag = false;
			}
			// ���� ��ư
			else if (MouseLC.x >= 1410 && MouseLC.x <= 1410 + 78 && MouseLC.y >= 16 && MouseLC.y <= 16 + 78) {
				Scheck = click;
				memset(&OFN, 0, sizeof(OPENFILENAME)); //--- ����ü �ʱ�ȭ
				OFN.lStructSize = sizeof(OPENFILENAME);
				OFN.hwndOwner = hwnd;
				OFN.lpstrFilter = filter;
				OFN.lpstrFile = lpstrFile;
				OFN.nMaxFile = 256;
				OFN.lpstrInitialDir = L".";

				if (GetSaveFileNameW(&OFN) != 0) { //--- ���� �Լ� ȣ��
					TCHAR a[100];
					wsprintf(a, L"%s ��ġ�� ������ �����Ͻðڽ��ϱ� ?", OFN.lpstrFile);
					MessageBox(hwnd, a, L"�����ϱ� ����", MB_OK);
					TCHAR b[100];
					wsprintf(b, L"%s.txt", OFN.lpstrFile);

					ofstream out{ b };


					// �����迭 ����
					for (int y = 0; y < 15; ++y) {
						for (int x = 0; x < 25; ++x) {
							out << Map[y][x] << " ";
						}
						out << endl;
					}
					// �� ������ġ, ���� ���� ����
					out << BallStartLC.x << " " << BallStartLC.y << " " << isSwitchOff << endl;

					out.close();
				}
				drag = false;
			}
		}
		else if (GamePlay == clear) {
			if (MouseLC.x >= 587 && MouseLC.x <= 587 + 674 && MouseLC.y >= 530 && MouseLC.y <= 530 + 155) {
				Scheck = click;
				GamePlay = stage;
			}
		}
		else if (GamePlay == survivalstop) {
			if (MouseLC.x >= 927 && MouseLC.x <= 1217 && MouseLC.y >= 337 && MouseLC.y <= 434) {
				Scheck = click;
				GamePlay = start;
			}
			else if (MouseLC.x >= 927 && MouseLC.x <= 1217 && MouseLC.y >= 451 && MouseLC.y <= 550) {
				Scheck = click;
				GamePlay = survivalready;
				MakeVector();
				blockDown = 0, PrintLc = 3;
				ball.x = window.right / 2;
				ball.y = 580;

				if (bestscore < score) {
					bestscore = score;
					ofstream out{ "BestScore.txt" };
					out << bestscore;
					out.close();
				}
			}
		}
		break;
	}
	case WM_MOUSEMOVE: {
		if (GamePlay == start || GamePlay == stage || GamePlay == stop || GamePlay == clear || GamePlay == custom || GamePlay == survivalstop) {
			MouseLC.x = LOWORD(lParam);
			MouseLC.y = HIWORD(lParam);
		}
		break;
	}
	case WM_LBUTTONUP: {
		if (drag == true)
			drag = false;
		break;
	}
	case WM_DESTROY: {
		PostQuitMessage(0);
		break;
	}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

//�浹üũ
void CrashExamin() {
	doubleRECT blockrc;
	int crashStart, crashEnd, crashDir;
	bool doQ2block = true;

	if (GamePlay == survival) {
		// ������ ��
		if (ball.x + rd >= side * 17) {
			ball.x = side * 17 - rd;
			ball.vx = -ball.vx / 2;
			ball.ax = -7;
			return;
		}
		// ���ʺ�
		else if (ball.x - rd <= side * 8) {
			ball.x = side * 8 + rd;
			ball.vx = -ball.vx / 2;
			ball.ax = 7;
			return;
		}
	}
	else {
		// ������ ��
		if (ball.x + rd >= window.right) {
			ball.x = window.right - rd;
			ball.vx = -ball.vx / 2;
			ball.ax = -7;
			return;
		}
		// ���ʺ�
		if (ball.x - rd <= window.left) {
			ball.x = rd;
			ball.vx = -ball.vx / 2;
			ball.ax = 7;
			return;
		}
	}

	crash.clear();
	crashStart = ballrc.top >= 0 ? (int)(ballrc.top / 60) : 0;
	crashEnd = (int)(ballrc.bottom / 60);
	for (int y = crashStart; y <= crashEnd; y++) {
		for (int i = 0; i < block[y].size(); i++) {
			//�浹üũ ���ص� �Ǵ� ���� ��� �Ѿ
			if (block[y][i].type == MvBkStopBk || (block[y][i].type == ElectricBk && isSwitchOff) || block[y][i].type == RectWHBk || block[y][i].type == CircleWHBk)
				continue;
			else if (block[y][i].type == OnceMvBk && block[y][i].ani != 255)
				continue;

			//�浹üũ
			if (block[y][i].type == MoveBk)
				blockrc = { (double)block[y][i].x, (double)block[y][i].y, (double)(block[y][i].x + side), (double)(block[y][i].y + side) };
			else
				blockrc = { (double)block[y][i].x * side, (double)block[y][i].y * side, (double)((block[y][i].x + 1) * side), (double)((block[y][i].y + 1) * side) };
			if (isCrashed(&ballrc, &blockrc) != 4) {
				crashDir = MyIntersectRect(&ballrc, &blockrc);
				crash.emplace_back(CrashedBk{ crashDir, i, y, blockrc.left, blockrc.top, BlockQuality(&block[y][i]) });
			}
		}
	}

	if (crash.size()) {
		for (int i = 0; i < crash.size() - 1; i++) {
			for (int j = 0; j < crash.size() - i - 1; j++) {
				if (crash[j].quality > crash[j + 1].quality)
					swap(crash[j], crash[j + 1]);
			}
		}
		if (crash[0].quality != 1)
			doQ2block = false;

		for (int i = 0; i < crash.size(); i++) {
			if (crash[i].quality == 1 && doQ2block == false);
			else {
				Crash(crash[i].dir, crash[i].i, crash[i].j);
			}
		}
	}
}
void Crash(int dir, int i, int y) {
	Block* temp;
	doubleRECT blockrc;

	// ��� ��� ��� ���� ��
	switch (block[y][i].type) {
	case CircleBHBk:
	case RectBHBk: {
		temp = Search(block[y][i].type + 2); // ���� RectWHBk, CircleWHBk �˻�
		if (temp) {
			Scheck = telpo;
			ball.x = temp->x * side + side / 2;
			ball.y = temp->y * side + side / 2;
			ball.vx = 0;
			ball.vy = 43;
		}
		return;
	}
	case Item: {
		if (block[y][i].subtype < ShortTelpo) // �� ���¸� �ٲٴ� �������� ���
			ball.state = block[y][i].subtype;
		else
			ball.item = block[y][i].subtype;
		block[y].erase(block[y].begin() + i);
		for (int j = 0; j < crash.size(); j++) {
			if (y == crash[j].j && i < crash[j].i)
				crash[j].i -= 1;
		}
		return;
	}
	case LightBk: {
		if (block[y][i].subtype < 2) {
			animation.emplace_back(Block{ (int)ball.x - 90, (int)ball.y - 90, customdeath, rand() % 4, 0 });
			Scheck = balldeath;
			if (GamePlay == play)
				GamePlay = death;
			else if (GamePlay == customplay)
				GamePlay = customdeath;
			else if (GamePlay == survival)
				GamePlay = survivaldeath;
			return;
		}
		else break;
	}
	case Star: {
		if (ball.state == Normal) {
			Scheck = eatstar;
			animation.emplace_back(Block{ (block[y][i].x - 1) * side, (block[y][i].y - 1) * side, Star, rand() % 4, 0 });
			block[y].erase(block[y].begin() + i);
			for (int j = 0; j < crash.size(); j++) {
				if (y == crash[j].j && i < crash[j].i)
					crash[j].i -= 1;
			}

			if (GamePlay == survival)
				score += 10;
		}
		return;
	}
	case ElectricBk: {
		blockrc = { (double)block[y][i].x * side + 20, (double)block[y][i].y * side + 5, (double)(block[y][i].x + 1) * side - 20, (double)(block[y][i].y + 1) * side - 5 };
		if (isCrashed(&ballrc, &blockrc) != 4) {
			animation.emplace_back(Block{ (int)ball.x - 90, (int)ball.y - 90, customdeath, rand() % 4, 0 });
			Scheck = balldeath;
			if (GamePlay == play)
				GamePlay = death;
			else if (GamePlay == customplay)
				GamePlay = customdeath;
			else if (GamePlay == survival)
				GamePlay = survivaldeath;
		}
		return;
	}
	}

	// ���⿡ ���� �޶����� ���� ���
	switch (dir) {
	case dirUp: { // �� ���ʿ� �浹���� ���
		switch (block[y][i].type) {
		case BreakBk: {
			CrashBasicTop(&block[y][i]);
			animation.emplace_back(Block{ (block[y][i].x - 1) * side, (block[y][i].y - 1) * side, BreakBk, rand() % 4, 0 });
			block[y].erase(block[y].begin() + i);
			for (int j = 0; j < crash.size(); j++) {
				if (y == crash[j].j && i < crash[j].i)
					crash[j].i -= 1;
			}
			return;
		}
		case JumpBk: {
			Scheck = telpo;
			ball.y = block[y][i].y * side - rd;
			ball.vy = -65;
			ball.ax = 0;
			if (isLeftPressed || isRightPressed) ball.vx = ball.vx > 0 ? 21 : -21;
			else ball.vx = 0;
			return;
		}
		case RStraightBk:
		case LStraightBk: {
			Scheck = telpo;
			ball.x = block[y][i].type == RStraightBk ? (block[y][i].x + 1) * side + rd : block[y][i].x * side - rd;
			ball.y = block[y][i].y * side + side / 2;
			ball.vy = ball.ax = 0;
			ball.vx = block[y][i].type == RStraightBk ? 60 : -60;
			return;
		}
		case SwitchBk: {
			CrashBasicTop(&block[y][i]);
			isSwitchOff = 1 - isSwitchOff;
			return;
		}
		case OnceMvBk: {
			if (block[y][i].ani == 255) {
				CrashBasicTop(&block[y][i]);
				block[y][i].ani -= 30;
			}
			return;
		}
		case LauncherBk: {
			CrashBasicTop(&block[y][i]);
			MakeBullet(&block[y][i], 1);
			return;
		}
		case MusicBk: {
			CrashBasicTop(&block[y][i]);
			Scheck = music;
			return;
		}
		default: {
			if (block[y][i].type == ClimbBK && (block[y][i].subtype == 2 || block[y][i].subtype == 3)) {}
			else
				CrashBasicTop(&block[y][i]);
			return;
		}
		}
		break;
	}
	case dirRight: { // �� �����ʿ� �浹���� ���
		if (block[y][i].type == ClimbBK) {
			ball.x = block[y][i].x * side + side + rd;
			ball.vx = ball.ax = 0;
			ball.vy = 5.1; // ������ �浹
		}
		else
			CrashBasicRight(&block[y][i]);
		return;
	}
	case dirLeft: { // �� ���ʿ� �浹���� ���
		if (block[y][i].type == ClimbBK) {
			ball.x = block[y][i].x * side - rd;
			ball.vx = ball.ax = 0;
			ball.vy = 5;
		}
		else
			CrashBasicLeft(&block[y][i]);
		return;
	}
	case dirDown: { // �� �Ʒ��ʿ� �浹���� ���
		if (block[y][i].type == ClimbBK && (block[y][i].subtype == 2 || block[y][i].subtype == 1)) {}
		else
			CrashBasicBottom(&block[y][i]);
		return;
	}
	default: // �浹���� �ʾ��� ���
		return;
	}
}
int BlockQuality(const Block* block) {
	switch (block->type) {
	case Item:
	case Star:
		return 2;
	case ElectricBk:
	case RectBHBk:
	case CircleBHBk:
		return 1;
	default:
		return 0;
	}
}
int MyIntersectRect(const doubleRECT* ballrc, const doubleRECT* blockrc) {
	doubleRECT tempballrc;

	//vy�� ���� �浹�ߴ���
	tempballrc = { (double)ballrc->left, (double)ballrc->top - remy * t, (double)ballrc->right, (double)ballrc->bottom - remy * t };
	if (isCrashed(&tempballrc, blockrc) == 4) {
		if (ball.vy > 0)
			return dirUp;
		else
			return dirDown;
	}
	// vx�� ���� �浹�ߴ���
	tempballrc = { (double)ballrc->left - remx * t, (double)ballrc->top, (double)ballrc->right - remx * t, (double)ballrc->bottom - remy * t };
	if (isCrashed(&tempballrc, blockrc) == 4) {
		if (ball.vx < 0)
			return dirRight;
		else
			return dirLeft;
	}
	return isCrashed(ballrc, blockrc);
}
int isCrashed(const doubleRECT* ballrc, const doubleRECT* blockrc) {
	double dbleft = max(ballrc->left, blockrc->left);
	double dbtop = max(ballrc->top, blockrc->top);
	double dbright = min(ballrc->right, blockrc->right);
	double dbbottom = min(ballrc->bottom, blockrc->bottom);

	if (dbleft <= dbright && dbtop <= dbbottom) { // �浹�� ���
		if (dbright - dbleft < dbbottom - dbtop) { // �¿쿡�� �浹�� ���
			if (abs(ball.x - blockrc->right) <= abs(ball.x - blockrc->left)) // ���� �����ʿ� ���� ���
				return dirRight;
			else // ���� ���ʿ� ���� ���
				return dirLeft;
		}
		else { // ���Ͽ��� �浹�� ���
			if (abs(ball.y - blockrc->top) <= abs(ball.y - blockrc->bottom)) // ���� ���ʿ� ���� ���
				return dirUp;
			else // ���� �Ʒ��ʿ� ���� ���
				return dirDown;
		}
	}
	else { // �浹���� ���� ���
		return 4;
	}
}

//�˻�
Block* Search(const int type) {
	for (int y = 0; y < 15; y++) {
		for (int i = 0; i < block[y].size(); i++) {
			if (block[y][i].type == type) return &block[y][i];
		}
	}
	return 0;
}

// �⺻�� ���� �浹�� ���
void CrashBasicTop(const Block* block) {
	if (block->type != MusicBk && block->type != JumpBk && block->type != SwitchBk) {
		Scheck = ballcrach;
	}
	if (block->type == MoveBk)
		ball.y = block->y - rd;
	else
		ball.y = block->y * side - rd;
	ball.vy = -43;
	ball.ax = 0;
	if (isLeftPressed) ball.vx = ball.vx < 0 ? -21 : ball.vx;
	else if (isRightPressed) ball.vx = ball.vx > 0 ? 21 : ball.vx;
	else ball.vx = 0;
}
//�⺻�� �Ʒ��� �浹�� ���
void CrashBasicBottom(const Block* block) {
	Scheck = ballcrach;
	if (block->type == MoveBk)
		ball.y = block->y + side + rd;
	else
		ball.y = block->y * side + side + rd;
	ball.vy = -ball.vy;
}
//�⺻�� ���ʿ� �浹�� ���
void CrashBasicLeft(const Block* block) {
	if (block->type != JumpBk && block->type != SwitchBk) {
		Scheck = ballcrach;
	}
	if (block->type == MoveBk)
		ball.x = block->x - rd;
	else
		ball.x = block->x * side - rd;
	ball.vx = -ball.vx / 2;
	ball.ax = -7;
}
//�⺻�� �����ʿ� �浹�� ���
void CrashBasicRight(const Block* block) {
	if (block->type != JumpBk && block->type != SwitchBk) {
		Scheck = ballcrach;
	}
	if (block->type == MoveBk)
		ball.x = block->x + side + rd;
	else
		ball.x = block->x * side + side + rd;
	ball.vx = -ball.vx / 2;
	ball.ax = 7;
}

//�� �̵�
void MoveBall() {
	SHORT leftKeyState, rightKeyState;

	// �����ִ��� Ȯ��
	if (ball.vy != 5.1 && ball.vy != 5 && GamePlay != clear) {
		leftKeyState = GetAsyncKeyState(VK_LEFT);
		rightKeyState = GetAsyncKeyState(VK_RIGHT);
		isLeftPressed = (leftKeyState & 0x8000) != 0;
		isRightPressed = (rightKeyState & 0x8000) != 0;

		if (isLeftPressed && isRightPressed == false) { // ���ʸ� ������ ���
			if (abs(ball.vx) == 60) // �����ϰ� �־��� ���
				ball.vx = 0;
			if (abs(ball.vx) != 50) // �뽬�ϰ����� ���� �� �⺻
				ball.ax = -7;
		}
		else if (isRightPressed && isLeftPressed == false) { // �����ʸ� ������ ���
			if (abs(ball.vx) == 60) // �����ϰ� �־��� ���
				ball.vx = 0;
			if (abs(ball.vx) != 50)
				ball.ax = 7;
		}
		else if (ball.vx && isLeftPressed == false && isRightPressed == false && abs(ball.vx) != 60) // �Ѵ� �ȴ����� ���
			ball.ax = -ball.vx / 4.3;
	}

	ball.x += ball.vx * t;
	ball.y += ball.vy * t;
	remx = ball.vx;
	remy = ball.vy;
	ball.vx += ball.ax * t;
	if (abs(ball.vx) != 60) ball.vy += g * t;
	//���ӵ� ����
	if ((ball.ax == 7 && ball.vx >= 21 || ball.ax == -7 && ball.vx <= -21) && abs(ball.vx) != 50) {
		ball.ax = 0;
		ball.vx = ball.vx > 0 ? 21 : -21;
	}
}

//��ź ����, �̵�, �浹
void MakeBullet(const Block* block, int BulletType) {
	switch (block->subtype)
	{
	case dirRight: {
		bullet.emplace_back(Block{ (block->x + 1) * side, block->y * side + 10, dirRight, BulletType, 0 });
		break;
	}
	case dirLeft: {
		bullet.emplace_back(Block{ (block->x - 1) * side + 20, block->y * side + 10, dirLeft, BulletType, 0 });
		break;
	}
	case dirDown: {
		bullet.emplace_back(Block{ block->x * side + 10, (block->y + 1) * side, dirDown, BulletType, 0 });
		break;
	}
	case dirUp: {
		bullet.emplace_back(Block{ block->x * side + 10, (block->y - 1) * side + 20, dirUp, BulletType, 0 });
		break;
	}
	}
}
void MoveBullet() {
	for (int i = 0; i < bullet.size(); i++) {
		switch (bullet[i].type)
		{
		case dirRight: {
			bullet[i].x += 4;
			break;
		}
		case dirLeft: {
			bullet[i].x -= 4;
			break;
		}
		case dirDown: {
			bullet[i].y += 4;
			break;
		}
		case dirUp: {
			bullet[i].y -= 4;
			break;
		}
		}

		if (bullet[i].x + 42 <= window.left || bullet[i].x >= window.right || bullet[i].y + 42 <= window.top || bullet[i].y >= window.bottom) {
			bullet.erase(bullet.begin() + i);
		}
	}
}
void CrashBullet() {
	doubleRECT bulletrc, blockrc;

	for (int i = 0; i < bullet.size(); ++i) {
		bulletrc = { (double)bullet[i].x, (double)bullet[i].y, (double)bullet[i].x + 40, (double)bullet[i].y + 40 };

		// �� & ��ź �浹
		if (isCrashed(&ballrc, &bulletrc) != 4 && GamePlay != death && GamePlay != customdeath && GamePlay != survivaldeath) {
			animation.emplace_back(Block{ (int)ball.x - 90, (int)ball.y - 90, customdeath, rand() % 4, 0 });
			Scheck = balldeath;
			if (GamePlay == play)
				GamePlay = death;
			else if (GamePlay == customplay)
				GamePlay = customdeath;
			else if (GamePlay == survival)
				GamePlay = survivaldeath;
			return;
		}

		// �� & ��ź �浹
		for (int y = 0; y < 15; ++y) {
			for (int k = 0; k < block[y].size(); ++k) {
				if (block[y][k].type == MvBkStopBk || block[y][k].type == ElectricBk || block[y][k].type == RectWHBk || block[y][k].type == CircleWHBk)
					continue;

				if (block[y][k].type == MoveBk)
					blockrc = { (double)block[y][k].x, (double)block[y][k].y, (double)(block[y][k].x + side), (double)(block[y][k].y + side) };
				else
					blockrc = { (double)block[y][k].x * side, (double)block[y][k].y * side, (double)((block[y][k].x + 1) * side), (double)((block[y][k].y + 1) * side) };

				if (isCrashed(&bulletrc, &blockrc) != 4) {
					if (block[y][k].type == BreakBk && bullet[i].subtype)
						block[y].erase(block[y].begin() + k);
					bullet.erase(bullet.begin() + i);
				}
			}
		}
	}
}

//��ĭ�̵��� �̵� ��ġ �˻�, �̵�
bool OnceMvBkGo(const Block* b)
{
	switch (b->subtype)
	{
	case dirRight: {
		for (int i = 0; i < block[b->y].size(); ++i) {
			if (block[b->y][i].type == MoveBk) {
				if ((b->x + 1) * side < block[b->y][i].x + side && (b->x + 1) * side + side > block[b->y][i].x)
					return false;
			}
			else if (b->x + 1 == block[b->y][i].x && (block[b->y][i].type != ElectricBk && block[b->y][i].type != MvBkStopBk))
				return false;
		}
		break;
	}
	case dirLeft: {
		for (int i = 0; i < block[b->y].size(); ++i) {
			if (block[b->y][i].type == MoveBk) {
				if ((b->x - 1) * side < block[b->y][i].x + side && (b->x - 1) * side + side > block[b->y][i].x)
					return false;
			}
			else if (b->x - 1 == block[b->y][i].x && (block[b->y][i].type != ElectricBk && block[b->y][i].type != MvBkStopBk))
				return false;
		}
		break;
	}
	case dirDown: {
		for (int i = 0; i < block[b->y + 1].size(); ++i) {
			if (block[b->y + 1][i].type == MoveBk) {
				if (b->x * side < block[b->y + 1][i].x + side && b->x * side + side > block[b->y + 1][i].x)
					return false;
			}
			else if (b->x == block[b->y + 1][i].x && (block[b->y + 1][i].type != ElectricBk && block[b->y + 1][i].type != MvBkStopBk))
				return false;
		}
		break;
	}
	case dirUp: {
		for (int i = 0; i < block[b->y - 1].size(); ++i) {
			if (block[b->y - 1][i].type == MoveBk) {
				if (b->x * side < block[b->y - 1][i].x + side && b->x * side + side > block[b->y - 1][i].x)
					return false;
			}
			else if (b->x == block[b->y - 1][i].x && (block[b->y - 1][i].type != ElectricBk && block[b->y - 1][i].type != MvBkStopBk))
				return false;
		}
		break;
	}
	default:
		break;
	}
	return true;
}
void MoveOnceMvBk(int y, int i) {
	switch (block[y][i].subtype)
	{
	case dirRight:
		if (block[y][i].x < 24 && OnceMvBkGo(&block[y][i])) {
			block[y][i].x++;
		}
		break;
	case dirLeft:
		if (block[y][i].x > 0 && OnceMvBkGo(&block[y][i])) {
			block[y][i].x--;
		}
		break;
	case dirUp:
		if (y > 0 && OnceMvBkGo(&block[y][i])) {
			block[y - 1].emplace_back(block[y][i]);
			block[y - 1].back().y -= 1;
			block[y].erase(block[y].begin() + i);
		}
		break;
	case dirDown:
		if (y < 14 && OnceMvBkGo(&block[y][i])) {
			block[y + 1].emplace_back(block[y][i]);
			block[y + 1].back().y += 1;
			block[y].erase(block[y].begin() + i);
		}
		break;
	}
}

// �̵��� �̵�, ������ȯ
bool MoveMoveBk(Block* b)
{
	if (b->x < 0 || b->x + side > window.right) {
		TurnMoveBk(b);
	}

	for (int k = 0; k < block[b->y / side].size(); ++k) {
		if (block[b->y / side][k].type == MoveBk && block[b->y / side][k].ani == b->ani) continue;//���� ���� �׷��� �̵��� �浹üũ ���ϰ� �����߰�

		if (b->x + b->subtype < ((block[b->y / side][k].x + 1) * side) && b->x + side + b->subtype >(block[b->y / side][k].x * side)) {//�̰� �̰Ÿ� �ᵵ �浹üũ ��
			TurnMoveBk(b);//�ݺ��Ǵºκ� �Լ��� �����ϴ� �߰��� �Լ��� ������
			break;
		}
	}
	return false;
}
void TurnMoveBk(Block* b)
{
	if (b->subtype < 0) {
		b->x -= b->subtype * 2;
	}
	for (int k = 0; k < block[b->y / side].size(); ++k) {
		if (block[b->y / side][k].type != MoveBk || block[b->y / side][k].ani != b->ani) continue;
		else {
			block[b->y / side][k].subtype *= -1;
		}
	}
}

// ������ ���
void UseItem() {
	int tempx, crashStart, crashEnd, crashReturn;
	doubleRECT ballrc, blockrc;

	switch (ball.item) {
	case ShortTelpo:
		Scheck = telpo;
		if (ball.vx >= 0) ballrc = { (ball.x + rd), (ball.y - rd), (ball.x + side * 3 + rd), (ball.y + rd) };
		else ballrc = { (ball.x - rd), (ball.y - rd), (ball.x - side * 3 + rd), (ball.y + rd) };

		ball.item = Normal;
		crashStart = ballrc.top >= 0 ? (int)(ballrc.top / 60) : 0;
		crashEnd = (int)(ballrc.bottom / 60);

		if (ball.vx >= 0) {
			for (int y = crashStart; y <= crashEnd; ++y) {
				for (int i = 0; i < block[y].size(); i++) {
					if (block[y][i].type == MoveBk)
						blockrc = { (double)block[y][i].x, (double)block[y][i].y, (double)(block[y][i].x + side), (double)(block[y][i].y + side) };
					else
						blockrc = { (double)block[y][i].x * side, (double)block[y][i].y * side, (double)((block[y][i].x + 1) * side), (double)((block[y][i].y + 1) * side) };
					if (isCrashed(&ballrc, &blockrc) != 4 && i != block[y].size() - 1 && block[y][i + 1].x - block[y][i].x == 1) {
						ball.x = blockrc.left - rd;
						return;
					}
				}
			}
			ball.x += side * 3;
		}

		else {
			for (int y = crashStart; y <= crashEnd; ++y) {
				for (int i = block[y].size() - 1; i >= 0; i--) {
					if (block[y][i].type == MoveBk)
						blockrc = { (double)block[y][i].x, (double)block[y][i].y, (double)(block[y][i].x + side), (double)(block[y][i].y + side) };
					else
						blockrc = { (double)block[y][i].x * side, (double)block[y][i].y * side, (double)((block[y][i].x + 1) * side), (double)((block[y][i].y + 1) * side) };
					if (isCrashed(&ballrc, &blockrc) != 4 && i > 0 && block[y][i].x - block[y][i - 1].x == 1) {
						ball.x = blockrc.right + rd;
						return;
					}
				}
			}
			ball.x -= side * 3;
		}
		break;
	case Dash:
		ball.item = Normal;
		ball.vy = -10;
		ball.vx = ball.vx > 0 ? 50 : -50;
		ball.ax = 0;
		break;
	case HighJmp:
		ball.item = Normal;
		ball.vy = -50;
		ball.vx = ball.ax = 0;
		break;
	}
}

// ����
void MakeBlockList() {
	list[0].type = Star;
	for (int i = 0; i < 5; i++) {
		list[i + 1] = { 0, 0, Item, i, 0 };
	}
	list[6].type = JumpBk;
	list[7].type = RStraightBk;
	list[8].type = LStraightBk;
	list[9].type = RectBHBk;
	list[10].type = RectWHBk;
	list[11].type = CircleBHBk;
	list[12].type = CircleWHBk;
	list[13] = { 0, 0, SwitchBk, 0, 0 };
	list[14] = { 0, 0, SwitchBk, 1, 0 };
	list[15] = { 0, 0, ElectricBk, 0, 0 };
	list[16].type = MvBkStopBk;
	list[17] = { 0, 0, MoveBk, 1, 0 };
	list[18].type = BreakBk;
	list[19].type = ClimbBK;
	list[20].type = MusicBk;
	for (int i = 0; i < 11; i++) {
		list[i + 21] = { 0, 0, BasicBk, i, 0 };
	}
	for (int i = 0; i < 4; i++) {
		list[i + 32] = { 0, 0, OnceMvBk, i, 255 };
		list[i + 36] = { 0, 0, LauncherBk, i, 0 };
	}
	list[40] = { 0, 0, LightBk, 0, 0 };
	list[41] = { 0, 0, LightBk, 2, 0 };
	list[42] = { 0, 0, LightBk, 4, 0 };
}

// �� �迭���� ���ͷ� ��ȯ (�� ��ǥ, ����ġ ���´� ���� �ޱ�)
void MakeVector() {
	ClearVector();
	Block temp;
	int groupcnt = 1; // �̵��� �׷�
	bool Continuous = false;
	starcnt = 0;
	ball.item = ball.state = 0;

	if (GamePlay == death || GamePlay == customdeath || GamePlay == customplay) {
		for (int i = 0; i < 15; i++) {
			for (int j = 0; j < 25; j++) {
				if (Map[i][j]) { // ���� ���
					// ��
					if (Map[i][j] == 1)
						starcnt++;

					temp.x = Map[i][j] - 1 == 17 ? j * side : j;
					temp.y = Map[i][j] - 1 == 17 ? i * side : i;
					temp.type = list[Map[i][j] - 1].type;
					if (Map[i][j] - 1 == 13 || Map[i][j] - 1 == 14 || Map[i][j] - 1 == 15) // ���� ���� ��
						temp.subtype = isSwitchOff;
					else
						temp.subtype = list[Map[i][j] - 1].subtype;
					if (Continuous) {
						if (Map[i][j] - 1 == 17 && Map[i][j - 1] - 1 == 17) temp.ani = groupcnt;
						else {
							Continuous = false;
							groupcnt++;
						}
					}
					if (Map[i][j] - 1 == 17) {
						temp.ani = groupcnt;
						Continuous = true;
					}
					if (Map[i][j] - 1 != 17)
						temp.ani = list[Map[i][j] - 1].ani;

					// ������ �׷�ȭ
					if (Map[i][j] == 20) {
						// �� ���� ���̰ų�, �� ���� �Ʒ��� �ƴϰ� �� ���� �����̰� �ƴϰ� �Ʒ��� �����̸� 1��
						if (i == 0 || i < 14 && Map[i - 1][j] != 20 && Map[i + i][j] == 20)
							temp.subtype = 1;
						// �� ���� ���� �Ʒ��� �ƴϰ� �� ���� �Ʒ��� �����̸� 2��
						else if (i > 0 && i < 14 && Map[i - 1][j] == 20 && Map[i + 1][j] == 20)
							temp.subtype = 2;
						// �� ���� �Ʒ��̰ų�, �� ���� ���� �ƴϰ� �� ���� �����̰� �Ʒ��� �����̰� �ƴϸ� 2��
						else if (i == 14 || i > 0 && Map[i - 1][j] == 20 && Map[i + 1][j] != 20)
							temp.subtype = 3;
						else
							temp.subtype = 4;
					}
					block[i].emplace_back(temp);
				}
			}
		}
	}
	else if (GamePlay == survivalready) {
		random = 12;
		for (int a = 0; a < 3; ++a) {
			if (random <= ((SVMAPCNT - 1) / 2)) // 0 ~ 11�̸�
				random = rand() % (SVMAPCNT / 2) + (SVMAPCNT / 2);
			else // 12 ~ 23�̸�
				random = rand() % (SVMAPCNT / 2);

			if (a == 2) // ù��° ��
				random = 0;

			for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 9; j++) {
					if (SurvivalMap[random][i][j]) { // ���� ���
						temp.x = SurvivalMap[random][i][j] - 1 == 17 ? (j + 8) * side : j + 8;
						temp.y = SurvivalMap[random][i][j] - 1 == 17 ? (i + (a * 4)) * side : i + (a * 4);
						temp.type = list[SurvivalMap[random][i][j] - 1].type;
						if (SurvivalMap[random][i][j] - 1 == 13 || SurvivalMap[random][i][j] - 1 == 14 || SurvivalMap[random][i][j] - 1 == 15)
							temp.subtype = isSwitchOff;
						else
							temp.subtype = list[SurvivalMap[random][i][j] - 1].subtype;

						// �̵��� ����
						if (Continuous) {
							if (SurvivalMap[random][i][j] - 1 == 17 && SurvivalMap[random][i][j - 1] - 1 == 17)
								temp.ani = groupcnt;
							else {
								Continuous = false;
								groupcnt++;
							}
						}
						if (SurvivalMap[random][i][j] - 1 == 17) {
							temp.ani = groupcnt;
							Continuous = true;
						}
						if (SurvivalMap[random][i][j] - 1 != 17)
							temp.ani = list[SurvivalMap[random][i][j] - 1].ani;

						// ������ �׷�ȭ
						if (SurvivalMap[random][i][j] == 20) {
							// �� ���� ���̰ų�, �� ���� �Ʒ��� �ƴϰ� �� ���� �����̰� �ƴϰ� �Ʒ��� �����̸� 1��
							if (i == 0 || i < 14 && SurvivalMap[random][i - 1][j] != 20 && SurvivalMap[random][i + i][j] == 20)
								temp.subtype = 1;
							// �� ���� ���� �Ʒ��� �ƴϰ� �� ���� �Ʒ��� �����̸� 2��
							else if (i > 0 && i < 14 && SurvivalMap[random][i - 1][j] == 20 && SurvivalMap[random][i + 1][j] == 20)
								temp.subtype = 2;
							// �� ���� �Ʒ��̰ų�, �� ���� ���� �ƴϰ� �� ���� �����̰� �Ʒ��� �����̰� �ƴϸ� 2��
							else if (i == 14 || i > 0 && SurvivalMap[random][i - 1][j] == 20 && SurvivalMap[random][i + 1][j] != 20)
								temp.subtype = 3;
							else
								temp.subtype = 4;
						}
						block[i + (a * 4)].emplace_back(temp);
					}
				}
			}
		}
	}
}
void ClearVector() { // �� �� �ʱ�ȭ�ϰ���
	bullet.clear();
	animation.clear();
	for (int i = 0; i < 15; i++) {
		block[i].clear();
	}
}
void MakeReadyVector() {
	ClearReadyVector();
	Block temp;
	int groupcnt = 1; // �̵��� �׷�
	bool Continuous = false;
	if (random <= ((SVMAPCNT - 1) / 2))
		random = rand() % ((SVMAPCNT - 1) / 2) + ((SVMAPCNT - 1) / 2) + 1;
	else
		random = rand() % ((SVMAPCNT - 1) / 2);

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 9; j++) {
			if (SurvivalMap[random][i][j]) { // ���� ���
				temp.x = SurvivalMap[random][i][j] - 1 == 17 ? (j + 8) * side : j + 8;
				temp.y = 0;
				temp.type = list[SurvivalMap[random][i][j] - 1].type;
				if (SurvivalMap[random][i][j] - 1 == 13 || SurvivalMap[random][i][j] - 1 == 14 || SurvivalMap[random][i][j] - 1 == 15)
					temp.subtype = isSwitchOff;
				else
					temp.subtype = list[SurvivalMap[random][i][j] - 1].subtype;

				// �̵��� ����
				if (Continuous) {
					if (SurvivalMap[random][i][j] - 1 == 17 && SurvivalMap[random][i][j - 1] - 1 == 17) temp.ani = groupcnt;
					else {
						Continuous = false;
						groupcnt++;
					}
				}
				if (SurvivalMap[random][i][j] - 1 == 17) {
					temp.ani = groupcnt;
					Continuous = true;
				}
				if (SurvivalMap[random][i][j] - 1 != 17)
					temp.ani = list[SurvivalMap[random][i][j] - 1].ani;

				// ������ �׷�ȭ
				if (SurvivalMap[random][i][j] == 20) {
					// �� ���� ���̰ų�, �� ���� �Ʒ��� �ƴϰ� �� ���� �����̰� �ƴϰ� �Ʒ��� �����̸� 1��
					if (i == 0 || i < 14 && SurvivalMap[random][i - 1][j] != 20 && SurvivalMap[random][i + i][j] == 20)
						temp.subtype = 1;
					// �� ���� ���� �Ʒ��� �ƴϰ� �� ���� �Ʒ��� �����̸� 2��
					else if (i > 0 && i < 14 && SurvivalMap[random][i - 1][j] == 20 && SurvivalMap[random][i + 1][j] == 20)
						temp.subtype = 2;
					// �� ���� �Ʒ��̰ų�, �� ���� ���� �ƴϰ� �� ���� �����̰� �Ʒ��� �����̰� �ƴϸ� 2��
					else if (i == 14 || i > 0 && SurvivalMap[random][i - 1][j] == 20 && SurvivalMap[random][i + 1][j] != 20)
						temp.subtype = 3;
					else
						temp.subtype = 4;
				}
				Readyblock[i].emplace_back(temp);
			}
		}
	}
}
void ClearReadyVector() {
	for (int i = 0; i < 4; i++) {
		Readyblock[i].clear();
	}
}