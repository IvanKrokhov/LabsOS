#include <iostream>
#include <windows.h>
#include <tlhelp32.h>
#include <iostream>
#include <sstream>
#include <vector>

void killById(DWORD pid)
{
    if (pid == 0 || pid == 4) {
        std::cout << "Cannot terminate system process (PID: " << pid << ")" << std::endl;
        return;
    }
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL)
    {
        std::cout << "Error opening process" << std::endl;
        return;
    }
    BOOL result = TerminateProcess(hProcess, 0);

    if (result) {
        std::cout << "Process terminated successfully" << std::endl;
    } else {
        std::cout << "Failed to terminate. Error: " << GetLastError() << std::endl;
    }

    CloseHandle(hProcess);
}

std::vector<DWORD> GetPids(const std::string& targetName)
{
    if (targetName.empty()) {
        std::cout << "Warning: Empty process name provided" << std::endl;
        return {};
    }
    std::vector<DWORD> pids;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        return {};
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    if (!Process32First(hSnap, &entry)) {
        std::cout << "Failed to get first process. Error: "
                  << GetLastError() << std::endl;
        CloseHandle(hSnap);
        return pids;
    }
    do {
        if (_stricmp(entry.szExeFile, targetName.c_str()) == 0) {
            pids.push_back(entry.th32ProcessID);
        }
    } while (Process32Next(hSnap, &entry));
    CloseHandle(hSnap);
    return pids;
}

void killByName(const std::string& targetName)
{
    auto pids = GetPids(targetName);
    if (pids.empty()) {
        std::cout << "No processes found with name: '" << targetName << "'" << std::endl;
        return;
    }
    for (auto& pid : pids)
    {
        killById(pid);
    }
}

void killByEnv()
{
    char* envVar = std::getenv("PROC_TO_KILL");
    if (!envVar) {
        return;
    }

    std::string envStr = envVar;
    std::istringstream iss(envStr);
    std::string processName;

    while (std::getline(iss, processName, ',')) {
        processName.erase(0, processName.find_first_not_of(" \t\n\r"));
        processName.erase(processName.find_last_not_of(" \t\n\r") + 1);

        if (!processName.empty()) {
            if (processName.find(".exe") == std::string::npos) {
                processName += ".exe";
            }
            killByName(processName);
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc == 1) {
        killByEnv();
        return 0;
    }
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--id" && i + 1 < argc) {
            try {
                DWORD pid = std::stoul(argv[i + 1]);
                killById(pid);
                i++;
            } catch (...) {
                std::cout << "Invalid PID: " << argv[i + 1] << std::endl;
                return 1;
            }
        }
        else if (arg == "--name" && i + 1 < argc) {
            std::string name = argv[i + 1];
            if (name.find(".exe") == std::string::npos) {
                name += ".exe";
            }
            killByName(name);
            i++;
        }
        else {
            std::cout << "Unknown argument: " << arg << std::endl;
        }
    }
    killByEnv();
    return 0;
}