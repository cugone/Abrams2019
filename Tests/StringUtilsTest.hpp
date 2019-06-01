#pragma once

#include "pch.h"

#include "Engine/Core/StringUtils.hpp"

#include <string>
#include <vector>


TEST(StringUtils, ToUpperCase) {
    using namespace StringUtils;
    auto a = std::string{"abc"};
    auto b = ToUpperCase(a);
    auto result = std::string{"ABC"};
    EXPECT_EQ(b,result);
    a = std::string{"Abc"};
    b = ToUpperCase(a);
    EXPECT_EQ(b,result);
    a = std::string{"aBc"};
    b = ToUpperCase(a);
    EXPECT_EQ(b,result);
    a = std::string{"abC"};
    b = ToUpperCase(a);
    EXPECT_EQ(b,result);
    a = std::string{"ABC"};
    b = ToUpperCase(a);
    EXPECT_EQ(b,result);
}

TEST(StringUtils, ToLowerCase) {
    using namespace StringUtils;
    auto a = std::string{ "abc" };
    auto b = ToLowerCase(a);
    auto result = std::string{ "abc" };
    EXPECT_EQ(b, result);
    a = std::string{ "Abc" };
    b = ToLowerCase(a);
    EXPECT_EQ(b, result);
    a = std::string{ "aBc" };
    b = ToLowerCase(a);
    EXPECT_EQ(b, result);
    a = std::string{ "abC" };
    b = ToLowerCase(a);
    EXPECT_EQ(b, result);
    a = std::string{ "ABC" };
    b = ToLowerCase(a);
    EXPECT_EQ(b, result);
}

TEST(StringUtils, SplitSkipEmpty) {
    using namespace StringUtils;
    using StringList = std::vector<std::string>;
    auto a = StringList{ "abc" };
    auto b = Split("abc", ',', true);
    EXPECT_EQ(a,b);
    a = StringList{ "a", "b", "c" };
    b = Split("a,b,c", ',', true);
    EXPECT_EQ(a,b);
    a = StringList{ "abc" };
    b = Split(",abc", ',', true);
    EXPECT_EQ(a,b);
    a = StringList{ "a", "bc" };
    b = Split("a,bc", ',', true);
    EXPECT_EQ(a,b);
    a = StringList{ "ab", "c" };
    b = Split("ab,c", ',', true);
    EXPECT_EQ(a,b);
    a = StringList{ "abc" };
    b = Split("abc,", ',', true);
    EXPECT_EQ(a,b);
    a = StringList{ "ab", "c" };
    b = Split("ab,,c", ',', true);
    EXPECT_EQ(a,b);
    a = StringList{ "abc" };
    b = Split("abc", '.', true);
    EXPECT_EQ(a,b);
    a = StringList{ "a", "b", "c" };
    b = Split("a.b.c", '.', true);
    EXPECT_EQ(a,b);
    a = StringList{ "abc" };
    b = Split(".abc", '.', true);
    EXPECT_EQ(a,b);
    a = StringList{ "a", "bc" };
    b = Split("a.bc", '.', true);
    EXPECT_EQ(a,b);
    a = StringList{ "ab", "c" };
    b = Split("ab.c", '.', true);
    EXPECT_EQ(a,b);
    a = StringList{ "abc" };
    b = Split("abc.", '.', true);
    EXPECT_EQ(a,b);
    a = StringList{ "ab", "c" };
    b = Split("ab..c", '.', true);
    EXPECT_EQ(a,b);
}

TEST(StringUtils, SplitNoSkipEmpty) {
    using namespace StringUtils;
    using StringList = std::vector<std::string>;
    auto a = StringList{ "abc" };
    auto b = Split("abc", ',', false);
    EXPECT_EQ(a,b);
    a = StringList{ "a", "b", "c" };
    b = Split("a,b,c", ',', false);
    EXPECT_EQ(a,b);
    a = StringList{"", "abc" };
    b = Split(",abc", ',', false);
    EXPECT_EQ(a,b);
    a = StringList{ "a", "bc" };
    b = Split("a,bc", ',', false);
    EXPECT_EQ(a,b);
    a = StringList{ "ab", "c" };
    b = Split("ab,c", ',', false);
    EXPECT_EQ(a,b);
    a = StringList{ "abc","" };
    b = Split("abc,", ',', false);
    EXPECT_EQ(a,b);
    a = StringList{ "ab","", "c" };
    b = Split("ab,,c", ',', false);
    EXPECT_EQ(a,b);
    a = StringList{ "abc" };
    b = Split("abc", '.', false);
    EXPECT_EQ(a,b);
    a = StringList{ "a", "b", "c" };
    b = Split("a.b.c", '.', false);
    EXPECT_EQ(a,b);
    a = StringList{ "", "abc" };
    b = Split(".abc", '.', false);
    EXPECT_EQ(a,b);
    a = StringList{ "a", "bc" };
    b = Split("a.bc", '.', false);
    EXPECT_EQ(a,b);
    a = StringList{ "ab", "c" };
    b = Split("ab.c", '.', false);
    EXPECT_EQ(a,b);
    a = StringList{ "abc","" };
    b = Split("abc.", '.', false);
    EXPECT_EQ(a,b);
    a = StringList{ "ab","", "c" };
    b = Split("ab..c", '.', false);
    EXPECT_EQ(a,b);
}

