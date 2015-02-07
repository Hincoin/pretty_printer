// ContainerPrettyPrint.cpp : Defines the entry point for the console application.
//

#include <vector>
#include <type_traits>
#include <utility>
#include <unordered_set>
#include <iterator>
#include <unordered_map>
#include <functional>
#include <list>
#include <forward_list>
#include <string>
#include <deque>
#include <queue>
#include <map>
#include <iostream>
#include <tuple>
#include <set>



namespace Util
{
	template<class...>
	struct voider{ using type = void; };
	template<class ... args>
	using void_t = typename voider<args...>::type;
	struct IntConst{};
	struct FalseType : IntConst
	{
	public:
		using value = std::false_type;
	};
	struct TrueType : IntConst
	{
	public:
		using value = std::true_type;
	};

	template<bool... args>
	struct _Or;

	template<bool... others>
	struct _Or<true, others...> : public TrueType{};

	template<>
	struct _Or<false> : public FalseType{};




}
template<typename T>
class TD;

template<typename T>
struct is_container
{

private:
	template<typename U>

	static std::true_type test(typename U::const_iterator *){}
	template<typename U>

	static std::false_type test(...){}

	template<typename U>
	static U makeU();

	template<typename U, class = void>
	struct CheckBeginEnd : public Util::FalseType{};

	template<typename U>
	struct CheckBeginEnd<U, Util::void_t< decltype(makeU<U>())>> : public Util::TrueType{};



public:
	using value = decltype(test<T>(nullptr));
};

template<typename Char, typename Traits, typename Alloc>
struct is_container< std::basic_string<Char, Traits, Alloc> >
{
	using value = std::false_type;
};

template<typename T, std::size_t N>
struct is_container<T[N]> {
	using value = std::true_type;
};

template<typename ... Types>
struct is_container<std::tuple<Types...>>
{
	using value = std::true_type;
};

template<class First, class Last>
struct is_container<std::pair<First, Last>>
{
	using value = std::true_type;
};


//using namespace Util;

class default_formatting
{

protected:

	

	template<typename T, class = void>
	struct has_hash : public Util::FalseType{ };

	template<typename T>
	struct has_hash<T, Util::void_t<typename T::hasher>> : public Util::TrueType{ };

	template<typename T, class = void>
	struct has_key_compare : public Util::FalseType{};
	template<typename T>
	struct has_key_compare<T, Util::void_t<typename T::key_compare>> : public Util::TrueType{};

	template<typename T, class = void>
	struct has_const_iter : public Util::FalseType{};

	template<typename T, class = void>
	struct has_underlying_const_iter : public Util::FalseType{};

	template<typename T>
	struct has_underlying_const_iter<T, Util::void_t<typename T::container_type::const_iterator>> : public Util::TrueType{};

	template<typename T, class = void>
	struct has_map_types : public Util::FalseType{};

	template<typename T>
	struct has_map_types<T, Util::void_t<typename T::key_type, typename T::mapped_type>> : public Util::TrueType{};

	template<typename T>
	struct has_const_iter<T, Util::void_t<typename T::const_iterator>> : public Util::TrueType{};

	template<typename T, class = void>
	struct has_pair_types : public Util::FalseType{ using value = std::false_type; };

	template<typename T>
	struct has_pair_types<T, Util::void_t<typename T::first_type, typename T::second_type>> : public Util::TrueType{};

	template<typename A, typename T, typename U>
	struct _If { };

	template<typename T, typename U>
	struct _If<std::false_type, T, U> { using value = U; };

	template<typename T, typename U>
	struct _If<std::true_type, T, U> { using value = T; };

	struct ArbitraryType{ enum{ value = 1 }; };
	struct SetType{ enum{ value = 2 }; };
	struct TupleType{ enum{ value = 3 }; };
	struct PairType{ enum{ value = 4 }; };
	struct MapType { enum { value = 5 }; };

