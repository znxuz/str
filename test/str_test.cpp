#include <fmt/core.h>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <iterator>
#include <random>
#include <string>
#include <vector>

#include "../src/str.hpp"

using std::begin, std::end;

class str_test : public ::testing::Test
{
protected:
    const char* line = "Lorem ipsum dolor sit amet, consectetur:"
                       "adipiscing elit. Quantum Aristoxeni"
                       "ingenium consumptum videmus in musicis?"
                       "Quae similitudo in genere etiam humano"
                       "apparet. Ego vero volo in virtute vim"
                       "esse quam maximam; Egone quaeris,"
                       "inquit, quid sentiam? Primum quid"
                       "tu dicis breve? Facile"
                       "pateremur, qui etiam nunc"
                       "agendi aliquid discendique"
                       "causa prope contra naturam"
                       "vígillas suscipere soleamus.";
    str s;
    std::string stls;
    std::mt19937 mt{};
    std::uniform_int_distribution<char> rand_char{'!', '~'};

    bool is_zero_or_same_sign(int a, int b)
    {
        return (!a && !b) || (a > 0 && b > 0) || (a < 0 && b < 0);
    }

    void SetUp() override
    {
        s = line;
        stls = line;
        mt.seed(std::random_device{}());

        ASSERT_STREQ(s.c_str(), line);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }

    str gen_str(char from, char to, size_t size)
    {
        auto ret = str();
        ret.resize(size);
        auto rand_c = std::uniform_int_distribution<char>(from, to);
        for (size_t i = 0; i < ret.size(); ++i)
            ret[i] = rand_c(mt);

        return ret;
    }
};

TEST_F(str_test, sbo_to_dyn)
{
    return;
    auto s1 = str();
    for (auto i = 0; i < 15; ++i) {
        s1.push_back('c');
        fmt::println("s1.size: {}", s1.size());
        fmt::println("s1.capacity: {}", s1.capacity());
    }
}

TEST_F(str_test, default_ctor)
{
    auto s = str();
    auto stls = std::string();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
    ASSERT_EQ(s.front(), 0);
    ASSERT_EQ(s.back(), 0);
    ASSERT_EQ(s.front(), stls.front());
    ASSERT_EQ(s.back(), stls.back());
}

TEST_F(str_test, char_ptr_ctor)
{
    auto s = str(line);
    auto stls = std::string(line);
    ASSERT_STREQ(s.c_str(), line);
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_NE(reinterpret_cast<std::uintptr_t>(s.c_str()),
            reinterpret_cast<std::uintptr_t>(line));
    ASSERT_EQ(s.size(), std::strlen(line));
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, copy_ctor)
{
    auto s2 = str(line);
    auto s3 = s2;

    ASSERT_STREQ(s3.c_str(), s2.c_str());
    ASSERT_EQ(s3.size(), s2.size());
    ASSERT_NE(s3.c_str(), s2.c_str());
}

TEST_F(str_test, move_ctor)
{
    auto s1 = str();
    auto s2 = str(line);
    auto addr = reinterpret_cast<intptr_t>(s2.c_str());

    s1 = std::move(s2);

    ASSERT_EQ(addr, reinterpret_cast<intptr_t>(s1.c_str()));
    ASSERT_STREQ(s1.c_str(), line);
    ASSERT_EQ(s1.size(), std::strlen(line));
}

TEST_F(str_test, move_ctor_rand)
{
    auto generate_random_string = [](size_t size) {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = std::string();
        string.reserve(size);
        for (size_t i = 0; i < size; ++i)
            string.push_back(rand_c(mt));

        return string;
    };
    auto rand_size = std::uniform_int_distribution<size_t>(0, 50);

    for (auto i = 0; i < 1000; ++i) {
        auto s1 = str();
        auto size = rand_size(mt);
        auto rand_stls = generate_random_string(size);
        auto rand_str = str(rand_stls.c_str());
        ASSERT_STREQ(rand_str.c_str(), rand_stls.c_str());
        ASSERT_EQ(rand_str.size(), rand_stls.size());

        s1 = str(std::move(rand_str));

        ASSERT_STREQ(s1.c_str(), rand_stls.c_str());
        ASSERT_EQ(s1.size(), rand_stls.size());
    }
}

TEST_F(str_test, moved_reuse)
{
    auto generate_random_string = [](size_t size) {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = str();
        string.resize(size);
        for (size_t i = 0; i < size; ++i)
            string.push_back(rand_c(mt));

        return string;
    };

    auto vec = std::vector<str>();

    auto rand_size = std::uniform_int_distribution<size_t>(0, 50);
    auto i = 0;
    for (auto line = generate_random_string(rand_size(mt)); i++ < 1000;
            line = str(generate_random_string(rand_size(mt)))) {
        vec.push_back(std::move(line));
    }
}

