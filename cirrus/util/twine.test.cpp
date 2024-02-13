#include "cirrus/util/twine.hpp"

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <vector>

using namespace cirrus;
using namespace cirrus::util;

TEST(Twine, empty) {
    Twine twine;
    EXPECT_EQ(twine.str(), "");
}

TEST(Twine, size) {
    {  // Empty
        Twine twine;
        EXPECT_EQ(twine.size(), 0);
    }

    {  // String
        Twine twine("hello");
        EXPECT_EQ(twine.size(), 5);
    }

    {  // Rope
        std::vector<std::string_view> segments{"hello", "world"};
        Twine                         twine(segments);
        EXPECT_EQ(twine.size(), 10);
    }
}

TEST(Twine, str) {
    {  // Empty
        Twine twine;
        EXPECT_EQ(twine.str(), "");
    }

    {  // String
        Twine twine("hello");
        EXPECT_EQ(twine.str(), "hello");
    }

    {  // Rope
        std::vector<std::string_view> segments{"hello", "world"};
        Twine                         twine(segments);
        EXPECT_EQ(twine.str(), "helloworld");
    }
}

TEST(Twine, concat) {
    {  // Empty + Empty
        Twine twine;
        twine.concat(Twine());
        EXPECT_EQ(twine.str(), "");
    }

    {  // Empty + String
        Twine twine;
        twine.concat("world");
        EXPECT_EQ(twine.str(), "world");
    }

    {  // String + Empty
        Twine twine("hello");
        twine.concat(Twine());
        EXPECT_EQ(twine.str(), "hello");
    }

    {  // String + String
        Twine twine("hello");
        twine.concat(" world");
        EXPECT_EQ(twine.str(), "hello world");
    }

    {  // Rope + Empty
        std::vector<std::string_view> segments{"hello", " world"};
        Twine                         twine(segments);
        twine.concat(Twine());
        EXPECT_EQ(twine.str(), "hello world");
    }

    {  // Rope + String
        std::vector<std::string_view> segments{"hello", " world"};
        Twine                         twine(segments);
        twine.concat("!");
        EXPECT_EQ(twine.str(), "hello world!");
    }

    {  // Rope + Rope
        std::vector<std::string_view> segments{"hello", " world"};
        Twine                         twine(segments);
        std::vector<std::string_view> more_segments{"!", "!"};
        twine.concat(more_segments);
        EXPECT_EQ(twine.str(), "hello world!!");
    }

    {  // String + String + String
        Twine twine("hello");
        twine.concat(" world");
        twine.concat("!");
        EXPECT_EQ(twine.str(), "hello world!");
    }

    {  // (String + String) + (String + String)
        Twine twine("hello");
        twine.concat(" world");
        Twine other("!");
        other.concat("!");
        twine.concat(other);
        EXPECT_EQ(twine.str(), "hello world!!");
    }
}

TEST(Twine, slice) {
    {  // Empty
        Twine twine;
        Twine slice = twine.slice(0, 0);
        EXPECT_EQ(slice.str(), "");
    }

    {  // String
        Twine twine("hello world");
        Twine slice = twine.slice(0, 5);
        EXPECT_EQ(slice.str(), "hello");
    }

    {  // Rope
        std::vector<std::string_view> segments{"hello", " world"};
        Twine                         twine(segments);
        Twine                         slice = twine.slice(0, 10);
        EXPECT_EQ(slice.str(), "hello world");
    }

    {  // Empty (out of bounds)
        Twine twine;
        Twine slice = twine.slice(0, 1);
        EXPECT_EQ(slice.str(), "");
    }

    {  // String (out of bounds)
        Twine twine("hello");
        Twine slice = twine.slice(0, 6);
        EXPECT_EQ(slice.str(), "hello");
    }

    {  // Rope (out of bounds)
        std::vector<std::string_view> segments{"hello", " world"};
        Twine                         twine(segments);
        Twine                         slice = twine.slice(0, 11);
        EXPECT_EQ(slice.str(), "hello world");
    }
}

TEST(Twine, find) {
    {  // String
        Twine           twine("hello world");
        Twine::Iterator it = twine.find('w', 0);
        EXPECT_EQ(it, 6);
    }

    {  // String
        Twine           twine("hello world");
        Twine::Iterator it = twine.rfind('l', 7);
        EXPECT_EQ(it, 3);
    }
}