	template<typename T>
	struct TypeCategorizer
	{
		using category =
		typename _If<typename has_map_types<T>::value, MapType,
		typename _If<typename has_hash<T>::value, SetType,
		typename _If<typename has_key_compare<T>::value, SetType,
		typename _If<typename has_const_iter<T>::value, ArbitraryType,
		typename _If<typename has_underlying_const_iter<T>::value, ArbitraryType,
		typename _If<typename has_pair_types<T>::value, PairType, TupleType>::value>::value>::value>::value>::value>::value;
		//using category = typename _If<typename has_const_iter<T>::value, PairType, ArbitraryType>::value;

	};
	// default printing for arbitrary T.. now we specialize for std::
	virtual void prefix_impl(std::ostream& os, ArbitraryType)
	{
		os << "[";
	}

	virtual void seperator_impl(std::ostream& os, ArbitraryType)
	{
		os << ", ";
	}

	virtual void suffix_impl(std::ostream& os, ArbitraryType)
	{
		os << "]";
	}




	virtual void prefix_impl(std::ostream& os, PairType)
	{
		os << "(";
	}

	virtual void seperator_impl(std::ostream& os, PairType)
	{
		os << ", ";
	}

	virtual void suffix_impl(std::ostream& os, PairType)
	{
		os << ")";
	}



	virtual void prefix_impl(std::ostream& os, SetType)
	{
		os << "{";
	}

	virtual void seperator_impl(std::ostream& os, SetType)
	{
		os << ", ";
	}

	virtual void suffix_impl(std::ostream& os, SetType)
	{
		os << "}";
	}


	virtual void prefix_impl(std::ostream& os, TupleType)
	{
		os << "(";
	}

	virtual void seperator_impl(std::ostream& os, TupleType)
	{
		os << ", ";
	}

	virtual void suffix_impl(std::ostream& os, TupleType)
	{
		os << ")";
	}

	virtual void prefix_impl(std::ostream& os, MapType){
		os << "{\n\t";
	}

	virtual void seperator_impl(std::ostream& os, MapType){
		os << ",\n\t";
	}
	virtual void suffix_impl(std::ostream& os, MapType){
		os << "\n}";
	}

	template<class, class = void>
	struct has_hasher : std::false_type{};

	template<class T>
	struct has_hasher<T, Util::void_t<typename T::hasher>> : std::true_type{};
public:


	template<typename T>
	void prefix(std::ostream& os, const T& t)
	{
		prefix_impl(os, typename TypeCategorizer<T>::category());
	}
	template<typename T>
	void seperator(std::ostream& os, const T&)
	{
		seperator_impl(os, typename TypeCategorizer<T>::category());
	}

	template<typename T>
	void suffix(std::ostream& os, const T&)
	{
		suffix_impl(os, typename TypeCategorizer<T>::category());
	}

	template<typename T>
	void element(std::ostream& os, const T& t)
	{
		os << t;
	}

	template<typename Char, typename Traits, typename Alloc>
	void element(std::ostream& os, const std::basic_string<Char, Traits, Alloc>& str)
	{
		os << "\"" << str << "\"";
	}

	template<typename T>
	void print_cat(const T& t)
	{

		auto categ = typename TypeCategorizer<T>::category();
		auto categ2 = typename TypeCategorizer<std::decay_t<decltype(t)>>::category();
		//TD<T> td2;
		//TD<decltype(t)> td1;
		//TD<std::decay_t<decltype(t)>> tt;
		std::cout << "T cat = " << categ.value << "\n";
		std::cout << "decltype(t) cat = " << categ2.value << "\n";

	}
};


class special_formatter : public default_formatting
{


public:

	template<typename T>
	void prefix(std::ostream& os, const T& t)
	{
		//std::cout << typename TypeCategorizer<T>::category().value << "\n";
		prefix(os, typename TypeCategorizer<T>::category());
	}
	template<typename T>
	void seperator(std::ostream& os, const T&)
	{
		seperator(os, typename TypeCategorizer<T>::category());
	}

	template<typename T>
	void suffix(std::ostream& os, const T&)
	{
		suffix(os, typename TypeCategorizer<T>::category());
	}

	template<typename T>
	void element(std::ostream& os, const T& t)
	{
		os << t;
	}

	template<typename Char, typename Traits, typename Alloc>
	void element(std::ostream& os, const std::basic_string<Char, Traits, Alloc>& str)
	{
		os << "\"" << str << "\"";
	}

	template<typename T>
	void test_cat(const T& t)
	{
		std::cout << TypeCategorizer<T>::category().value << "\n";
		prefix(std::cout, t);
	}

};