TEST_F(str_test, iter_ctor1)
{
    auto s1 = str(begin(s), end(s));

    ASSERT_STREQ(s.c_str(), s1.c_str());
    ASSERT_EQ(s.size(), s1.size());
}

TEST_F(str_test, iter_ctor2)
{
    auto rand_first = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size() - 1));

    for (size_t i = 0; i < 10000; ++i) {
        auto first_idx = rand_first(mt);
        auto rand_last = std::uniform_int_distribution<ptrdiff_t>(first_idx,
                static_cast<ptrdiff_t>(s.size() - 1));
        auto last_idx = rand_last(mt);

        auto s1 = str(begin(s) + first_idx, begin(s) + last_idx);
        auto stls1 = std::string(begin(stls) + first_idx, begin(stls) + last_idx);

        ASSERT_STREQ(s1.c_str(), stls1.c_str());
        ASSERT_EQ(s1.size(), stls1.size());
    }
}

TEST_F(str_test, copy_swap_both_sbo)
{
    const auto* small_str = "hello, world!";
    auto small_str_size = std::strlen(small_str);

    auto s1 = str(small_str);
    ASSERT_STREQ(s1.c_str(), small_str);
    ASSERT_EQ(s1.size(), small_str_size);

    auto s2 = str();
    s2 = s1;
    ASSERT_STREQ(s2.c_str(), small_str);
    ASSERT_EQ(s2.size(), small_str_size);
}

TEST_F(str_test, copy_swap_left_sbo)
{
    auto sbo = str();
    auto dyn = str(line);

    sbo = dyn;
    ASSERT_STREQ(sbo.c_str(), dyn.c_str());
    ASSERT_EQ(sbo.size(), dyn.size());
}

TEST_F(str_test, copy_swap_right_sbo)
{
    auto dyn = str(line);
    const auto* small_str = "aaaaa";
    auto len = std::strlen(small_str);

    dyn = small_str;
    ASSERT_STREQ(dyn.c_str(), small_str);
    ASSERT_EQ(dyn.size(), len);
}

TEST_F(str_test, copy_swap_both_dyn)
{
    auto dyn1 = str(line);
    auto dyn2 = str("need to get beyond 15 character");

    dyn1 = dyn2;
    ASSERT_STREQ(dyn1.c_str(), dyn2.c_str());
    ASSERT_EQ(dyn1.size(), dyn2.size());
}

TEST_F(str_test, copy_swap2)
{
    auto tmp = str(line);
    s = tmp;

    ASSERT_STREQ(s.c_str(), tmp.c_str());
    ASSERT_NE(s.c_str(), tmp.c_str());
    ASSERT_EQ(s.size(), tmp.size());

    s = str();
    ASSERT_EQ(s.size(), 0);
    ASSERT_EQ(s.capacity(), 15);
    ASSERT_STREQ(s.c_str(), "");
}

TEST_F(str_test, copy_swap3)
{
    auto tmp = str(line);
    s = std::move(tmp);

    ASSERT_STREQ(s.c_str(), line);
    ASSERT_EQ(s.size(), std::strlen(line));
}

TEST_F(str_test, copy_swap_random)
{
    auto generate_random_string = [](size_t size) {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = std::string();
        string.reserve(size);
        for (size_t i = 0; i < size; ++i)
            string.push_back(rand_c(mt));

        return string;
    };

    auto rand_size = std::uniform_int_distribution<size_t>(0, 50);

    for (auto i = 0; i < 1000; ++i) {
        auto s1 = str();
        auto size = rand_size(mt);
        auto rand_stls = generate_random_string(size);
        auto rand_str = str(rand_stls.c_str());
        ASSERT_STREQ(rand_str.c_str(), rand_stls.c_str());
        ASSERT_EQ(rand_str.size(), rand_stls.size());

        s1 = str(std::move(rand_str));

        ASSERT_STREQ(s1.c_str(), rand_stls.c_str());
        ASSERT_EQ(s1.size(), rand_stls.size());
    }
}

TEST_F(str_test, Front)
{
    s = str(line);
    ASSERT_EQ(s.front(), line[0]);
}

TEST_F(str_test, back_and_push_back)
{
    s = str();
    stls = std::string();
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        s.push_back(line[i]);
        stls.push_back(line[i]);

        ASSERT_EQ(s.back(), line[i]);
        ASSERT_EQ(s.size(), stls.size());
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }
}

