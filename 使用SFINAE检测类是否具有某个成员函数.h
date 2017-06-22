//ע�⣺Ҫ��ģ�������std::decay
template<typename T>
struct has_member_function {
private:
	using true_t = char;
	using false_t = char[2];

	template<typename T2, ReturnType/*����(�� ReturnType �滻Ϊ)д��Ҫ����Ƿ���ڵĺ����ķ���ֵ*/(T::*)(/*����д�ú����Ĳ�������*/)>
	struct func_matcher;

	template<typename T2>
	static true_t& test(func_matcher<T2, &T2::FunctionName>*/*����(�� FunctionName �滻Ϊ)д��Ҫ����Ƿ���ڵĺ���������*/);

	template<typename T2>
	static false_t& test(...);

public:
	enum
	{
		result = (sizeof(test<T>(NULL)) == sizeof(true_t))
	};
};