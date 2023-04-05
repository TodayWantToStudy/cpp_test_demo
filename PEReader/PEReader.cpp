#include <stdio.h>
#include <Windows.h>
#include <Imagehlp.H>
#pragma comment(lib,"Imagehlp.lib")

HANDLE OpenPeByFileName(LPCTSTR FileName)
{
    LPCTSTR peFile = FileName;
    HANDLE hFile, hFileMapping, lpMapAddress = NULL;
    DWORD dwFileSize = 0;

    // 打开指定文件
    hFile = CreateFile(peFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    // 获取文件大小
    dwFileSize = GetFileSize(hFile, NULL);
    // 创建文件映射对象：只读
    hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, dwFileSize, NULL);
    // 将文件映射到当前进程的地址空间，并返回首地址
    lpMapAddress = MapViewOfFile(hFileMapping, FILE_MAP_READ, 0, 0, dwFileSize);
    if (lpMapAddress != NULL)
        return lpMapAddress;
}

// 读取 DOS 头
#include <winnt.h>
void DisplayDOSHeadInfo(HANDLE ImageBase)
{
    if (ImageBase == NULL)
    {
        printf("function failed, func=%s, line=%d", __FUNCTION__, __LINE__);
        return;
    }

    PIMAGE_DOS_HEADER pDosHead = NULL;
    pDosHead = (PIMAGE_DOS_HEADER)ImageBase;

    char szMZ[3] = { 0 };
    strncpy_s(szMZ, 3, reinterpret_cast<const char*>(&pDosHead->e_magic), 2);
    printf("DOS头：        %s\n", szMZ);
    printf("DOS头：        %x\n", pDosHead->e_magic);
    printf("文件地址：     %x\n", pDosHead->e_lfarlc);
    printf("PE结构偏移：   %x\n", pDosHead->e_lfanew);
}

/*
// NT头
typedef struct _IMAGE_NT_HEADERS {
    DWORD Signature; // PE标识："PE\0\0" = 0x00004550
    IMAGE_FILE_HEADER FileHeader; // PE文件头
    IMAGE_OPTIONAL_HEADER32 OptionalHeader; // PE可选头
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;
// PE文件头
typedef struct _IMAGE_FILE_HEADER {
    WORD    Machine;                  // 运行平台
    WORD    NumberOfSections;         // 文件的节数目
    DWORD   TimeDateStamp;            // 文件创建日期和时间
    DWORD   PointerToSymbolTable;     // 指向符号表(用于调试)
    DWORD   NumberOfSymbols;          // 符号表中的符号数量
    WORD    SizeOfOptionalHeader;     // IMAGE_OPTIONAL_HANDLER32结构的长度
    WORD    Characteristics;          // 文件的属性 exe=010fh dll=210eh
} IMAGE_FILE_HEADER, * PIMAGE_FILE_HEADER;
// PE可选头
typedef struct _IMAGE_OPTIONAL_HEADER {
    WORD    Magic;
    BYTE    MajorLinkerVersion;           // 连接器版本
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;                   // 所有包含代码节的总大小
    DWORD   SizeOfInitializedData;        // 所有已初始化数据的节总大小
    DWORD   SizeOfUninitializedData;      // 所有未初始化数据的节总大小
    DWORD   AddressOfEntryPoint;          // 程序执行入口RVA
    DWORD   BaseOfCode;                   // 代码节的起始RVA
    DWORD   BaseOfData;                   // 数据节的起始RVA
    DWORD   ImageBase;                    // 程序镜像基地址
    DWORD   SectionAlignment;             // 内存中节的对其粒度
    DWORD   FileAlignment;                // 文件中节的对其粒度
    WORD    MajorOperatingSystemVersion;  // 操作系统主版本号
    WORD    MinorOperatingSystemVersion;  // 操作系统副版本号
    WORD    MajorImageVersion;            // 可运行于操作系统的最小版本号
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;        // 可运行于操作系统的最小子版本号
    WORD    MinorSubsystemVersion;
    DWORD   Win32VersionValue;
    DWORD   SizeOfImage;                  // 内存中整个PE映像尺寸
    DWORD   SizeOfHeaders;                // 所有头加节表的大小
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    DWORD   SizeOfStackReserve;           // 初始化时堆栈大小
    DWORD   SizeOfStackCommit;
    DWORD   SizeOfHeapReserve;
    DWORD   SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;          // 数据目录的结构数量
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

*/