TEST_F(str_test, pop_back)
{
    auto generate_random_string = [](size_t size) {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = std::string();
        string.reserve(size);
        for (size_t i = 0; i < size; ++i)
            string.push_back(rand_c(mt));

        return string;
    };

    auto rand_size = std::uniform_int_distribution<size_t>(0, 50);
    for (auto i = 0; i < 100; ++i) {
        stls = generate_random_string(rand_size(mt));
        auto s1 = str(stls.c_str());

        while (!s1.empty()) {
            stls.pop_back();
            s1.pop_back();
            ASSERT_STREQ(s1.c_str(), stls.c_str());
            ASSERT_EQ(s1.size(), stls.size());
        }
        ASSERT_STREQ(s1.c_str(), stls.c_str());
        ASSERT_EQ(s1.size(), stls.size());
    }
}

TEST_F(str_test, append1)
{
    auto len = std::strlen(line);
    auto rand_idx = std::uniform_int_distribution<size_t>(0, len - 1);
    auto rand_cnt = std::uniform_int_distribution<size_t>(0, 100);
    for (size_t i = 0; i < 100; ++i) {
        auto idx = rand_idx(mt);
        auto cnt = rand_cnt(mt);
        s.append(cnt, line[idx]);
        stls.append(cnt, line[idx]);

        ASSERT_EQ(s.size(), stls.size());
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }
}

TEST_F(str_test, append2)
{
    s.append(str());
    stls.append(std::string());
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());

    for (size_t i = 0; i < 5; ++i) {
        s.append(line);
        stls.append(line);

        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }

    s.clear();
    stls.clear();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, append3)
{
    auto rand = std::uniform_int_distribution<size_t>(0, 100);
    for (size_t i = 0; i < 100; ++i) {
        auto cnt = rand(mt);
        s.append(line, cnt);
        stls.append(line, cnt);

        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, append_iter1)
{
    auto buf = str();
    buf.append(begin(s), end(s));

    ASSERT_STREQ(buf.c_str(), s.c_str());
}

TEST_F(str_test, append_iter2)
{
    std::copy(begin(s), end(s), begin(stls));

    ASSERT_EQ(stls.size(), s.size());

    auto buf = str();
    buf.append(begin(stls), end(stls));
    ASSERT_STREQ(buf.c_str(), s.c_str());
    ASSERT_EQ(buf.size(), s.size());
}

TEST_F(str_test, append_iter3)
{
    auto buf = std::string();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    buf.append(begin(s), end(s));

    ASSERT_STREQ(buf.c_str(), stls.c_str());
    ASSERT_EQ(buf.size(), stls.size());
}

TEST_F(str_test, insert1)
{
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        s.insert(i, 1, line[i]);
        stls.insert(i, 1, line[i]);
    }

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());

    s.clear();
    stls.clear();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, insert2)
{
    const auto line = std::string{"Lorem ipsum dolor sit amet, consectetur:"
                                  "adipiscing elit. Quantum Aristoxeni"
                                  "ingenium consumptum videmus in musicis?"
                                  "Quae similitudo in genere etiam humano"
                                  "apparet. Ego vero volo in virtute vim"
                                  "esse quam maximam; Egone quaeris,"
                                  "inquit, quid sentiam? Primum quid"
                                  "tu dicis breve? Facile"
                                  "pateremur, qui etiam nunc"
                                  "agendi aliquid discendique"
                                  "causa prope contra naturam"
                                  "vígillas suscipere soleamus."};

    auto line_idx_range = std::uniform_int_distribution<size_t>(0, line.size() - 1);
    auto count_range = std::uniform_int_distribution<size_t>(0, 20);

    for (size_t i = 0; i < line.size(); ++i) {
        auto insert_idx_range = std::uniform_int_distribution<size_t>(0, i);
        auto insert_idx = insert_idx_range(mt);
        auto line_idx = line_idx_range(mt);
        auto cnt = count_range(mt);

        s.insert(insert_idx, cnt, line[line_idx]);
        stls.insert(insert_idx, cnt, line[line_idx]);
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }

    s.clear();
    stls.clear();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, insert_str)
{
    const auto line = str{ "Lorem ipsum dolor sit amet, consectetur:" };

    for (size_t i = 0; i < 50; ++i) {
        auto insert_idx_range = std::uniform_int_distribution<size_t>(0, i);
        auto insert_idx = insert_idx_range(mt);

        s.insert(insert_idx, line);
        stls.insert(insert_idx, line.c_str());

        ASSERT_STREQ(s.c_str(), stls.c_str());
    }

    s.clear();
    stls.clear();
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, resize1)
{
    auto rand = std::uniform_int_distribution<size_t>(0, 100);
    for (size_t i = 0; i < 50; ++i) {
        auto size = rand(mt);
        s.resize(size);
        stls.resize(size);
        ASSERT_EQ(s.size(), stls.size());
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }
}

