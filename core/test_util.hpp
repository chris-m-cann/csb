#pragma once

namespace csb::test
{

    class custom
    {
      public:
        custom(int i) : i(i) {}
        custom() : custom(0) {}

        friend bool operator==(custom const &l, custom const &r)
        {
            return l.i == r.i;
        }
        friend bool operator!=(custom const &l, custom const &r)
        {
            return !(l == r);
        }
        friend bool operator<(custom const &l, custom const &r)
        {
            return l.i < r.i;
        }
        friend bool operator>(custom const &l, custom const &r)
        {
            return r < l;
        }
        friend bool operator<=(custom const &l, custom const &r)
        {
            return !(r < l);
        }
        friend bool operator>=(custom const &l, custom const &r)
        {
            return !(l < r);
        }

        int i = 0;
    };

    struct move_only
    {
        int i;

        explicit move_only(int i) : i(i) {}
        move_only(move_only &&) = default;
        move_only(move_only const &) = delete;

        move_only &operator=(move_only &&) = default;
        move_only &operator=(move_only const &) = delete;

        friend bool operator==(move_only const &l, move_only const &r)
        {
            return l.i == r.i;
        }

        friend bool operator!=(move_only const &l, move_only const &r)
        {
            return !(l == r);
        }
    };
} // namespace csb::test