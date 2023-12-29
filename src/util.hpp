// 11/15/2023
// Spencer Banasik

// util.hpp & cpp provide miscellaneous functions such as error handling
#ifndef UT_UTIL_HPP
#define UT_UTIL_HPP

#include <string>
#include <type_traits>
#include <stdexcept>

#define DEBUG_ERR_CHECKS

struct GLFWwindow;
namespace uteng_util{
extern std::string read_file(const std::string& file_path);
extern void shader_comp_err(const unsigned int shader_id);
extern void shader_link_err(const unsigned int shader_prog_id);

template <typename T>
T set_bit(T data, size_t position) {
#ifdef DEBUG_ERR_CHECKS
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type.");
    if (position > sizeof(T) * 8)
        throw std::out_of_range("Position is invalid.");
#endif
    return (data |= (1 << position));
}
template <typename T>
T clear_bit(T data, size_t position) {
#ifdef DEBUG_ERR_CHECKS
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type.");
    if (position > sizeof(T) * 8)
        throw std::out_of_range("Position is invalid.");
#endif
    return (data &= ~(1 << position));
}
template <typename T>
T toggle_bit(T data, size_t position) {
#ifdef DEBUG_ERR_CHECKS
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type.");
    if (position > sizeof(T) * 8)
        throw std::out_of_range("Position is invalid.");
#endif
    return (data ^= (1 << position));
}
template <typename T>
bool get_bit(T data, size_t position) {
#ifdef DEBUG_ERR_CHECKS
    static_assert(std::is_arithmetic<T>::value, "Not an arithmetic type.");
    if (position > sizeof(T) * 8)
        throw std::out_of_range("Position is invalid.");
#endif
    return ((data >> position) & 1);
}

}

#undef DEBUG_ERR_CHECKS

#endif