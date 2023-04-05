#include <Windows.h>
#include <stdio.h>

CRITICAL_SECTION cs;
CONDITION_VARIABLE cvRead, cvWrite;
int sharedData = 0;
int numReaders = 0;
BOOL isWriting = FALSE;

void StartRead() {
    EnterCriticalSection(&cs);
    while (isWriting) {
        SleepConditionVariableCS(&cvRead, &cs, INFINITE);
    }
    numReaders++;
    LeaveCriticalSection(&cs);
}

void EndRead() {
    EnterCriticalSection(&cs);
    numReaders--;
    if (numReaders == 0) {
        WakeConditionVariable(&cvWrite);
    }
    LeaveCriticalSection(&cs);
}

void StartWrite() {
    EnterCriticalSection(&cs);
    while (isWriting || numReaders > 0) {
        SleepConditionVariableCS(&cvWrite, &cs, INFINITE);
    }
    isWriting = TRUE;
    LeaveCriticalSection(&cs);
}

void EndWrite() {
    EnterCriticalSection(&cs);
    isWriting = FALSE;
    WakeAllConditionVariable(&cvRead);
    WakeConditionVariable(&cvWrite);
    LeaveCriticalSection(&cs);
}

DWORD WINAPI WriterThread(LPVOID lpParam) {
    for (int i = 0; i < 10; i++) {
        StartWrite();
        sharedData++;
        printf("Writer %d wrote sharedData: %d\n", GetCurrentThreadId(), sharedData);
        EndWrite();
        Sleep(1000);
    }
    return 0;
}

DWORD WINAPI ReaderThread(LPVOID lpParam) {
    for (int i = 0; i < 20; i++) {
        StartRead();
        printf("Reader %d read sharedData: %d\n", (INT)lpParam, sharedData);
        EndRead();
        Sleep(500);
    }
    return 0;
}

int main() {
    InitializeCriticalSection(&cs);
    InitializeConditionVariable(&cvRead);
    InitializeConditionVariable(&cvWrite);

    constexpr int THREAD_NUM = 5;
    HANDLE hThreads[THREAD_NUM];
    hThreads[0] = CreateThread(NULL, 0, WriterThread, NULL, 0, NULL);
    hThreads[1] = CreateThread(NULL, 0, ReaderThread, LPVOID(1), 0, NULL);
    hThreads[2] = CreateThread(NULL, 0, ReaderThread, LPVOID(2), 0, NULL);
    hThreads[3] = CreateThread(NULL, 0, ReaderThread, LPVOID(3), 0, NULL);
    hThreads[4] = CreateThread(NULL, 0, ReaderThread, LPVOID(4), 0, NULL);

    WaitForMultipleObjects(THREAD_NUM, hThreads, TRUE, INFINITE);

    DeleteCriticalSection(&cs);
    return 0;
}