class special2 : public default_formatting
{
protected:

	using default_formatting::prefix_impl;
	using default_formatting::seperator_impl;
	using default_formatting::suffix_impl;

	struct ForwardListType { enum{value=7}; };



	struct NetworkType{ enum{ value = 6 }; };

	template<typename T, class = void>
	struct has_network_type : public Util::FalseType{};

	template<typename T>
	struct has_network_type<T, Util::void_t<typename T::const_iterator>> : public Util::TrueType{};

	template<typename T, class=void>
	struct has_forward_iterator : public Util::FalseType{};
	
	template<typename T>
	struct has_forward_iterator<T, Util::void_t<typename T::const_iterator>>{
		using const_iterator = typename T::const_iterator;
		using value = typename std::is_same<typename std::iterator_traits<const_iterator>::iterator_category, std::forward_iterator_tag>::type;
	};

	void prefix_impl(std::ostream& os, ForwardListType)
	{
		os << "[[\t--> ";
	}
	void seperator_impl(std::ostream& os, ForwardListType)
	{
		os << " \n\t--> ";
	}
	void suffix_impl(std::ostream& os, ForwardListType)
	{
		os << "\n]]";
	}

	void prefix_impl(std::ostream& os, MapType)
	{
		os << "{{\n\t";
	}
	void suffix_impl(std::ostream& os, MapType)
	{
		os << "\n}}";
	}

	void prefix_impl(std::ostream& os, ArbitraryType)
	{
		os << "<";
	}
	void suffix_impl(std::ostream& os, ArbitraryType)
	{
		os << ">";
	}

	void prefix_impl(std::ostream& os, NetworkType)
	{
		os << "::";
	}
	void suffix_impl(std::ostream& os, NetworkType)
	{
		os << "!!";
	}
	void seperator_impl(std::ostream& os, NetworkType)
	{
		os << " / ";
	}

	template<typename T>
	struct TypeCategorizer2
	{
		
		using category = typename TypeCategorizer<T>::category;

	}; 

	// partially specialize TypeCategorizer for std::forward_list<>
	template<typename T,typename Alloc>
	struct TypeCategorizer2<std::forward_list<T, Alloc>>
	{
		using category = ForwardListType;
	};


public:


	template<typename T>
	void prefix(std::ostream& os, const T& t)
	{
		prefix_impl(os, typename TypeCategorizer2<T>::category());
	}

	template<typename T>
	void seperator(std::ostream& os, const T& t)
	{
		seperator_impl(os, typename TypeCategorizer2<T>::category());
	}

	template<typename T>
	void suffix(std::ostream& os, const T& t)
	{
		suffix_impl(os, typename TypeCategorizer2<T>::category());
	}



};
template<typename T, typename Fmt>
void print_entity(const T& t, std::ostream& os, std::false_type, Fmt& formatter)
{
	formatter.element(os, t);
}
template<typename First, typename Last, typename Fmt>
void print_entity(const std::pair<First, Last>& pair, std::ostream& os, std::true_type, Fmt& formatter)
{
	formatter.prefix(os, pair);
	print_entity(pair.first, os, typename is_container<First>::value(), formatter);
	formatter.seperator(os, pair);
	print_entity(pair.second, os, typename is_container<Last>::value(), formatter);
	formatter.suffix(os, pair);
}


template< typename Fmt, typename ... Args>
void print_tuple(const std::tuple<Args...>& tuple, std::ostream& os, std::integral_constant<std::size_t, 1>, Fmt& formatter)
{
	const std::size_t N = sizeof...(Args);
	print_entity(std::get<N - 1>(tuple), os, typename is_container<std::decay_t<decltype(std::get<N - 1>(tuple))>>::value(), formatter);
}

template<size_t index, typename Fmt, typename ... Args>
void print_tuple(const std::tuple<Args...>& tuple, std::ostream& os, std::integral_constant<std::size_t, index>, Fmt& formatter)
{
	const std::size_t N = sizeof...(Args);
	print_entity(std::get<N - index>(tuple), os, typename is_container<std::decay_t<decltype(std::get<N - index>(tuple))>>::value(), formatter);
	formatter.seperator(os, tuple);
	print_tuple(tuple, os, std::integral_constant<std::size_t, index - 1>(), formatter);
}


