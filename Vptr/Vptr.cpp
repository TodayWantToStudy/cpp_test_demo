// Vptr.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <Windows.h>
#include <iostream>
#include <functional>
class Base;
void print_vptr(Base* ptr_obj);

class Base
{
public:
	Base() {
		// 虚函数表在构造函数调用之前就创建好了
		//print_vptr(this);
		memset(this, NULL, sizeof(this));
	};
	Base(int i) :m_int(i) {};
	virtual void print_func1() {
		std::cout << __FUNCTION__ << std::endl;
	}
	virtual void print_func2() {
		std::cout << __FUNCTION__ << std::endl;
	}
private:
	int m_int = 0;
};

class Derive :public Base
{
public:
	Derive(int i) : m_int(i) {
		//print_vptr(this);
		//memset(this, NULL, sizeof(this));
	};
	virtual void print_func1() {
		std::cout << __FUNCTION__ << std::endl;
	}
private:
	int m_int = 0;
};

void print_vptr(Base* ptr_obj)
{
	std::cout << "------" << __FUNCTION__ << "------" << std::endl;
	std::cout << "input ptr=" << ptr_obj << std::endl;
#ifdef __WIN64
	typedef int64_t* Pointer;
#else
	typedef int32_t* Pointer;
#endif

	typedef void (*FunPtr)();
	Pointer ptr_table = (Pointer)ptr_obj;
	Pointer vfn_table = (Pointer)(*ptr_table);
	std::cout << "vfn_table(long):" << ptr_table << "->" << vfn_table << std::endl;

	FunPtr vfnptr1 = (FunPtr)vfn_table[0];
	std::cout << "vfnptr1:" << &vfn_table[0] << "->" << vfnptr1 << std::endl;
	vfnptr1();

	FunPtr vfnptr2 = (FunPtr)vfn_table[1];
	std::cout << "vfnptr2:" << &vfn_table[1] << "->" << vfnptr2 << std::endl;
	vfnptr2();

	std::cout << "------" << __FUNCTION__ << "------" << std::endl;
}

int main()
{
	std::cout << "Hello World!\n";
	Base base_obj;
	base_obj.print_func1();
	//Derive derive_obj = Derive(2);

	Base* ptr1 = new Base;
	Base* ptr2 = new Derive(1);

	//ptr1->print_func1();
	//ptr1->print_func2();
	ptr2->print_func1();
	ptr2->print_func2();

	std::cout << "start print_vptr call.\n";
	//print_vptr(&base_obj);
	//print_vptr(&derive_obj);

	std::cout << "End World!\n";
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
