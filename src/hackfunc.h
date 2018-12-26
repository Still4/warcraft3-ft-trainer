#ifndef _HACKFUNC_H_
#define _HACKFUNC_H_

HWND FindGame(LPCTSTR lpWindowName); //функция - поиск окна игры
DWORD FindPID(HWND Game); //функция - поиск окна игры и вычисление его PID
void SetPriveleges(); //функция - дать привелегии текущему процессу
HANDLE OpenhProcess(DWORD PID); //функция - открыть процесс по PID 
DWORD FindAdr(HANDLE hProcess); //функция - найти указатель
SIZE_T ReadAdr(HANDLE hProcess, DWORD StartAdr, SIZE_T Size); //функция - прочитать память
void WriteToAdr(HANDLE hProcess, DWORD Adr, SIZE_T Size, DWORD Code); //функция - вписать в память
BOOL Key(int vKey); //функция - определить состояниие кнопки
void ShowText(LPVOID Text); //написать в окне игры
void AddStr(HANDLE hProcess, DWORD StartAdr, int points, BOOL Reverse); //функция - добавить очки силы
void AddAgi(HANDLE hProcess, DWORD StartAdr, int points, BOOL Reverse); //функция - добавить очки ловкости
DWORD ReverseCode(DWORD Code);

#endif