TEST_F(str_test, resize2)
{
    auto rand = std::uniform_int_distribution<size_t>(0, 100);
    auto rand_alpha = std::uniform_int_distribution<char>('a', 'z');
    for (size_t i = 0; i < 50; ++i) {
        auto size = rand(mt);
        auto c = rand_alpha(mt);
        s.resize(size, c);
        stls.resize(size, c);
        ASSERT_EQ(s.size(), stls.size());
        ASSERT_STREQ(s.c_str(), stls.c_str());
    }
}

TEST_F(str_test, remove_newline)
{
    auto orig_size = s.size();
    s.push_back('\n');
    s.remove_newline();

    ASSERT_EQ(s.size(), orig_size);
}

TEST_F(str_test, remove_newline2)
{
    auto blank_str = str("\n\n\n\r\r\r\n\r\n\r\r");
    blank_str.remove_newline();

    ASSERT_EQ(blank_str.size(), 0);
}

TEST_F(str_test, replace1)
{
    const auto* line = "Lorem ipsum dolor sit amet, consectetur:"
                       "adipiscing elit. Quantum Aristoxeni"
                       "ingenium consumptum videmus in musicis?"
                       "Quae similitudo in genere etiam humano"
                       "apparet. Ego vero volo in virtute vim"
                       "esse quam maximam; Egone quaeris,"
                       "inquit, quid sentiam? Primum quid"
                       "tu dicis breve? Facile"
                       "pateremur, qui etiam nunc"
                       "agendi aliquid discendique"
                       "causa prope contra naturam"
                       "vígillas suscipere soleamus.";

    s = str(line);
    stls = std::string(line);
    ASSERT_EQ(s.size(), stls.size());

    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        s.replace(i, cnt, cnt, 'c');
        stls.replace(i, cnt, cnt, 'c');
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, replace2)
{
    s.replace(2, 2, 4, 'b');
    stls.replace(2, 2, 4, 'b');

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, replace3)
{
    s = str(line);
    stls = std::string(line);
    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    auto extra_count_dis = dis;
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        auto extra = extra_count_dis(mt);
        char c = rand_char(mt);

        s.replace(i, cnt, cnt + extra, c);
        stls.replace(i, cnt, cnt + extra, c);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, replace4)
{
    stls = std::string(s.c_str());

    s.replace(5, 4, 2, 'b');
    stls.replace(5, 4, 2, 'b');

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, replace5)
{
    const auto* buf = "aaaaa";
    s = buf;
    stls = buf;
    ASSERT_STREQ(s.c_str(), buf);
    ASSERT_EQ(s.size(), std::strlen(buf));

    return;
    s.replace(3, 10, 8, 'b');
    stls.replace(3, 10, 8, 'b');

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, replace6)
{
    s = str(line);
    stls = std::string(line);
    auto dis = std::uniform_int_distribution<size_t>{1, 10};
    auto extra_count_dis = dis;
    for (size_t i = 0, len = std::strlen(line); i < len; ++i) {
        auto cnt = dis(mt);
        auto extra = extra_count_dis(mt);
        auto c = rand_char(mt);

        s.replace(i, cnt + extra, cnt, c);
        stls.replace(i, cnt + extra, cnt, c);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, replace7)
{
    return;
    stls = std::string("hello\tworld\t\t!");
    s = "hello\tworkd\t\t!";
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());

    return;

    for (size_t i = 0; i < s.size(); ++i) {
        if (s[i] == '\t')
            s.replace(i, 1, 8, ' ');
        if (stls[i] == '\t')
            stls.replace(i, 1, 8, ' ');
    }

    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.size(), stls.size());
}

