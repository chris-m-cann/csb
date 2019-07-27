#ifndef CSB_RED_BLACK_TREE_HPP
#define CSB_RED_BLACK_TREE_HPP

namespace csb
{
    template <typename T> class red_black_tree
    {
      public:
        enum class Colour : std::byte
        {
            Red,
            Black
        };

        struct node
        {
            T data;
            Colour colour;
            std::unique_ptr<node> left;
            std::unique_ptr<node> right;
        };

        class iterator
        {
          public:
          private:
            node *ref = nullptr;
        };

      private:
        std::unique_ptr<node> head;
    };
} // namespace csb

#endif // CSB_RED_BLACK_TREE_HPP
