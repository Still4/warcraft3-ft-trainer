#ifndef _HACKFUNC_H_
#define _HACKFUNC_H_

HWND FindGame(LPCTSTR lpWindowName); //������� - ����� ���� ����
DWORD FindPID(HWND Game); //������� - ����� ���� ���� � ���������� ��� PID
void SetPriveleges(); //������� - ���� ���������� �������� ��������
HANDLE OpenhProcess(DWORD PID); //������� - ������� ������� �� PID 
DWORD FindAdr(HANDLE hProcess); //������� - ����� ���������
SIZE_T ReadAdr(HANDLE hProcess, DWORD StartAdr, SIZE_T Size); //������� - ��������� ������
void WriteToAdr(HANDLE hProcess, DWORD Adr, SIZE_T Size, DWORD Code); //������� - ������� � ������
BOOL Key(int vKey); //������� - ���������� ���������� ������
void ShowText(LPVOID Text); //�������� � ���� ����
void AddStr(HANDLE hProcess, DWORD StartAdr, int points, BOOL Reverse); //������� - �������� ���� ����
void AddAgi(HANDLE hProcess, DWORD StartAdr, int points, BOOL Reverse); //������� - �������� ���� ��������
DWORD ReverseCode(DWORD Code);

#endif