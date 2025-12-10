#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <csignal>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <cstring>
#include <fstream>

bool isProcessRunning(int pid) {
    if (pid <= 0) return false;
    
    if (kill(pid, 0) == 0) {
        return true;
    }
    
    return false;
}

bool isProcessRunningByName(const std::string& name) {
    DIR* dir = opendir("/proc");
    if (!dir) return false;
    
    dirent* entry;
    bool found = false;
    
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR) {
            char* endptr;
            int pid = strtol(entry->d_name, &endptr, 10);
            
            if (*endptr == '\0' && pid > 0) {
                std::string path = "/proc/" + std::string(entry->d_name) + "/comm";
                std::ifstream comm(path);
                
                if (comm) {
                    std::string procName;
                    std::getline(comm, name);
                    
                    if (procName == name) {
                        found = true;
                        break;
                    }
                }
            }
        }
    }
    
    closedir(dir);
    return found;
}

int startProcess(const std::string& path) {
    pid_t pid = fork();
    
    if (pid == 0) {
        execlp(path.c_str(), path.c_str(), nullptr);
        exit(1);
    } else if (pid > 0) {
        return pid;
    }
    
    return 0;
}

void printProcessStatus(const std::string& name, int pid) {
    std::cout << "Process: " << name;
    if (pid != 0) {
        std::cout << " (PID: " << pid << ")";
    }
    
    bool running = (pid != 0) ? isProcessRunning(pid) : isProcessRunningByName(name);
    
    if (running) {
        std::cout << " - RUNNING" << std::endl;
    } else {
        std::cout << " - NOT RUNNING" << std::endl;
    }
}

int main() {
    std::cout << "=== USER APP: Testing Killer (Linux) ===" << std::endl;
    
    std::string testName1 = "gedit";
    std::string testName2 = "xclock";
    std::string testName3 = "xeyes";
    
    std::cout << "\n1. Setting environment variable PROC_TO_KILL..." << std::endl;
    setenv("PROC_TO_KILL", (testName2 + "," + testName3).c_str(), 1);
    
    std::cout << "\n2. Starting test processes..." << std::endl;
    int pidGedit = startProcess("gedit");
    int pidClock = startProcess("xclock");
    int pidEyes = startProcess("xeyes");
    
    sleep(1);
    
    std::cout << "\n3. Before Killer execution:" << std::endl;
    printProcessStatus(testName1, pidGedit);
    printProcessStatus(testName2, pidClock);
    printProcessStatus(testName3, pidEyes);
    
    std::cout << "\n4. Running Killer with --id parameter..." << std::endl;
    std::string cmdId = "./killer --id " + std::to_string(pidGedit);
    system(cmdId.c_str());
    
    sleep(1);
    
    std::cout << "\n5. After --id test:" << std::endl;
    printProcessStatus(testName1, pidGedit);
    
    std::cout << "\n6. Starting gedit again for --name test..." << std::endl;
    pidGedit = startProcess("gedit");
    sleep(1);
    
    std::cout << "\n7. Running Killer with --name parameter..." << std::endl;
    std::string cmdName = "./killer --name " + testName1;
    system(cmdName.c_str());
    
    sleep(1);
    
    std::cout << "\n8. After --name test:" << std::endl;
    printProcessStatus(testName1, pidGedit);
    
    std::cout << "\n9. Running Killer without parameters..." << std::endl;
    system("./killer");
    
    sleep(1);
    
    std::cout << "\n10. Final status:" << std::endl;
    printProcessStatus(testName1, 0);
    printProcessStatus(testName2, 0);
    printProcessStatus(testName3, 0);
    
    std::cout << "\n11. Cleaning up..." << std::endl;
    unsetenv("PROC_TO_KILL");
    
    if (getenv("PROC_TO_KILL") == nullptr) {
        std::cout << "Environment variable successfully removed." << std::endl;
    }
    
    std::cout << "\n=== TEST COMPLETED ===" << std::endl;
    
    return 0;
}