TEST_F(str_test, erase1)
{
    auto rand_erase_cnt = std::uniform_int_distribution<std::size_t>(0, 3);
    while (stls.size()) {
        auto rand_idx = std::uniform_int_distribution<size_t>(0, stls.size());
        auto idx = rand_idx(mt);
        auto cnt = rand_erase_cnt(mt);
        s.erase(idx, cnt);
        stls.erase(idx, cnt);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, erase_iter1)
{
    while (s.size())
        s.erase(std::begin(s));
    ASSERT_STREQ(s.c_str(), "");
}

using std::begin;

TEST_F(str_test, erase_iter2)
{
    while (stls.size()) {
        auto rand_idx = std::uniform_int_distribution<ptrdiff_t>(0,
                static_cast<ptrdiff_t>(stls.size() - 1));
        auto idx = rand_idx(mt);

        s.erase(begin(s) + idx);
        stls.erase(begin(stls) + idx);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, erase_iter3)
{
    const size_t MAX_SIZE = 100;
    auto generate_random_string = []() {
        auto rand_c = std::uniform_int_distribution<char>('!', '~');
        auto mt = std::mt19937{};
        auto string = std::string();
        string.reserve(MAX_SIZE);
        for (size_t i = 0; i < string.capacity(); ++i)
            string.push_back(rand_c(mt));

        return string;
    };

    auto rand_first = std::uniform_int_distribution<ptrdiff_t>(0, MAX_SIZE - 1);
    for (size_t i = 0; i < 1000; ++i) {
        stls = generate_random_string();
        s = stls.c_str();
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());

        auto first_idx = rand_first(mt);
        auto rand_last = std::uniform_int_distribution<ptrdiff_t>(first_idx, MAX_SIZE);
        auto last_idx = rand_last(mt);

        s.erase(begin(s) + first_idx, begin(s) + last_idx);
        stls.erase(begin(stls) + first_idx, begin(stls) + last_idx);

        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
}

TEST_F(str_test, find_iterate)
{
    const auto* line = "hello, hello, world";
    auto s = str(line);
    auto stls = std::string(line);
    size_t pos = 0;
    size_t stls_pos = 0;
    for (size_t i = 0; i < 3; ++i) {
        pos = s.find("hello", pos) + 1;
        stls_pos = stls.find("hello", stls_pos) + 1;
        ASSERT_EQ(pos, stls_pos);
    }

    pos = 0;
    stls_pos = 0;
    for (size_t i = 0; i < 3; ++i) {
        pos = s.find("world", pos) + 1;
        stls_pos = stls.find("world", stls_pos) + 1;
        ASSERT_EQ(pos, stls_pos);
    }
}

TEST_F(str_test, find_empty_haystack)
{
    auto haystack = str();
    auto needle = str("hello");

    auto stls_haystack = std::string();
    auto stls_needle = std::string("hello");

    ASSERT_EQ(haystack.find(needle), stls_haystack.find(stls_needle));
}

TEST_F(str_test, find_empty_haystack_needle)
{
    auto haystack = str();
    auto needle = str();

    auto stls_haystack = std::string();
    auto stls_needle = std::string();

    ASSERT_EQ(haystack.find(needle), stls_haystack.find(stls_needle));
}

TEST_F(str_test, find_empty_needle)
{
    auto haystack = str(line);
    auto needle = str();

    auto stls_haystack = std::string(line);
    auto stls_needle = std::string();

    ASSERT_EQ(haystack.find(needle), stls_haystack.find(stls_needle));
}

TEST_F(str_test, find_starting_from_pos)
{
    auto haystack = str(line);
    auto needle = str("quid");

    auto stls_haystack = std::string(haystack.c_str());
    auto stls_needle = std::string(needle.c_str());

    auto rand_pos = std::uniform_int_distribution<size_t>(0, haystack.size() + 100);
    for (auto i = 0; i < 10000; ++i) {
        auto pos = rand_pos(mt);
        ASSERT_EQ(haystack.find(needle, pos), stls_haystack.find(stls_needle, pos));
    }
}

TEST_F(str_test, find_longer_needle)
{
    auto buf = str(line);
    buf.append(line);
    auto haystack = str(line);
    auto needle = str(buf);

    auto stls_haystack = std::string(haystack.c_str());
    auto stls_needle = std::string(needle.c_str());

    auto rand_pos = std::uniform_int_distribution<size_t>(0, haystack.size() + 12);
    for (auto i = 0; i < 10000; ++i) {
        auto pos = rand_pos(mt);
        ASSERT_EQ(haystack.find(needle, pos), stls_haystack.find(stls_needle, pos));
    }
}

TEST_F(str_test, find_rand1)
{
    for (char c = 'a'; c <= 'z'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto s = gen_str('a', c, 50);
            auto stls = std::string(s.c_str());

            for (auto j = 0; j < 100; ++j) {
                auto rand_begin = std::uniform_int_distribution<ptrdiff_t>(0,
                        static_cast<ptrdiff_t>(s.size() - 1));
                auto begin_idx = rand_begin(mt);
                auto rand_end = std::uniform_int_distribution<ptrdiff_t>(begin_idx,
                        static_cast<ptrdiff_t>(s.size()));
                auto end_idx = rand_end(mt);

                auto substr = str(begin(s) + begin_idx, begin(s) + end_idx);
                ASSERT_EQ(s.find(substr), stls.find(substr.c_str()));
            }
        }
    }
}

TEST_F(str_test, find_rand2)
{
    auto rand_haystack_size = std::uniform_int_distribution<size_t>(0, 120);
    auto rand_needle_size = std::uniform_int_distribution<size_t>(0, 100);
    for (char c = '!'; c <= '~'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto haystack = gen_str('!', c, rand_haystack_size(mt));
            auto stls_haystack = std::string(haystack.c_str());

            for (auto j = 0; j < 20; ++j) {
                auto needle = gen_str('!', c, rand_needle_size(mt));
                auto stls_needle = std::string(needle.c_str());
                ASSERT_EQ(haystack.find(needle), stls_haystack.find(stls_needle))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n';
                ASSERT_EQ(needle.find(haystack), stls_needle.find(stls_haystack))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n';
            }
        }
    }
}

