#include <windows.h>
#include <stdio.h>
#include <windows.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
void QueryAllAddress();
void GetTextSegmentAddress();
void WriteData(const std::string& filename, MEMORY_BASIC_INFORMATION mbi);
void DeleteBinFilesInCurrentDirectory();
int main()
{
    DeleteBinFilesInCurrentDirectory();
    QueryAllAddress();
    return 0;
}

std::string DecimalToHex(int decimal)
{
    std::stringstream ss;
    ss << std::hex << std::uppercase << decimal;
    std::string hex = ss.str();
    return hex;
}

void WriteData(const std::string& filename, MEMORY_BASIC_INFORMATION mbi)
{
    FILE* fp = nullptr;
    fopen_s(&fp, filename.c_str(), "wb");
    if (fp != NULL)
    {
        fwrite(mbi.BaseAddress, mbi.RegionSize, 1, fp);
        fclose(fp);
        printf("Text segment saved to %s\n", filename.c_str());
    }
}

void QueryAllAddress()
{
    // 获取进程句柄
    HANDLE hProcess = GetCurrentProcess();
    //if (hProcess == INVALID_HANDLE_VALUE)
    //{
    //    DWORD dwError = GetLastError();
    //    LPSTR lpMsgBuf = nullptr;
    //    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER |
    //        FORMAT_MESSAGE_FROM_SYSTEM |
    //        FORMAT_MESSAGE_IGNORE_INSERTS,
    //        nullptr,
    //        dwError,
    //        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    //        (LPSTR)&lpMsgBuf,
    //        0,
    //        nullptr);
    //    std::cout << "GetCurrentProcess failed with error: " << dwError
    //        << ", " << lpMsgBuf << std::endl;
    //    LocalFree(lpMsgBuf);
    //}
    //HANDLE hProcess = GetModuleHandle(L"VirtualQueryDemo.exe");

    // 遍历进程的内存信息
    MEMORY_BASIC_INFORMATION mbi;
    LPVOID lpAddress = NULL;
    while (VirtualQueryEx(hProcess, lpAddress, &mbi, sizeof(mbi)) != 0)
    {
        // 判断内存信息的类型和保护属性
        if (mbi.State == MEM_COMMIT)
        {
            if (mbi.Protect & PAGE_EXECUTE_READ)
            {
                // .text段
                printf(".text Range: 0x%p -> 0x%p, Size: %d KB\n", mbi.BaseAddress, (LPBYTE)mbi.BaseAddress + mbi.RegionSize, mbi.RegionSize / 1024);
                WriteData(DecimalToHex((int)mbi.BaseAddress) + ".bin", mbi);
            }
            //else if (mbi.Protect & PAGE_READONLY)
            //{
            //    // .rodata段
            //    printf(".rodata Range: 0x%p -> 0x%p, Size: %d KB\n", mbi.BaseAddress, (LPBYTE)mbi.BaseAddress + mbi.RegionSize, mbi.RegionSize / 1024);
            //}
            //else if (mbi.Protect & PAGE_READWRITE)
            //{
            //    // .data段
            //    printf(".data Range: 0x%p -> 0x%p, Size: %d KB\n", mbi.BaseAddress, (LPBYTE)mbi.BaseAddress + mbi.RegionSize, mbi.RegionSize / 1024);
            //}
            //else if (mbi.Protect & PAGE_GUARD)
            //{
            //    // .bss段
            //    printf(".bss Range: 0x%p -> 0x%p, Size: %d KB\n", mbi.BaseAddress, (LPBYTE)mbi.BaseAddress + mbi.RegionSize, mbi.RegionSize / 1024);
            //}
            //else if (mbi.AllocationBase == mbi.BaseAddress)
            //{
            //    // 堆段
            //    printf("Heap Range: 0x%p -> 0x%p, Size: %d KB\n", mbi.BaseAddress, (LPBYTE)mbi.BaseAddress + mbi.RegionSize, mbi.RegionSize / 1024);
            //}
            //else if (mbi.Type == MEM_IMAGE)
            //{
            //    // 共享库空间
            //    printf("Image Range: 0x%p -> 0x%p, Size: %d KB\n", mbi.BaseAddress, (LPBYTE)mbi.BaseAddress + mbi.RegionSize, mbi.RegionSize / 1024);
            //}
            //else if (mbi.Protect & PAGE_EXECUTE_READWRITE)
            //{
            //    // .stack段
            //    printf(".stack Range: 0x%p -> 0x%p, Size: %d KB\n", mbi.BaseAddress, (LPBYTE)mbi.BaseAddress + mbi.RegionSize, mbi.RegionSize / 1024);
            //}
        }

        // 移动下一个内存块的起始地址
        lpAddress = (LPBYTE)mbi.BaseAddress + mbi.RegionSize;
    }
}

void GetTextSegmentAddress()
{
    // 获取当前模块的句柄
    HMODULE hModule = GetModuleHandle(NULL);

    // 获取当前模块的内存信息
    MEMORY_BASIC_INFORMATION mbi;
    VirtualQuery(hModule, &mbi, sizeof(mbi));

    // 查找.text段的内存信息
    while (mbi.BaseAddress < (LPBYTE)hModule + mbi.RegionSize)
    {
        if (mbi.Protect == PAGE_EXECUTE_READ || mbi.Protect == PAGE_EXECUTE_READWRITE)
        {
            if (mbi.Type == MEM_IMAGE && (DWORD_PTR)mbi.BaseAddress >= (DWORD_PTR)hModule)
            {
                PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
                PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)hModule + pDosHeader->e_lfanew);

                DWORD textVA = pNtHeaders->OptionalHeader.BaseOfCode + pNtHeaders->OptionalHeader.ImageBase;
                DWORD textEndVA = textVA + pNtHeaders->OptionalHeader.SizeOfCode;

                if ((DWORD_PTR)mbi.BaseAddress >= textVA && (DWORD_PTR)mbi.BaseAddress < textEndVA)
                {
                    printf(".text Range: 0x%p -> 0x%p, Size: %d KB\n", mbi.BaseAddress, (LPBYTE)mbi.BaseAddress + mbi.RegionSize, mbi.RegionSize / 1024);
                    return;
                }
            }
        }

        mbi.BaseAddress = (LPBYTE)mbi.BaseAddress + mbi.RegionSize;
        VirtualQuery(mbi.BaseAddress, &mbi, sizeof(mbi));
    }

    return;
}

void DeleteBinFilesInCurrentDirectory()
{
    WIN32_FIND_DATAA findData;
    HANDLE hFind = INVALID_HANDLE_VALUE;
    DWORD dwError = 0;
    std::string path = ".\\*.bin"; // 当前目录下的所有.bin文件

    hFind = FindFirstFileA(path.c_str(), &findData);

    if (hFind == INVALID_HANDLE_VALUE)
    {
        std::cout << "Failed to find files in current directory." << std::endl;
        return;
    }

    do
    {
        if (!(findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // 如果不是目录
        {
            std::string fileName(findData.cFileName);
            if (fileName.find(".bin") != std::string::npos) // 如果是.bin文件
            {
                std::string filePath = ".\\" + fileName;
                if (!DeleteFileA(filePath.c_str())) // 删除文件
                {
                    std::cout << "Failed to delete file: " << filePath << std::endl;
                }
            }
        }
    } while (FindNextFileA(hFind, &findData) != 0);

    dwError = GetLastError();
    if (dwError != ERROR_NO_MORE_FILES)
    {
        std::cout << "Failed to find more files." << std::endl;
    }

    FindClose(hFind);
}