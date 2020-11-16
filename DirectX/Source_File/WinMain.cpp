////////////////////////////////////////////////////////////////////////////////
// Filename: main.cpp
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "../Header_File/SystemClass.h"
//#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")


int APIENTRY wWinMain(_In_ HINSTANCE hinstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPWSTR IpCmdLine,_In_ int nCmdShow)
{
	SystemClass* System = new SystemClass();

	if (!System)
	{
		return -1;
	}

	if (System->Initialize())
	{
		System->Run();
	}

	System->ShutDown();
	delete System;
	System = nullptr;

	return 0;
}