// 11/15/2023
// Spencer Banasik

// util.hpp & cpp provide miscellaneous functions such as error handling
#ifndef UT_UTIL_HPP
#define UT_UTIL_HPP

// TODO: I'd like to avoid including string here, but I don't think I can avoid it
#include <string>
#include <type_traits>
#include <stdexcept>

struct GLFWwindow;
namespace uteng_util{
    extern void framebuffer_resize_callback(GLFWwindow*, int, int);
    extern std::string read_file(const std::string& file_path);
    extern void shader_comp_err(const unsigned int shader_id);
    extern void shader_link_err(const unsigned int shader_prog_id);

    // -- Bitmap Template Class --
    // Wraps a numeric data type with operations that make it easy to treat
    // the data as a bitmap. Positions do not need to be bit shifted when inputted
    // Positions are little endian. (0 is on the right)
    // https://stackoverflow.com/questions/8130602/using-extern-template-c11-to-avoid-instantiation
    template <class T>
    class Bitmap {
    public:
        Bitmap() {
            internal_data = 0;
        }
        Bitmap(const T) {
            internal_data = T;
        }
        // Return objects number of bits
        size_t size(void) const {
            return sizeof(T) * 8;
        }
        // Returns data of object
        T data(void) const {
            return internal_data;
        }

        void clear() {
            internal_data = 0;
        }

        void set_bit(const size_t position) {
            if (position > sizeof(T) * 8)
                throw std::out_of_range("Invalid Bitmask Position");
            internal_data |= (1 << position);
        }
        void clear_bit(const size_t position) {
            if (position > sizeof(T) * 8)
                throw std::out_of_range("Invalid Bitmask Position");
            internal_data &= ~(1 << position);
        }
        void toggle_bit(const size_t position) {
            if (position > sizeof(T) * 8)
                throw std::out_of_range("Invalid Bitmask Position");
            internal_data ^= (1 << position);
        }
        unsigned char get_bit(const size_t position) const {
            if (position > sizeof(T) * 8)
                throw std::out_of_range("Invalid Bitmask Position");
            return (internal_data >> position) & 1;
        }
    private:
        T internal_data;
    };
}

#endif