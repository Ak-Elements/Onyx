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

		template <bool KeepNamespace, typename T>
		constexpr auto type_name_array()
		{
#if ONYX_IS_CLANG
			constexpr auto prefix = std::string_view{ "T = " };
			constexpr auto suffix = std::string_view{ "]" };
			constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif ONYX_IS_GCC
			constexpr auto prefix = std::string_view{ "; T = " };
			constexpr auto suffix = std::string_view{ "]" };
			constexpr auto function = std::string_view{ __PRETTY_FUNCTION__ };
#elif ONYX_IS_VISUAL_STUDIO
			constexpr auto prefix = std::string_view{ "e," }; // tru'e', or fals'e',
			constexpr auto suffix = std::string_view{ ">(void)" };
			constexpr auto function = std::string_view{ __FUNCSIG__ };
#else
# error Unsupported compiler
#endif

			constexpr auto start = function.find(prefix) + prefix.size();
			constexpr auto end = function.rfind(suffix);

			static_assert(start < end);

			constexpr auto name = function.substr(start, (end - start));

			if constexpr (KeepNamespace)
			{
				return substring_as_array(name, std::make_index_sequence<name.size()>{});
			}
			else
			{
				constexpr auto new_start = name.find_last_of("::");
				if constexpr (new_start != std::string_view::npos)
				{
					static_assert((new_start + 1) < end);

					constexpr auto stripped_name = name.substr(new_start + 1, name.size() - (new_start + 1));

					return substring_as_array(stripped_name, std::make_index_sequence<stripped_name.size()>{});
				}
				else
				{
					return substring_as_array(name, std::make_index_sequence<name.size()>{});
				}
			}
		}

		template <typename T, bool keep_namespace>
		struct type_name_holder
		{
			static inline constexpr auto value = type_name_array<keep_namespace, T>();
		};
	} // namespace Impl

	template <typename T>
	constexpr StringView TypeName()
	{
		constexpr auto& value = Internal::type_name_holder<T, false>::value;
		return { value.data(), value.size() - 1 };
	}

	template <typename T>
	constexpr StringView FullyQualifiedTypeName()
	{
		constexpr auto& value = Internal::type_name_holder<T, true>::value;
		return { value.data(), value.size() - 1 };
	}

	template <typename T>
	constexpr onyxU32 TypeHash()
	{
		constexpr auto& value = Internal::type_name_holder<T, true>::value;
		return Hash::FNV1aHash32(value.data(), value.size() - 1);
	}

	template <typename T>
	constexpr onyxU64 TypeHash64()
	{
		constexpr auto& value = Internal::type_name_holder<T, true>::value;
		return Hash::FNV1aHash64(value.data(), value.size() - 1, 0);
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


}