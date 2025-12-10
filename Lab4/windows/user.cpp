#include <windows.h>
#include <iostream>
#include <string>
#include <vector>
#include <tlhelp32.h>

bool isProcessRunning(DWORD pid) {
    if (pid == 0) return false;

    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) {
        return false;
    }

    DWORD exitCode;
    BOOL result = GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);

    return (result && exitCode == STILL_ACTIVE);
}

bool isProcessRunningByName(const std::string& name) {
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        return false;
    }

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hSnap, &pe)) {
        CloseHandle(hSnap);
        return false;
    }

    bool found = false;
    do {
        if (_stricmp(pe.szExeFile, name.c_str()) == 0) {
            found = true;
            break;
        }
    } while (Process32Next(hSnap, &pe));

    CloseHandle(hSnap);
    return found;
}

DWORD startProcess(const std::string& path) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi = { 0 };

    std::string cmd = path;
    if (CreateProcess(NULL, &cmd[0], NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        CloseHandle(pi.hThread);
        return pi.dwProcessId;
    }

    return 0;
}

void printProcessStatus(const std::string& name, DWORD pid) {
    std::cout << "Process: " << name;
    if (pid != 0) {
        std::cout << " (PID: " << pid << ")";
    }

    bool runningByName = isProcessRunningByName(name);
    bool runningByPid = (pid != 0) ? isProcessRunning(pid) : false;

    if (runningByName || runningByPid) {
        std::cout << " - RUNNING" << std::endl;
    } else {
        std::cout << " - NOT RUNNING" << std::endl;
    }
}

int main() {
    std::cout << "=== USER APP: Testing Killer ===" << std::endl;

    std::string testName1 = "notepad.exe";
    std::string testName2 = "calc.exe";
    std::string testName3 = "mspaint.exe";

    std::cout << "\n1. Setting environment variable PROC_TO_KILL..." << std::endl;
    SetEnvironmentVariable("PROC_TO_KILL", (testName2 + "," + testName3).c_str());

    std::cout << "\n2. Starting test processes..." << std::endl;
    DWORD pidNotepad = startProcess("notepad.exe");
    DWORD pidCalc = startProcess("calc.exe");
    DWORD pidPaint = startProcess("mspaint.exe");

    Sleep(1000);

    std::cout << "\n3. Before Killer execution:" << std::endl;
    printProcessStatus(testName1, pidNotepad);
    printProcessStatus(testName2, pidCalc);
    printProcessStatus(testName3, pidPaint);

    std::cout << "\n4. Running Killer with --id parameter..." << std::endl;
    std::string cmdId = "killer.exe --id " + std::to_string(pidNotepad);
    system(cmdId.c_str());

    Sleep(500);

    std::cout << "\n5. After --id test:" << std::endl;
    printProcessStatus(testName1, pidNotepad);

    std::cout << "\n6. Starting notepad again for --name test..." << std::endl;
    pidNotepad = startProcess("notepad.exe");
    Sleep(500);

    std::cout << "\n7. Running Killer with --name parameter..." << std::endl;
    std::string cmdName = "killer.exe --name " + testName1;
    system(cmdName.c_str());

    Sleep(500);

    std::cout << "\n8. After --name test:" << std::endl;
    printProcessStatus(testName1, pidNotepad);

    std::cout << "\n9. Running Killer without parameters..." << std::endl;
    system("killer.exe");

    Sleep(500);

    std::cout << "\n10. Final status (should all be dead):" << std::endl;
    printProcessStatus(testName1, 0);
    printProcessStatus(testName2, 0);
    printProcessStatus(testName3, 0);

    std::cout << "\n11. Cleaning up environment variable..." << std::endl;
    SetEnvironmentVariable("PROC_TO_KILL", NULL);

    char* envCheck = std::getenv("PROC_TO_KILL");
    if (envCheck == NULL) {
        std::cout << "Environment variable successfully removed." << std::endl;
    } else {
        std::cout << "Warning: Environment variable still exists!" << std::endl;
    }

    std::cout << "\n=== TEST COMPLETED ===" << std::endl;

    return 0;
}