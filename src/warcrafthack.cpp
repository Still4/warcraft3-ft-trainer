// gamehack.cpp: ���������� ����� ����� ��� ����������� ����������.
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
	HWND Game = FindGame((LPCTSTR)"Warcraft III"); //����� ���� ����
	DWORD PID = FindPID(Game); //��������� PID ����
	SetPriveleges(); //������������� ���������� �������� ��������
	HANDLE hProcess = OpenhProcess(PID); //��������� �������

	BOOL Reverse = FALSE; //���� ������������� ��������
	DWORD StartAdr; //����� ����� � ������
	int points = 1; //������� ����������

	SetTimer(NULL, NULL, 100, NULL); //��������� ������ �� 100��

	MSG msg = {0}; //��������� ����
	while(GetMessage(&msg, NULL, 0, 0) != 0)
	{
		switch (msg.message)
		{
			case WM_TIMER: // ���� �������� ������
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