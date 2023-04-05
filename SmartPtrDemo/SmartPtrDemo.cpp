#include <iostream>
#include <memory>
class B;
class A {
public:
    //std::shared_ptr<B> b_ptr; // 循环引用
    std::weak_ptr<B> b_ptr; // 不增加引用计数，没有循环引用问题，实际使用时需要使用 lock() 转换为 shared_ptr
    A() { std::cout << "A constructed" << std::endl; }
    ~A() { std::cout << "A destructed" << std::endl; }
};
class B {
public:
    std::shared_ptr<A> a_ptr;
    B() { std::cout << "B constructed" << std::endl; }
    ~B() { std::cout << "B destructed" << std::endl; }
};
int main() {
    {
        std::shared_ptr<A> a_ptr(new A());
        std::shared_ptr<B> b_ptr(new B());
        a_ptr->b_ptr = b_ptr;
        b_ptr->a_ptr = a_ptr;
        // a_ptr 析构时 std::shared_ptr<A> 引用计数 2->1
        // b_ptr 析构时 std::shared_ptr<B> 引用计数 2->1
    }
    return 0;
}