template<typename Fmt, typename ... Args>
void print_entity(const std::tuple<Args...>& tuple, std::ostream& os, std::true_type, Fmt& formatter)
{
	// call the tuple-print helper which performs the countdown
	formatter.prefix(os, tuple);
	print_tuple(tuple, os, std::integral_constant<std::size_t, sizeof...(Args)>(), formatter);
	formatter.suffix(os, tuple);
}
template<typename T, typename Fmt>
void print_entity(const T& t, std::ostream& os, std::true_type, Fmt& formatter)
{
	formatter.prefix(os, t);
	auto begin_iter = std::begin(t);
	auto end_iter = std::end(t);
	while (begin_iter != end_iter)
	{
		//auto tmp = *begin;
		//os << typename is_container<decltype(tmp)>::value() << "\n";
		print_entity(*begin_iter, os, typename is_container<std::decay_t<decltype(*begin_iter)>>::value(), formatter);
		if (++begin_iter != end_iter)
			formatter.seperator(os, t);
	}
	formatter.suffix(os, t);
}
template<typename T, typename Fmt>
void print_line(const T& t, std::ostream& os, Fmt& formatter)
{

	print_entity(t, os, typename is_container<T>::value(), formatter);
	os << std::endl;
}


template<typename T>
void print_line(const T& t, std::ostream& os)
{
	default_formatting df;
	print_line(t, os, df);
}


class network_scanner
{
public:
	std::string ip_addr;
	struct current_machine
	{
		std::string mac_addr;
	};

	using network_device = current_machine;

	friend std::ostream& operator << (std::ostream& os, const network_scanner& ns)
	{
		//		os << ns.ip_addr;
		return os;
	}

	
};

int main()
{
	default_formatting df;
	special_formatter sf;
	special2 sp2;
	std::vector<std::vector<int>> vvi;
	const std::vector<std::vector<int>>& sti = vvi;
	for (int i = 0; i < 10; ++i)
	{
		std::vector<int> vtor_sub{ i, i + 1, i + 2 };
		vvi.push_back(vtor_sub);
	}
	//	print_line(vvi, std::cout, df);

	std::tuple< int, char, std::vector<std::vector<double>>, double > wow_tuple{ 1, 'a', { { 4.0, 5.0 }, { 6.0, 7.0 } }, 5.6 };
	std::pair<std::vector<std::vector<int>>, std::vector<double>> pair_1{ { { 5, 6, 7 }, { 8, 9, 10 } }, { 5.6, 7.8, 9.45 } };
	std::pair<int, int> pint{ 5, 7 };
	std::unordered_map<int, std::string> my_map;
	my_map[1] = "Hello!";
	my_map[2] = "goodbye!";
	std::map<int, std::vector<int>> map_int_vec;
	map_int_vec[0] = { 4, 5, 6 };
	map_int_vec[1] = { 7, 8, 9 };

	std::cout << "map<int,vector<int>>: " << std::endl;
	print_line(map_int_vec, std::cout, sp2);
	std::cout << std::endl;

	//	std::cout << "unordered_map<int,string>: " << std::endl;
	//	print_line(my_map, std::cout, sp2);
	//std::cout << std::endl;

	std::cout << "vector<vector<int>>:" << std::endl;
	print_line(vvi, std::cout, sp2);
	std::cout << std::endl;

	//std::cout << "std::string: " << std::endl;
	//print_line(std::string("hi!"), std::cout, sp2);
	//std::cout << std::endl;

	/*std::cout << "std::tuple< int, char, std::vector<std::vector<double>>, double >: " << std::endl;
	print_line(wow_tuple, std::cout, sp2);
	std::cout << std::endl;

	std::cout << "std::pair<int, int>:\n";
	print_line(pint, std::cout, sp2);
	std::cout << "\n";

	std::cout << "std::pair<std::vector<std::vector<int>>, std::vector<double>>:\n";
	print_line(pair_1, std::cout, sp2);*/
	int a[5] = { 1, 2, 3, 4, 5 };
	print_line(a, std::cout, sp2);

	std::forward_list<int> rap = { 1, 2, 3, 4, 5 };
	print_line(rap, std::cout, sp2);


	network_scanner ns;

}