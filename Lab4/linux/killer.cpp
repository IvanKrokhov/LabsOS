#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <csignal>
#include <dirent.h>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>

void killById(int pid) {
    if (pid <= 1) {
        std::cout << "Cannot terminate system process (PID: " << pid << ")" << std::endl;
        return;
    }

    if (kill(pid, SIGKILL) == 0) {
        std::cout << "Process " << pid << " terminated successfully" << std::endl;
    } else {
        std::cout << "Failed to terminate process " << pid
                  << ". Error: " << strerror(errno) << std::endl;
    }
}

std::vector<int> GetPids(const std::string& targetName) {
    std::vector<int> pids;
    DIR* dir = opendir("/proc");

    if (!dir) {
        return pids;
    }

    dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            char* endptr;
            int pid = std::strtol(entry->d_name, &endptr, 10);

            if (*endptr == '\0' && pid > 0) {
                std::string path = "/proc/" + std::string(entry->d_name) + "/comm";
                std::ifstream comm(path);

                if (comm) {
                    std::string name;
                    std::getline(comm, name);

                    if (name == targetName) {
                        pids.push_back(pid);
                    }
                }
            }
        }
    }

    closedir(dir);
    return pids;
}

void killByName(const std::string& targetName) {
    auto pids = GetPids(targetName);
    if (pids.empty()) {
        std::cout << "No processes found with name: '" << targetName << "'" << std::endl;
        return;
    }

    for (auto pid : pids) {
        killById(pid);
    }
}

void killByEnv() {
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
            killByName(processName);
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        killByEnv();
        return 0;
    }

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--id" && i + 1 < argc) {
            try {
                int pid = std::stoi(argv[i + 1]);
                killById(pid);
                i++;
            } catch (...) {
                std::cout << "Invalid PID: " << argv[i + 1] << std::endl;
                return 1;
            }
        }
        else if (arg == "--name" && i + 1 < argc) {
            std::string name = argv[i + 1];
            killByName(name);
            i++;
        }
        else if (arg == "--help") {
            std::cout << "Usage: ./killer [--id PID] [--name PROCESS_NAME]" << std::endl;
            return 0;
        }
        else {
            std::cout << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    killByEnv();
    return 0;
}