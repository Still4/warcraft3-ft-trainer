//hackfunc.cpp
#include "stdafx.h"
#include <iostream>
#include <windows.h>
#include "hackfunc.h"

using namespace std;

HWND FindGame(LPCTSTR lpWindowName)
{
	HWND Game = FindWindow(NULL, lpWindowName); //������� ���� �� �����
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
	GetWindowThreadProcessId(Game, &PID); //�������� PID
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
	HANDLE hToken; //������� token
	TOKEN_PRIVILEGES TokenPriv; //������� ��������� ����������
	LUID SeDebugNameValue; //��������� ����������
		LookupPrivilegeValue(
					NULL,
					(LPCTSTR)"SeDebugPrivilege",
					&SeDebugNameValue); //������ �������� ���������� � ������ �������
		TokenPriv.PrivilegeCount = 1; // ������������� ���������� ����������
		TokenPriv.Privileges[0].Luid = SeDebugNameValue; //������������� ����������
		TokenPriv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; //������������� ���� ��������� ����������
	BOOL OpenToken = OpenProcessToken(
					GetCurrentProcess(),
					TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
					&hToken); //�������� token ������ ��������
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
					NULL); //���� ���������� ������ ��������
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
					PID); //�������� �������
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
				NULL); //��������� �� ������
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
				NULL); //����� � ������
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
	SHORT KeyState = GetKeyState(vKey); //���������� ������ ������
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
	DWORD Code; //�������� ��� ������

	StartAdr += 0x94; //����� ����� � ������
	Code = ReadAdr(hProcess, StartAdr, sizeof(Code)); //�������� ������� ��������
	if(Reverse == TRUE)
		Code = ReverseCode(Code);
	cout << "Strenght was: " << dec << Code << endl; //������� �������� � hex
	Code +=  points; //������������ ��������
	if(Reverse == TRUE)
		Code = ReverseCode(Code);
	cout << "Strenght now: " << dec << Code << endl; //������� �������� � hex
	WriteToAdr(hProcess, StartAdr, sizeof(Code), Code); //���������� Code
}

void AddAgi(HANDLE hProcess, DWORD StartAdr, int points, BOOL Reverse)
{
	DWORD Code; //�������� ��� ������

	StartAdr += 0x0A8;
	Code = ReadAdr(hProcess, StartAdr, sizeof(Code)); //�������� ������� ��������
	if(Reverse == TRUE)
		Code = ReverseCode(Code);
	cout << "Agility was: " << dec << Code << endl; //������� �������� � hex
	Code +=  points; //������������ ��������
	if(Reverse == TRUE)
		Code = ReverseCode(Code);
	cout << "Agility now: " << dec << Code << endl; //������� �������� � hex
	WriteToAdr(hProcess, StartAdr, sizeof(Code), Code); //���������� Code
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
	DWORD GameThreadId; //Id ����������� ���� ����
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

	HANDLE GameThread; //��������� ���������� ���� ����
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
	CloseHandle(GameThread); // ��������� ����������
	return StartAdr;
}
*/

/*
		
		__try{
				context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
                GetThreadContext(GameThread, &context); // ��������� ����������� ��������
				context.Dr0 = 0x6F20ACF7;
				context.Dr7 = (context.Dr7 & 0xFFF0FFFF) | 0x101; // �������� ����� �������� ����� 0
				SetThreadContext(GameThread, &context); // ������������� ����� �������� ����� 0 �� ������ Dr0
				cout << "Point setted" << endl;
        }

        __except(1)
        {
				context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
                GetThreadContext(GameThread, &context); // ��������� ����������� ��������
				context.Dr7 = (context.Dr7 & 0xFFFFFFFE); // ��������� ����� �������� ����� 0
				SetThreadContext(GameThread, &context); // ��������� ����� ��������
				context.ContextFlags = CONTEXT_INTEGER;
				GetThreadContext(GameThread, &context); // ��������� ����������� ��������
				cout << "Context: " << hex << context.Eax << endl;
        }
*/
        