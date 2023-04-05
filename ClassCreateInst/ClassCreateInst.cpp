// ClassCreateInst.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

class OnlyOnStack
{
public:
    OnlyOnStack() :m_int(0) {
        std::cout << __FUNCTION__ << std::endl;
    }
    OnlyOnStack(int i) :m_int(i) {
        std::cout << __FUNCTION__ << std::endl;
    }
    void print_data() {
        std::cout << __FUNCTION__ << ": m_int=" << m_int << std::endl;
    }
    static void* operator new(size_t size) = delete;
private:
    int m_int;
};

class OnlyOnHeap
{
public:
    static OnlyOnHeap* CreateInstanceOnHeap(int i)
    {
        OnlyOnHeap* ptr = new OnlyOnHeap(i);
        return ptr;
    }
    void print_data() {
        std::cout << __FUNCTION__ << ": m_int=" << m_int << std::endl;
    }
private:
    OnlyOnHeap() :m_int(0) {
        std::cout << __FUNCTION__ << std::endl;
    }
    OnlyOnHeap(int i) :m_int(i) {
        std::cout << __FUNCTION__ << std::endl;
    }
private:
    int m_int;
};

class MyNewOperInst
{
public:
    static MyNewOperInst* CreateInstanceOnHeap(int i)
    {
        // new 关键字先调用 operator new(size_t) 再调用对应的构造函数
        MyNewOperInst* ptr = new MyNewOperInst(i);
        return ptr;
    }
    void print_data() {
        std::cout << __FUNCTION__ << ": m_int=" << m_int << std::endl;
    }
    // 重载类成员 operater new
    static void* operator new(size_t size)
    {
        std::cout << __FUNCTION__ << ": size=" << size << std::endl;
        return malloc(size);
    }
private:
    MyNewOperInst() :m_int(0) {
        std::cout << __FUNCTION__ << std::endl;
    }
    MyNewOperInst(int i) :m_int(i) {
        std::cout << __FUNCTION__ << ": m_int=" << m_int << std::endl;
    }
private:
    int m_int;
};

// 重载全局 operator new
void* ::operator new(size_t size)
{
    std::cout << __FUNCTION__ << ": size=" << size << std::endl;
    return malloc(size);
}

int main()
{
    std::cout << "Hello World!\n";

    OnlyOnStack test1(1);
    test1.print_data();
    //OnlyOnStack* test11 = new OnlyOnStack; // 报错，无法访问 OnlyOnStack::operator new

    OnlyOnHeap* test2 = OnlyOnHeap::CreateInstanceOnHeap(2);
    test2->print_data();
    //OnlyOnHeap test22(2); // 报错，无法访问 OnlyOnHeap::OnlyOnHeap

    MyNewOperInst* test3 = MyNewOperInst::CreateInstanceOnHeap(3);
    test3->print_data();
    //MyNewOperInst test33(3); // 报错，无法访问构造函数


}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
