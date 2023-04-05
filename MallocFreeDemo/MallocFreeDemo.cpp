// MallocFreeDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

int main()
{
    std::cout << "Hello World!\n";

    int* p1 = new int{ 1 };
    int* p2 = (int*)malloc(sizeof(int));

    delete p1;
    free(p2);

    std::cout << "Bye World!\n";
}

