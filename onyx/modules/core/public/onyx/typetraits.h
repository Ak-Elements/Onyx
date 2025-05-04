#pragma once

namespace Onyx
{
	namespace Internal
    {
		template<typename>
		struct is_tuple_impl : std::false_type {};

		template<typename... Args>
		struct is_tuple_impl<std::tuple<Args...>> : std::true_type {};

		template <onyxU64...Idxs>
		constexpr auto substring_as_array(StringView str, std::index_sequence<Idxs...>)
		{
			return Array<char, sizeof...(Idxs) + 1>{ str[Idxs]..., '\0' };
		}

		template <typename T>
		constexpr auto type_name()
		{
#if ONYX_IS_CLANG
			constexpr auto prefix = std::string_view{ "=" };
			constexpr auto suffix = std::string_view{ "]" };
			constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif ONYX_IS_GCC
			constexpr auto prefix = std::string_view{ "=" };
			constexpr auto suffix = std::string_view{ "]" };
			constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif ONYX_IS_VISUAL_STUDIO
			constexpr auto classPrefix = std::string_view{ "class " };
			constexpr auto structPrefix = std::string_view{ "struct " };

			constexpr auto prefix = std::string_view{ "<" };
			constexpr auto suffix = std::string_view{ ">" };
			constexpr auto function = std::string_view{ __FUNCSIG__ };
#else
# error Unsupported compiler
#endif

			constexpr auto first = function.find_first_not_of(' ', function.find_first_of(prefix) + 1);
			constexpr auto value = function.substr(first, function.find_last_of(suffix) - first);
			return value;
		}

		template <typename T>
		struct type_name_holder
		{
			static inline constexpr auto value = type_name<T>();
		};
	} // namespace Impl

	template <typename T>
	constexpr StringView TypeName()
	{
		constexpr auto value = Internal::type_name_holder<T>::value;
		return { value.data(), value.size() };
	}

	template <typename T>
	constexpr onyxU32 TypeHash()
	{
		constexpr auto value = Internal::type_name_holder<T>::value;
		return Hash::FNV1aHash<onyxU32>(value);
	}

	template <typename T>
	constexpr onyxU64 TypeHash64()
	{
		constexpr auto value = Internal::type_name_holder<T>::value;
		return Hash::FNV1aHash<onyxU64>(value);
	}

	template<typename Type>
	struct is_tuple : Internal::is_tuple_impl<std::remove_cv_t<Type>> {};

	template<typename Type>
	inline constexpr bool is_tuple_v = is_tuple<Type>::value;

	// Helper to extract function argument types
	template <typename Ret, typename... Args>
	constexpr Tuple<Args...> GetFunctionArgumentTypes(Ret(*)(Args...)) { return {}; }

	// Overload for member functions
	template <typename Ret, typename ClassType, typename... Args>
	constexpr Tuple<Args...> GetFunctionArgumentTypes(Ret(ClassType::*)(Args...)) { return {}; }

	template <typename T>
	concept HasTypeId = requires(const T obj)
	{
		T::TypeId;
		{ obj.GetTypeId() };
	};


}