TEST_F(str_test, find_rand3)
{
    auto rand_haystack_size = std::uniform_int_distribution<size_t>(0, 120);
    auto rand_needle_size = std::uniform_int_distribution<size_t>(0, 100);
    for (char c = '!'; c <= '~'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto haystack_size = rand_haystack_size(mt);
            auto haystack = gen_str('!', c, haystack_size);
            auto rand_hastack_start_pos = std::uniform_int_distribution<size_t>(0, haystack_size);
            auto haystack_start_pos = rand_hastack_start_pos(mt);
            auto stls_haystack = std::string(haystack.c_str());

            for (auto j = 0; j < 20; ++j) {
                auto needle_size = rand_needle_size(mt);
                auto needle = gen_str('!', c, needle_size);
                auto rand_haystack_start_pos = std::uniform_int_distribution<size_t>(0, needle_size);
                auto needle_start_pos = rand_hastack_start_pos(mt);
                auto stls_needle = std::string(needle.c_str());
                ASSERT_EQ(haystack.find(needle, haystack_start_pos),
                        stls_haystack.find(stls_needle, haystack_start_pos))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n';
                ASSERT_EQ(needle.find(haystack, needle_start_pos),
                        stls_needle.find(stls_haystack, needle_start_pos))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n';
            }
        }
    }
}

TEST_F(str_test, rfind1)
{
    const auto* haystack = "hello, hello, world, hello, world, hello world";
    const auto* needle = "hello";
    auto s = str(haystack);
    auto stls = std::string(haystack);
    auto len = std::strlen(haystack) + 10;

    for (auto i = len; i <= len; --i)
        ASSERT_EQ(s.rfind(needle, i), stls.rfind(needle, i));
}

TEST_F(str_test, rfind2)
{
    const auto* haystack = "abxaabxabba";
    const auto* needle = "ab";
    auto s = str(haystack);
    auto stls = std::string(haystack);
    auto len = std::strlen(haystack) + 10;

    for (auto i = len; i <= len; --i)
        ASSERT_EQ(s.rfind(needle, i), stls.rfind(needle, i));
}

TEST_F(str_test, rfind3)
{
    const auto* haystack = "abxaabxabba";
    const auto* needle = "ba";
    auto s = str(haystack);
    auto stls = std::string(haystack);
    auto len = std::strlen(haystack) + 10;

    for (auto i = len; i <= len; --i)
        ASSERT_EQ(s.rfind(needle, i), stls.rfind(needle, i));
}

TEST_F(str_test, rfind4)
{
    const auto* haystack = "abxaabxabba";
    const auto* needle = "abxa";
    auto s = str(haystack);
    auto stls = std::string(haystack);
    auto len = std::strlen(haystack) + 10;

    for (auto i = len; i <= len; --i) {
        auto pos = s.rfind(needle, i);
        ASSERT_EQ(pos, stls.rfind(needle, i));
    }
}

TEST_F(str_test, rfind_rand1)
{
    auto rand_haystack_size = std::uniform_int_distribution<size_t>(0, 120);
    auto rand_needle_size = std::uniform_int_distribution<size_t>(0, 100);
    for (char c = '!'; c <= '~'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto haystack = gen_str('!', c, rand_haystack_size(mt));
            auto stls_haystack = std::string(haystack.c_str());

            for (auto j = 0; j < 20; ++j) {
                auto needle = gen_str('!', c, rand_needle_size(mt));
                auto stls_needle = std::string(needle.c_str());
                ASSERT_EQ(haystack.rfind(needle), stls_haystack.rfind(stls_needle))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n'
                    << "haystack: " << haystack.c_str() << '\n'
                    << "needle: " << needle.c_str() << '\n';
                ASSERT_EQ(needle.rfind(haystack), stls_needle.rfind(stls_haystack))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n'
                    << "haystack: " << haystack.c_str() << '\n'
                    << "needle: " << needle.c_str() << '\n';
            }
        }
    }
}

