// gamehack.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include "hackfunc.h"
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Advapi32.lib")


using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	HWND Game = FindGame((LPCTSTR)"Warcraft III"); //найти окно игры
	DWORD PID = FindPID(Game); //вычислить PID игры
	SetPriveleges(); //устанавливаем привелегии текущему процессу
	HANDLE hProcess = OpenhProcess(PID); //открываем процесс

	BOOL Reverse = FALSE; //флаг перевернутого значения
	DWORD StartAdr; //адрес входа в память
	int points = 1; //сколько прибавлять

	SetTimer(NULL, NULL, 100, NULL); //поставить таймер на 100мс

	MSG msg = {0}; //сообщения окна
	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		switch (msg.message)
		{
			case WM_TIMER: // если сработал таймер
				if(Key(VK_NUMPAD1))// Num1
					AddStr(hProcess, StartAdr, points, Reverse);
				if(Key(VK_NUMPAD2))// Num2
					AddAgi(hProcess, StartAdr, points, Reverse);
				if(Key(VK_NUMPAD5))// Num5
					StartAdr = FindAdr(hProcess);
				if(Key(VK_NUMPAD6))// Num6
				{
					if(Reverse == FALSE)
						Reverse = TRUE;
					else
						Reverse = FALSE;
				}

				if(Key(VK_NUMPAD7))// Num7
				{
					if(points < 1000)
					{
						points++;
						cout << "Now points = " << dec << points << endl;
					}
				}
				if(Key(VK_NUMPAD8))// Num8
				{
					if(points > 1)
					{
						points--;
						cout << "Now points = " << dec << points << endl;
					}
				}
				if(Key(VK_NUMPAD9)) // Num9
					PostQuitMessage(0);
				break;
			default:
				break;
		}
	} 
	CloseHandle(hProcess);
	return 0;
}