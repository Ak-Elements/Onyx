#pragma once

namespace onyx::ui {

enum class ScalarInputFlag : uint8_t {
    None,
    PowerOf2,
};

template < typename T > requires( std::is_arithmetic_v< T > || IsVector< T > )
struct ScalarOptions {
    Optional< T > Min;
    Optional< T > Max;

    bool IsSlider = false;
};
} // namespace onyx::ui
