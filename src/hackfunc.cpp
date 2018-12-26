//hackfunc.cpp
#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include "hackfunc.h"

using namespace std;

HWND FindGame(LPCTSTR lpWindowName)
{
	HWND Game = FindWindow(NULL, lpWindowName); //находим окно по имени
	if(!Game)
	{
		cout << "Error: " << dec << GetLastError() << endl;
		return 0;
	} else 
	{
		cout << "Game finded" << endl;
		return Game;
	}
}

DWORD FindPID(HWND Game)
{
	DWORD PID;
	GetWindowThreadProcessId(Game, &PID); //получаем PID
	if(!PID)
	{
		cout << "Error: " << dec << GetLastError() << endl;
		return 0;
	}else
	{
		cout << "PID finded" << endl;
		return PID;
	}
}

void SetPriveleges()
{
	HANDLE hToken; //создаем token
	TOKEN_PRIVILEGES TokenPriv; //создаем структуру привелегий
	LUID SeDebugNameValue; //структура привелегии
		LookupPrivilegeValue(
					NULL,
					(LPCTSTR)"SeDebugPrivilege",
					&SeDebugNameValue); //узнаем значение привелегии в данной системе
		TokenPriv.PrivilegeCount = 1; // устанавливаем количество привелегий
		TokenPriv.Privileges[0].Luid = SeDebugNameValue; //устанавливаем привелегию
		TokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; //устанавливаем флаг включения привелегии
	BOOL OpenToken = OpenProcessToken(
					GetCurrentProcess(),
					TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					&hToken); //получаем token нашего процесса
	if(!OpenToken)
	{
		cout << "Error: " << dec << GetLastError() << endl;
	} else 
	{
		cout << "Token opened" << endl;
		BOOL AdjustPriv = AdjustTokenPrivileges(
					hToken,
					FALSE,
					&TokenPriv,
					sizeof(TokenPriv),
					NULL,
					NULL); //даем привелегии нашему процессу
		if(!AdjustPriv)
		{
			cout << "Error: " << dec << GetLastError() << endl;
		} else 
		{
			cout << "Priveleges granted" << endl;
		}
	}
}

HANDLE OpenhProcess(DWORD PID)
{
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | SYNCHRONIZE | PROCESS_QUERY_INFORMATION | PROCESS_CREATE_THREAD,
					FALSE,
					PID); //открывем процесс
	if(!hProcess)
	{
		cout << "Error: " << dec << GetLastError() << endl;
		return 0;
	} else 
	{
		cout << "Process opened" << endl;
		return hProcess;
	}
}

DWORD FindAdr(HANDLE hProcess)
{
	LPVOID VirtualAdr;
	VirtualAdr = VirtualAllocEx(
				hProcess,
				NULL,
				4,
				MEM_COMMIT,
				PAGE_READWRITE
				); //allocate additional memory and set address
	if(!VirtualAdr)
	{
		cout << "Error: " << dec << GetLastError() << endl;
	} else
	{
		cout << "Virtual address setted at " << hex << VirtualAdr << endl;
	}
	BYTE OldCode;
	DWORD OldCode2;
	OldCode = ReadAdr(hProcess, 0x6F20ACF8, sizeof(OldCode)); //this address contains some instruction
	OldCode2 = ReadAdr(hProcess, 0x6F20ACF9, sizeof(OldCode2)); //this address contains (points to) another address?
	BYTE NewCode = 0x35; //XOR instruction for x86 processor?
	DWORD State = 1;
	WriteToAdr(hProcess, 0x6F20ACF8, sizeof(NewCode), NewCode); //replace instuction
	WriteToAdr(hProcess, 0x6F20ACF9, sizeof(VirtualAdr), (DWORD)VirtualAdr); //replace old address by new allocated virtual memory address
	WriteToAdr(hProcess, (DWORD)VirtualAdr, sizeof(State), State); //write State to allocated virtual memory
	DWORD StartAdr = 1;
	while(StartAdr == State) //this will be false when value of desired address will be written to the value of virtual allocated address
	{
		Sleep(5000);
		StartAdr = ReadAdr(hProcess, (DWORD)VirtualAdr, sizeof(StartAdr)); //when Game will run method which change Agility or Strength address of another method will be written to allocated virtual memory
	}
	WriteToAdr(hProcess, 0x6F20ACF8, sizeof(OldCode), OldCode); //restore previous value
	WriteToAdr(hProcess, 0x6F20ACF9, sizeof(OldCode2), OldCode2); //restore previous value
	cout << "\a" << hex << StartAdr-1 << endl;
	return StartAdr-1; //why substract 1?
}

SIZE_T ReadAdr(HANDLE hProcess, DWORD StartAdr, SIZE_T Size)
{
	DWORD Adr;
	ReadProcessMemory(hProcess,
				(void*)(StartAdr),
				(void*)(&Adr),
				Size,
				NULL); //считываем из памяти
	if(!Adr)
	{
		cout << "Error: " << dec << GetLastError() << endl;
		return 0;
	} else 
	{
		cout << "Address readed" << endl;
		return Adr;
	}
}