TEST_F(str_test, rfind_rand2)
{
    auto rand_haystack_size = std::uniform_int_distribution<size_t>(0, 120);
    auto rand_needle_size = std::uniform_int_distribution<size_t>(0, 100);
    for (char c = '!'; c <= '~'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto haystack = gen_str('!', c, rand_haystack_size(mt));
            auto stls_haystack = std::string(haystack.c_str());

            for (auto j = 0; j < 20; ++j) {
                auto needle = gen_str('!', c, rand_needle_size(mt));
                auto stls_needle = std::string(needle.c_str());
                ASSERT_EQ(haystack.rfind(needle), stls_haystack.rfind(stls_needle))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n'
                    << "haystack: " << haystack.c_str() << '\n'
                    << "needle: " << needle.c_str() << '\n';
                ASSERT_EQ(needle.rfind(haystack), stls_needle.rfind(stls_haystack))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n'
                    << "haystack: " << haystack.c_str() << '\n'
                    << "needle: " << needle.c_str() << '\n';
            }
        }
    }
}

TEST_F(str_test, rfind_rand3)
{
    auto rand_haystack_size = std::uniform_int_distribution<size_t>(0, 120);
    auto rand_needle_size = std::uniform_int_distribution<size_t>(0, 100);
    for (char c = '!'; c <= '~'; ++c) {
        for (auto i = 0; i < 100; ++i) {
            auto haystack_size = rand_haystack_size(mt);
            auto haystack = gen_str('!', c, haystack_size);
            auto rand_haystack_start_pos = std::uniform_int_distribution<size_t>(0, haystack_size);
            auto haystack_start_pos = rand_haystack_start_pos(mt);
            auto stls_haystack = std::string(haystack.c_str());
            ASSERT_STREQ(haystack.c_str(), stls_haystack.c_str());
            ASSERT_EQ(haystack.size(), stls_haystack.size());

            for (auto j = 0; j < 20; ++j) {
                auto needle_size = rand_needle_size(mt);
                auto needle = gen_str('!', c, needle_size);
                auto rand_needle_start_pos = std::uniform_int_distribution<size_t>(0, needle_size);
                auto needle_start_pos = rand_needle_start_pos(mt);
                auto stls_needle = std::string(needle.c_str());

                // fmt::println("haystack: {}", haystack.c_str());
                // fmt::println("needle: {}", needle.c_str());

                ASSERT_STREQ(needle.c_str(), stls_needle.c_str());
                ASSERT_EQ(needle.size(), stls_needle.size());
                ASSERT_EQ(haystack.rfind(needle, haystack_start_pos),
                        stls_haystack.rfind(stls_needle, haystack_start_pos))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n'
                    << "haystack: " << haystack.c_str() << '\n'
                    << "needle: " << needle.c_str() << '\n'
                    << "haystack start pos: " << haystack_start_pos << '\n';
                ASSERT_EQ(needle.rfind(haystack, needle_start_pos),
                        stls_needle.rfind(stls_haystack, needle_start_pos))
                    << "haystack size: " << haystack.size() << '\n'
                    << "needle size: " << needle.size() << '\n'
                    << "haystack: " << haystack.c_str() << '\n'
                    << "needle: " << needle.c_str() << '\n'
                    << "needle start pos: " << needle_start_pos << '\n';
            }
        }
    }
}

TEST_F(str_test, find_char1)
{
    for (auto i = 0; i < 10000; ++i) {
        auto c = rand_char(mt);
        ASSERT_EQ(s.find(c), stls.find(c));
        ASSERT_EQ(s.rfind(c), stls.rfind(c));
    }
}

TEST_F(str_test, find_char2)
{
    auto rand_idx = std::uniform_int_distribution<size_t>(0, s.size() - 1);
    for (auto i = 0; i < 10000; ++i) {
        auto c = rand_char(mt);
        auto idx = rand_idx(mt);
        ASSERT_EQ(s.find(c, idx), stls.find(c, idx));
        ASSERT_EQ(s.rfind(c, idx), stls.rfind(c, idx));
    }
}

TEST_F(str_test, compare1)
{
    ASSERT_STREQ(s.c_str(), stls.c_str());
    ASSERT_EQ(s.compare(stls.c_str()), stls.compare(s.c_str()));
}

TEST_F(str_test, compare2)
{
    for (size_t i = 0; i < 1000; ++i) {
        auto s1 = gen_str('!', '~', i);
        ASSERT_EQ(s1.compare(s1), 0);
    }
}

