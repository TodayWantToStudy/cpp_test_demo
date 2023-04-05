#include <stdio.h>
#include <Windows.h>

#define BUFFER_SIZE 10

int buffer[BUFFER_SIZE];  // 缓冲区
int in = 0, out = 0;  // 生产者和消费者的指针
HANDLE sem_empty, sem_full, mutex;  // 信号量和互斥量

DWORD WINAPI producer(LPVOID lpParam) {
    for (int i = 0; i < 20; i++) {
        int item = i + 1;
        WaitForSingleObject(sem_empty, INFINITE);  // 等待空缓冲区
        WaitForSingleObject(mutex, INFINITE);  // 获取互斥量
        buffer[in] = item;  // 生产者向缓冲区中添加数据
        in = (in + 1) % BUFFER_SIZE;  // 更新生产者指针
        ReleaseMutex(mutex);  // 释放互斥量
        ReleaseSemaphore(sem_full, 1, NULL);  // 发送满信号量
        printf("producer: item %d produced\n", item);
        Sleep(20);  // 生产者休眠1秒
    }
    return 0;
}

DWORD WINAPI consumer(LPVOID lpParam) {
    for (int i = 0; i < 20; i++) {
        WaitForSingleObject(sem_full, INFINITE);  // 等待满缓冲区
        WaitForSingleObject(mutex, INFINITE);  // 获取互斥量
        int item = buffer[out];  // 消费者从缓冲区中取出数据
        out = (out + 1) % BUFFER_SIZE;  // 更新消费者指针
        ReleaseMutex(mutex);  // 释放互斥量
        ReleaseSemaphore(sem_empty, 1, NULL);  // 发送空信号量
        printf("consumer: item %d consumed\n", item);
        Sleep(1500);  // 消费者休眠1.5秒
    }
    return 0;
}

int main() {
    sem_empty = CreateSemaphore(NULL, BUFFER_SIZE, BUFFER_SIZE, NULL);  // 创建空信号量，初始值为缓冲区大小
    sem_full = CreateSemaphore(NULL, 0, BUFFER_SIZE, NULL);  // 创建满信号量，初始值为0
    mutex = CreateMutex(NULL, FALSE, NULL);  // 创建互斥量

    HANDLE hProducer = CreateThread(NULL, 0, producer, NULL, 0, NULL);
    HANDLE hConsumer = CreateThread(NULL, 0, consumer, NULL, 0, NULL);

    WaitForSingleObject(hProducer, INFINITE);
    WaitForSingleObject(hConsumer, INFINITE);

    CloseHandle(sem_empty);
    CloseHandle(sem_full);
    CloseHandle(mutex);
    CloseHandle(hProducer);
    CloseHandle(hConsumer);

    return 0;
}