// 判断是否为 PE 文件
BOOL IsPEFile(HANDLE ImageBase)
{
    PIMAGE_DOS_HEADER pDosHead = NULL;
    PIMAGE_NT_HEADERS pNtHead = NULL;
    if (ImageBase == NULL) { return FALSE; }
    pDosHead = (PIMAGE_DOS_HEADER)ImageBase;
    if (IMAGE_DOS_SIGNATURE != pDosHead->e_magic) { return FALSE; }
    pNtHead = (PIMAGE_NT_HEADERS)((DWORD)pDosHead + pDosHead->e_lfanew);
    if (IMAGE_NT_SIGNATURE != pNtHead->Signature) { return FALSE; }
    return TRUE;
}

// 读取 NT 头
PIMAGE_NT_HEADERS GetNtHead(HANDLE ImageBase)
{
    PIMAGE_DOS_HEADER pDosHead = NULL;
    PIMAGE_NT_HEADERS pNtHead = NULL;
    pDosHead = (PIMAGE_DOS_HEADER)ImageBase;
    pNtHead = (PIMAGE_NT_HEADERS)((DWORD)pDosHead + pDosHead->e_lfanew);
    return pNtHead;
}

void DisplayFileHeaderInfo(HANDLE ImageBase)
{
    PIMAGE_NT_HEADERS pNtHead = NULL;
    PIMAGE_FILE_HEADER pFileHead = NULL;
    pNtHead = GetNtHead(ImageBase);
    pFileHead = &pNtHead->FileHeader;
    printf("运行平台:     %x\n", pFileHead->Machine);
    printf("节区数目:     %x\n", pFileHead->NumberOfSections);
    printf("时间标记:     %x\n", pFileHead->TimeDateStamp);
    printf("可选头大小    %x\n", pFileHead->SizeOfOptionalHeader);
    printf("文件特性:     %x\n", pFileHead->Characteristics);
}

void DisplayOptionalHeaderInfo(HANDLE ImageBase)
{
    PIMAGE_NT_HEADERS pNtHead = NULL;
    pNtHead = GetNtHead(ImageBase);
    printf("入口点：        %x\n", pNtHead->OptionalHeader.AddressOfEntryPoint);
    printf("镜像基址：      %x\n", pNtHead->OptionalHeader.ImageBase);
    printf("镜像大小：      %x\n", pNtHead->OptionalHeader.SizeOfImage);
    printf("代码基址：      %x\n", pNtHead->OptionalHeader.BaseOfCode);
    printf("区块对齐：      %x\n", pNtHead->OptionalHeader.SectionAlignment);
    printf("文件块对齐：    %x\n", pNtHead->OptionalHeader.FileAlignment);
    printf("子系统：        %x\n", pNtHead->OptionalHeader.Subsystem);
    printf("区段数目：      %x\n", pNtHead->FileHeader.NumberOfSections);
    printf("时间日期标志：  %x\n", pNtHead->FileHeader.TimeDateStamp);
    printf("首部大小：      %x\n", pNtHead->OptionalHeader.SizeOfHeaders);
    printf("特征值：        %x\n", pNtHead->FileHeader.Characteristics);
    printf("校验和：        %x\n", pNtHead->OptionalHeader.CheckSum);
    printf("可选头部大小：  %x\n", pNtHead->FileHeader.SizeOfOptionalHeader);
    printf("RVA 数及大小：  %x\n", pNtHead->OptionalHeader.NumberOfRvaAndSizes);
}

int main()
{
    HANDLE lpMapAddress = NULL;
    lpMapAddress = OpenPeByFileName(LR"(F:\c++\DLPDATA\截图分析\Snipaste-2.8.1-Beta-x86\Snipaste.exe)");
    DisplayDOSHeadInfo(lpMapAddress);
    DisplayFileHeaderInfo(lpMapAddress);
    DisplayOptionalHeaderInfo(lpMapAddress);
    return 0;
}