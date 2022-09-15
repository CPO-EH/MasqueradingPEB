#include <Windows.h>
#include <stdio.h>
#include <winternl.h>

#define PATH MAX_PATH

typedef NTSTATUS(*MYPROC) (HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

int Error(const char* msg) {
	printf("%s (%u)", msg, GetLastError());
	return -1;
}

int main()
{
	HANDLE h = GetCurrentProcess();
	if (!h)
		Error("Failed in GetCurrentProcess\n");

	PROCESS_BASIC_INFORMATION ProcessInformation;
	ULONG lenght = 0;
	HINSTANCE ntdll;
	MYPROC ptrNtQueryInformationProcess;
	wchar_t commandline[] = L"C:\\windows\\system32\\notepad.exe";
	
	ntdll = LoadLibrary(TEXT("Ntdll.dll"));
	if (!ntdll)
		return Error("Failed in LoadLibrary\n");

	//resolve address of NtQueryInformationProcess in ntdll.dll
	ptrNtQueryInformationProcess = (MYPROC)GetProcAddress(ntdll, "NtQueryInformationProcess");
	printf("[+] NtQueryInformationProcess : %p\n", ptrNtQueryInformationProcess);

	
	
	//get _PEB object
	(ptrNtQueryInformationProcess)(h, ProcessBasicInformation, &ProcessInformation, sizeof(ProcessInformation), &lenght);

	TCHAR Buffer[PATH];

	if (!GetCurrentDirectory(PATH, Buffer))
		return Error("Failed in GetCurrentDirectory\n");

	//replace commandline and imagepathname
	printf("[+] Get PebBaseAddress %p\n", ProcessInformation.PebBaseAddress);
	printf("[+] Get ProcessParameters %p\n", ProcessInformation.PebBaseAddress->ProcessParameters);
	printf("[+] Current CommandLine : %ws\n", ProcessInformation.PebBaseAddress->ProcessParameters->CommandLine.Buffer);
	printf("[+] Current ImagePathName : %ws\n", ProcessInformation.PebBaseAddress->ProcessParameters->ImagePathName.Buffer);
	printf("[+] Current Directory : %ws\n",Buffer);
	// change directory to the one Entered in the command line 
	printf("[+] Time For Masquerading :) ....\n");

	DWORD cddwRet;
	const wchar_t* path = L"C:\\Windows\\System32";
	if (!SetCurrentDirectory(path))
		return Error("Failed in SetCurrentDirectory!\n");

	if (!GetCurrentDirectory(PATH, Buffer))
		return Error("Failed in GetCurrentDirectory\n");

	printf("[+] Now Directory : %ws\n", Buffer);
	ProcessInformation.PebBaseAddress->ProcessParameters->CommandLine.Buffer = commandline;
	ProcessInformation.PebBaseAddress->ProcessParameters->ImagePathName.Buffer = commandline;
	printf("[+] Now CommandLine : %ws\n", ProcessInformation.PebBaseAddress->ProcessParameters->CommandLine.Buffer);
	printf("[+] Now ImagePathName : %ws\n", ProcessInformation.PebBaseAddress->ProcessParameters->ImagePathName.Buffer);





	printf("Press AnyThing To Quit the Process :\n");
	getchar();
	return 0;
}