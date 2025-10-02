// using FileModifiedCallback = std::function<void(const std::wstring&)>;

// void watchFile(const std::wstring& dirPath,
//                const std::wstring& targetFile,
//                FileModifiedCallback onModified,
//                std::atomic<bool>& stopFlag)
// {
// 	std::wcout << L"[Watcher] Started thread to monitor: " << targetFile << std::endl;

//     HANDLE hDir = CreateFileW(dirPath.c_str(),
// 							  FILE_LIST_DIRECTORY,
// 							  FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
// 							  nullptr,
// 							  OPEN_EXISTING,
// 							  FILE_FLAG_BACKUP_SEMANTICS,
// 							  nullptr);

//     if (hDir == INVALID_HANDLE_VALUE)
// 	{
// 		std::cerr << "Failed to open directory handle. Error: " << GetLastError() << std::endl;
//         return;
//     }
	
//     char buffer[1024];
//     DWORD bytesReturned;

//     while (!stopFlag.load())
// 	{
//         if (ReadDirectoryChangesW(
//             hDir,
//             buffer,
//             sizeof(buffer),
//             FALSE,
//             FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
//             &bytesReturned,
//             nullptr,
//             nullptr
//         )) {
//             FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
//             std::wstring changedFile(fni->FileName, fni->FileNameLength / sizeof(WCHAR));

//             if (changedFile == targetFile) {
//                 onModified(changedFile);
//             }
//         }

//         Sleep(100); // Prevent CPU hogging
//     }

//     CloseHandle(hDir);
// }

// int main()
// {
//     std::wstring dirPath = L"c:\\Users\\istranathan\\_WORK\\cpp\\AHH\\rsc\\wgsl-shaders"; 
//     std::wstring fileName = L"first.wgsl";

//     std::atomic<bool> stopFlag(false);

//     // Define the callback
//     FileModifiedCallback callback = [](const std::wstring& file) {
//         std::wcout << L"[Callback] File modified: " << file << std::endl;
//     };

//     // Start watcher thread
//     std::thread watcherThread(watchFile, dirPath, fileName, callback, std::ref(stopFlag));

//     std::wcout << L"Watching for changes to " << fileName << L"...\n";
//     std::wcout << L"Press 'q' to quit.\n";

//     // // Fake event loop
//     // while (!stopFlag.load()) {
//     //     if (_kbhit()) {
//     //         int ch = _getch();
//     //         if (ch == 'q' || ch == 'Q') {
//     //             stopFlag = true;
//     //             break;
//     //         }
//     //     }
//     //     // Simulate other app logic here
//     //     Sleep(100); // Avoid busy loop
//     // }

//     // Clean up
//     watcherThread.join();
//     std::wcout << L"Stopped watching.\n";
//     return 0;
// }