void WriteToAdr(HANDLE hProcess, DWORD Adr, SIZE_T Size, DWORD Code)
{
	BOOL WriteFlag = WriteProcessMemory(hProcess,
				(void*)(Adr),
				(void*)(&Code),
				Size,
				NULL); //пишем в память
	if(!WriteFlag)
	{
		cout << "Error: " << GetLastError() << endl;
	} else 
	{
		cout << "Code writed" << endl;
	}
}

BOOL Key(int vKey)
{
	SHORT KeyState = GetKeyState(vKey); //определяем статус кнопки
		switch (KeyState)
		{
			case -127:
			case -128:
				return 1;
				break;
			default:
				return 0;
				break;
		}
}

void AddStr(HANDLE hProcess, DWORD StartAdr, int points, BOOL Reverse)
{
	DWORD Code; //параметр для записи

	StartAdr += 0x94; //адрес входа в память
	Code = ReadAdr(hProcess, StartAdr, sizeof(Code)); //получаем текущее значение
	if(Reverse == TRUE)
		Code = ReverseCode(Code);
	cout << "Strenght was: " << dec << Code << endl; //выводим значение в hex
	Code +=  points; //записываемое значение
	if(Reverse == TRUE)
		Code = ReverseCode(Code);
	cout << "Strenght now: " << dec << Code << endl; //выводим значение в hex
	WriteToAdr(hProcess, StartAdr, sizeof(Code), Code); //записываем Code
}

void AddAgi(HANDLE hProcess, DWORD StartAdr, int points, BOOL Reverse)
{
	DWORD Code; //параметр для записи

	StartAdr += 0x0A8;
	Code = ReadAdr(hProcess, StartAdr, sizeof(Code)); //получаем текущее значение
	if(Reverse == TRUE)
		Code = ReverseCode(Code);
	cout << "Agility was: " << dec << Code << endl; //выводим значение в hex
	Code +=  points; //записываемое значение
	if(Reverse == TRUE)
		Code = ReverseCode(Code);
	cout << "Agility now: " << dec << Code << endl; //выводим значение в hex
	WriteToAdr(hProcess, StartAdr, sizeof(Code), Code); //записываем Code
}

DWORD ReverseCode(DWORD Code)
{
	Code = ((((Code) & 0xFF000000) >> 24) | (((Code) & 0x00FF0000) >> 8) | \
		(((Code) & 0x0000FF00) << 8) | (((Code) & 0x000000FF) << 24));
	return Code;
}







/*
DWORD FindAdr(HWND Game)
{
	DWORD StartAdr = 0;
	DWORD GameThreadId; //Id дескриптора окна игры
	GameThreadId = GetWindowThreadProcessId(
										Game,
										NULL
										);
	if(!GameThreadId)
	{
		cout << "Error: " << dec << GetLastError() << endl;
	} else
	{
		cout << "ThreadId finded" << endl;
	}

	HANDLE GameThread; //открываем дескриптор окна игры
	GameThread = OpenThread(
					THREAD_GET_CONTEXT | SYNCHRONIZE,
					NULL,
					GameThreadId
					);
	if(!GameThread)
	{
		cout << "Error: " << dec << GetLastError() << endl;
	} else
	{
		DWORD Adr = 0x6F20ACF8;
		CONTEXT context;
		context.ContextFlags = CONTEXT_FULL;
		while(!StartAdr)
		{
			if(!GetThreadContext(GameThread, &context))
			{
				cout << "Error: " << dec << GetLastError() << endl;
			} else 
			{
				if(context.Eip != 0x7C90E514)
					cout << "EIP = " << hex << context.Eip << endl;
				if(context.Eip == Adr)
				{
					StartAdr = context.Esi;
					cout << "\a" << hex << context.Esi;
				}
			}
		}
	}
	CloseHandle(GameThread); // закрываем дескриптор
	return StartAdr;
}
*/

/*
		
		__try{
				context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
                GetThreadContext(GameThread, &context); // считываем регистровый контекст
				context.Dr0 = 0x6F20ACF7;
				context.Dr7 = (context.Dr7 & 0xFFF0FFFF) | 0x101; // включаем точку останова номер 0
				SetThreadContext(GameThread, &context); // устанавливаем точку останова номер 0 по адресу Dr0
				cout << "Point setted" << endl;
        }

        __except(1)
        {
				context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
                GetThreadContext(GameThread, &context); // считываем регистровый контекст
				context.Dr7 = (context.Dr7 & 0xFFFFFFFE); // выключаем точку останова номер 0
				SetThreadContext(GameThread, &context); // выключаем точку останова
				context.ContextFlags = CONTEXT_INTEGER;
				GetThreadContext(GameThread, &context); // считываем регистровый контекст
				cout << "Context: " << hex << context.Eax << endl;
        }
*/
        