// ReferenceDemo.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

void func(int& n) {
	std::cout << "lvalue=" << n << std::endl;
}
void func(int&& n) {
	std::cout << "rvalue=" << n << std::endl;
}
template<typename T>
void revoke(T&& t) {
	// 参数 T&& t 是一个左值，因为是有具名的
	//func(t); //无法进行完美转发，因为形参是左值
	func(std::forward<T>(t)); //可以进行完美转发
	std::remove_reference_t<T> b;
}

int main() {
	int i = 0;
	int& m = i;
	int&& n = 1;
	func(m); // lvalue
	func(n); // lvalue
	revoke(static_cast<int&>(m));
	revoke(static_cast<int&&>(n));
}

/*
// remove_reference_t<_Ty>&& _Arg 的作用是解除引用，确保参数类型是右值引用
template <class _Ty>
_NODISCARD constexpr _Ty&& forward(remove_reference_t<_Ty>&& _Arg) noexcept { // forward an rvalue as an rvalue
	static_assert(!is_lvalue_reference_v<_Ty>, "bad forward call");
	return static_cast<_Ty&&>(_Arg);
}

// 参数是左值引用
template <class _Ty>
_NODISCARD constexpr _Ty&& forward(remove_reference_t<_Ty>& _Arg) noexcept { // forward an lvalue as either an lvalue or an rvalue
	return static_cast<_Ty&&>(_Arg);
}
*/