
#pragma warning(disable : 4996)

#define WINVER 0x0A00
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <tchar.h>

using namespace std;

int main() 
{
	// пункт 1
	SECURITY_ATTRIBUTES attributes;
	attributes.nLength = sizeof(SECURITY_ATTRIBUTES);
	attributes.bInheritHandle = TRUE;
	attributes.lpSecurityDescriptor = NULL;

	HANDLE input_pipe_read_end;
	HANDLE input_pipe_write_end;
	CreatePipe(&input_pipe_read_end, &input_pipe_write_end, &attributes, 0);

	HANDLE output_pipe_read_end;
	HANDLE output_pipe_write_end;
	CreatePipe(&output_pipe_read_end, &output_pipe_write_end, &attributes, 0);

	SetHandleInformation(input_pipe_write_end, HANDLE_FLAG_INHERIT, 0);
	SetHandleInformation(output_pipe_read_end, HANDLE_FLAG_INHERIT, 0);
	//пункт 2
	STARTUPINFO startup_info;
	ZeroMemory(&startup_info, sizeof(startup_info));
	startup_info.cb = sizeof(startup_info);
	startup_info.hStdInput = input_pipe_read_end;
	startup_info.hStdOutput = output_pipe_write_end;
	startup_info.hStdError = output_pipe_write_end;
	startup_info.dwFlags |= STARTF_USESTDHANDLES;

	PROCESS_INFORMATION pi;
	CreateProcess(
		NULL,
		TEXT("cmd.exe"),
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&startup_info,
		&pi);
	printf("CreateProcess failed (%d).\n", GetLastError());
	// punkt 3
	DWORD bytes_read;
	bool flg=NULL;
	do {
		do {
			char buffer[64];
			ReadFile(output_pipe_read_end, buffer, sizeof(buffer), &bytes_read, NULL);
			fwrite(buffer, bytes_read, 1, stdout);
		} while (bytes_read != 0);
		//punkt 4
		const char PLEASE[] = "please";
		char* input = NULL;
		char buffer[256];
		while (!input) {
			fgets(buffer, sizeof(buffer), stdin);
			if (!strncmp(buffer, "thanks", 6)) {
				flg = 1;
				break;
			}
			else if (strncmp(buffer, "please", 6)) {
				fprintf(stderr, "Please ask politely!\n> ");
			}
			else {
				//buffer += '\n';
				input = buffer + sizeof(PLEASE);
				WriteFile(input_pipe_write_end, input, strlen(input), NULL, NULL);
				fprintf(stdout, input);
			}
		}
	} while (!flg);

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	CloseHandle(output_pipe_write_end);
	CloseHandle(input_pipe_read_end);
	return 0;

}