TEST(StringUtils, SplitOnFirst) {
    using namespace StringUtils;
    auto [command,args] = SplitOnFirst("command arg1 arg2 arg3", ' ');
    EXPECT_EQ(command, std::string{ "command" });
    EXPECT_EQ(args, std::string{ "arg1 arg2 arg3" });
}


TEST(StringUtils, SplitOnLast) {
    using namespace StringUtils;
    auto [args,command] = SplitOnLast("arg1 arg2 arg3 command", ' ');
    EXPECT_EQ(command, std::string{ "command" });
    EXPECT_EQ(args, std::string{ "arg1 arg2 arg3" });
}

TEST(StringUtils, JoinNoDelimSkipEmpty) {
    using namespace StringUtils;
    using StringList = std::vector<std::string>;
    auto a = std::string{ "abc" };
    auto b = Join(StringList{"a","b","c"}, true);
    EXPECT_EQ(a, b);
    a = std::string{ "abcd" };
    b = Join(StringList{"a","b","c", "", "d"}, true);
    EXPECT_EQ(a, b);
}

TEST(StringUtils, JoinDelimSkipEmpty) {
    using namespace StringUtils;
    using StringList = std::vector<std::string>;
    auto a = std::string{ "a,b,c" };
    auto b = Join(StringList{"a","b","c"}, ',', true);
    EXPECT_EQ(a, b);
    a = std::string{ "a,b,c,d" };
    b = Join(StringList{"a","b","c", "", "d"}, ',', true);
    EXPECT_EQ(a, b);
}

TEST(StringUtils, JoinNoDelimNoSkipEmpty) {
    using namespace StringUtils;
    using StringList = std::vector<std::string>;
    auto a = std::string{ "abc" };
    auto b = Join(StringList{"a","b","c"}, false);
    EXPECT_EQ(a, b);
    a = std::string{ "abcd" };
    b = Join(StringList{"a","b","c", "", "d"}, false);
    EXPECT_EQ(a, b);
}

TEST(StringUtils, JoinDelimNoSkipEmpty) {
    using namespace StringUtils;
    using StringList = std::vector<std::string>;
    auto a = std::string{ "a,b,c" };
    auto b = Join(StringList{"a","b","c"}, ',', false);
    EXPECT_EQ(a, b);
    a = std::string{ "a,b,c,,d" };
    b = Join(StringList{"a","b","c", "", "d"}, ',', false);
    EXPECT_EQ(a, b);
}

TEST(StringUtils, SplitOnUnquotedSkipEmpty) {
    using namespace StringUtils;
    using StringList = std::vector<std::string>;
    std::string input =
        R"(
a=b

c=d
e="Hello
World"
)";
    auto a = StringList{"a=b","c=d","e=\"Hello\nWorld\""};
    auto b = SplitOnUnquoted(input, '\n', true);
    EXPECT_EQ(a, b);
}

TEST(StringUtils, SplitOnUnquotedNoSkipEmpty) {
    using namespace StringUtils;
    using StringList = std::vector<std::string>;
    std::string input =
        R"(
a=b

c=d
e="Hello
World"
)";
    auto a = StringList{"","a=b","","c=d","e=\"Hello\nWorld\"",""};
    auto b = SplitOnUnquoted(input, '\n', false);
    EXPECT_EQ(a, b);
}

//std::vector<std::string> SplitOnUnquoted(const std::string& string, char delim = ',', bool skip_empty = true);
//std::vector<std::wstring> SplitOnUnquoted(const std::wstring& string, wchar_t delim = ',', bool skip_empty = true);
