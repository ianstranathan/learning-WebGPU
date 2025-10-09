#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


#include <functional>

#include <chrono>
#include <thread>
#include <atomic>

#include <windows.h>


typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t  i8;
typedef int16_t i16;
typedef int32_t i32;

// ====================================================================================================

std::string load_shader_from_file(const char* filepath)
{
	// NOTE:
	// throws std::runtime_error if the file cannot be opened.

    std::ifstream file(filepath);
    
    if (!file.is_open())
	{
        std::cerr << "Error: Could not open shader file at " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer; //Read the entire file content into a stringstream
    buffer << file.rdbuf();

    file.close();

    return buffer.str();
}

// ====================================================================================================


void watchFile(const std::wstring& dirPath,
               const std::wstring& targetFile,
               std::atomic<bool>& should_recompile_shader_flag,
               std::atomic<bool>& stopFlag)
{
    HANDLE hDir = CreateFileW(dirPath.c_str(),
                              FILE_LIST_DIRECTORY,
                              FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr,
                              OPEN_EXISTING,
                              FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                              nullptr);

    if (hDir == INVALID_HANDLE_VALUE)
    {
        std::cerr << "Failed to open directory handle. Error: " << GetLastError() << std::endl;
        return;
    }

    char buffer[1024];
    DWORD bytesReturned = 0;
    OVERLAPPED overlapped = {};
    overlapped.hEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    if (!overlapped.hEvent)
    {
        std::cerr << "Failed to create event. Error: " << GetLastError() << std::endl;
        CloseHandle(hDir);
        return;
    }

    while (!stopFlag.load())
    {
        BOOL success = ReadDirectoryChangesW(
            hDir,
            buffer,
            sizeof(buffer),
            FALSE,
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
            &bytesReturned,
            &overlapped,
            nullptr);

        if (!success)
        {
            std::cerr << "ReadDirectoryChangesW failed: " << GetLastError() << std::endl;
            break;
        }

        // Wait for either the directory change or timeout (e.g., 500ms)
        DWORD waitStatus = WaitForSingleObject(overlapped.hEvent, 500);
        if (waitStatus == WAIT_OBJECT_0)
        {
            // Operation completed
            FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
            std::wstring changedFile(fni->FileName, fni->FileNameLength / sizeof(WCHAR));

            if (changedFile == targetFile)
            {
                should_recompile_shader_flag.store(true);
            }
        }
        else if (waitStatus == WAIT_TIMEOUT)
        {
            // Timeout: check stopFlag and continue
            continue;
        }
        else
        {
            std::cerr << "Wait error: " << GetLastError() << std::endl;
            break;
        }
    }

    CloseHandle(overlapped.hEvent);
    CloseHandle(hDir);
}

