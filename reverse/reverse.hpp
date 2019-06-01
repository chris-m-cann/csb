#ifndef CSB_REVERSE_HPP
#define CSB_REVERSE_HPP

#include <core/type_traits.hpp>

#include <cstring>
#include <type_traits>

namespace csb
{

    template <typename Iter,
              typename = std::enable_if_t<is_bidirectional_v<Iter>>>
    void reverse(Iter beg, Iter end)
    {
        while (beg != end && beg != --end)
        {
            std::swap(*beg, *end);
            ++beg;
        }
    }

    void reverse(char *str) { reverse(str, str + std::strlen(str)); }
} // namespace csb

#endif // CSB_REVERSE_HPP
