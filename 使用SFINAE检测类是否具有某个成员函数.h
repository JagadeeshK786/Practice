//注意：要把模板参数做std::decay
template<typename T>
struct has_member_function {
private:
	using true_t = char;
	using false_t = char[2];

	template<typename T2, ReturnType/*这里(将 ReturnType 替换为)写你要检测是否存在的函数的返回值*/(T::*)(/*这里写该函数的参数类型*/)>
	struct func_matcher;

	template<typename T2>
	static true_t& test(func_matcher<T2, &T2::FunctionName>*/*这里(将 FunctionName 替换为)写你要检测是否存在的函数的名字*/);

	template<typename T2>
	static false_t& test(...);

public:
	enum
	{
		result = (sizeof(test<T>(NULL)) == sizeof(true_t))
	};
};