TEST_F(str_test, compare3)
{
    for (size_t i = 0; i < 1000; ++i) {
        auto s1 = gen_str('!', '~', i);
        auto s2 = gen_str('!', '~', i);
        std::string stls1 = s1.c_str();
        std::string stls2 = s2.c_str();

        ASSERT_TRUE(is_zero_or_same_sign(s1.compare(s2), stls1.compare(stls2)));
        ASSERT_TRUE(is_zero_or_same_sign(s2.compare(s1), stls2.compare(stls1)));
    }
}

TEST_F(str_test, compare4)
{
    auto rand_size = std::uniform_int_distribution<size_t>(200, 1000);
    for (auto i = 0; i < 26; ++i) {
        auto size = rand_size(mt);
        auto s1 = gen_str('a', static_cast<char>('a' + i), size);
        auto s2 = gen_str('!', static_cast<char>('a' + i), size);
        std::string stls1 = s1.c_str();
        std::string stls2 = s2.c_str();

        ASSERT_EQ(s1.compare(s2), stls1.compare(stls2));
        ASSERT_EQ(s2.compare(s1), stls2.compare(stls1));
    }
}

TEST_F(str_test, compare5)
{
    auto comment_syn = str("//");
    auto s1 = str("01// comment");
    auto stls1 = std::string(s1.c_str());

    for (size_t i = 0; i < s1.size(); ++i) {
        ASSERT_TRUE(is_zero_or_same_sign(s1.compare(i, comment_syn.size(), comment_syn),
                stls1.compare(i, comment_syn.size(), comment_syn.c_str())));
    }
}

TEST_F(str_test, compare6)
{
    for (size_t i = 1; i < 500; ++i) {
        auto s1 = gen_str('!', '~', i);
        auto rand_idx = std::uniform_int_distribution<size_t>(0, i - 1);
        auto rand_char = std::uniform_int_distribution<char>('!', '~');
        for (size_t j = 0; j < s1.size(); ++j) {
            auto idx = rand_idx(mt);
            auto c = rand_char(mt);
            auto s2 = s1;
            auto diff1 = s2[idx] - c;
            auto diff2 = c - s2[idx];
            s2[idx] = c;
            ASSERT_EQ(s1.compare(s2), diff1);
            ASSERT_EQ(s2.compare(s1), diff2);
        }
    }
}

TEST_F(str_test, compare_this_idx)
{
    auto rand_size = std::uniform_int_distribution<size_t>(200, 1000);
    for (auto i = 0; i < 26; ++i) {
        for (auto k = 0; k < 100; ++k) {
            auto size = rand_size(mt);
            auto s1 = gen_str('a', static_cast<char>('a' + i), size);
            auto s2 = gen_str('a', static_cast<char>('a' + i), size);
            std::string stls1 = s1.c_str();
            std::string stls2 = s2.c_str();

            for (size_t j = 0; j < size; ++j) {
                auto res_s1 = s1.compare(j, s1.size(), s2);
                auto res_stls1 = stls1.compare(j, stls1.size(), stls2);
                ASSERT_TRUE(is_zero_or_same_sign(res_s1, res_stls1))
                    << "res_s1: " << res_s1 << '\n'
                    << "res_stls1: " << res_stls1 << '\n';

                auto res_s2 = s2.compare(j, s2.size(), s1);
                auto res_stls2 = stls2.compare(j, stls2.size(), stls1);
                ASSERT_TRUE(is_zero_or_same_sign(res_s2, res_stls2))
                    << "res_s1: " << res_s1 << '\n'
                    << "res_stls1: " << res_stls1 << '\n';
            }
        }
    }
}

TEST_F(str_test, compare_one_diff)
{
    for (size_t i = 1; i < 150; ++i) {
        auto s1 = gen_str('!', '~', i);
        auto rand_idx = std::uniform_int_distribution<size_t>(0, i - 1);
        auto rand_char = std::uniform_int_distribution<char>('!', '~');
        for (size_t j = 0; j < s1.size(); ++j) {
            auto idx = rand_idx(mt);
            auto c = rand_char(mt);
            auto s2 = s1;
            auto diff1 = s2[idx] - c;
            auto diff2 = c - s2[idx];
            s2[idx] = c;

            for (size_t k = 0; k < s1.size(); ++k) {
                ASSERT_EQ(s1.compare(k, s1.size(), s2, k, s2.size()), k <= idx ? diff1 : 0);
                ASSERT_EQ(s2.compare(k, s2.size(), s1, k, s2.size()), k <= idx ? diff2 : 0);
            }
        }
    }
}
