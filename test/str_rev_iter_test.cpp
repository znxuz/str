#include <gtest/gtest.h>
#include <random>

#include "../src/str.hpp"

class str_rev_iter_test : public ::testing::Test
{
protected:
    str s;
    std::string stls;
    const char* line = "Lorem ipsum dolor sit amet, consectetur:"
                       "adipiscing elit. Quantum Aristoxeni"
                       "ingenium consumptum videmus in musicis?"
                       "Quae similitudo in genere etiam humano"
                       "apparet. Ego vero volo in virtute vim"
                       "esse quam maximam; Egone quaeris,"
                       "inquit, quid sentiam? Primum quid"
                       "tu dicis breve? Facile"
                       "pateremur, qui etiam nunc"
                       "agcendi aliquid disccendique"
                       "causa prope contra naturam"
                       "vígillas suscipere soleamus.";
    std::mt19937 mt{};

    void SetUp() override
    {
        s = line;
        stls = line;
        mt.seed(std::random_device{}());

        ASSERT_STREQ(s.c_str(), line);
        ASSERT_STREQ(s.c_str(), stls.c_str());
        ASSERT_EQ(s.size(), stls.size());
    }
};

using std::rbegin, std::rend;

TEST_F(str_rev_iter_test, forward_iterate)
{
    auto rb = rbegin(s);
    auto re = rend(s);

    auto stls_rb = rbegin(stls);
    auto stls_re = rend(stls);
    ASSERT_EQ(std::distance(rb, re), std::distance(stls_rb, stls_re));

    while (rb != re)
        ASSERT_EQ(*rb++, *stls_rb++);
}

TEST_F(str_rev_iter_test, iter_copy)
{
    size_t i = s.size() - 1;
    for (auto it = rbegin(s); it != rend(s); ++it) {
        const auto cp = it;
        ASSERT_EQ(*it, s[i--]);
        ASSERT_EQ(*cp, *it);
    }
}

TEST_F(str_rev_iter_test, ordering_less)
{
    auto it = rbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs < rhs;
        ASSERT_EQ(it + lhs < it + rhs, res);
    }
}

TEST_F(str_rev_iter_test, ordering_less_eq)
{
    auto it = rbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs <= rhs;
        ASSERT_EQ(it + lhs <= it + rhs, res);
    }
}

TEST_F(str_rev_iter_test, ordering_eq)
{
    auto it = rbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs == rhs;
        ASSERT_EQ(it + lhs == it + rhs, res);
    }
}

TEST_F(str_rev_iter_test, ordering_eq_greater)
{
    auto it = rbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs >= rhs;
        ASSERT_EQ(it + lhs >= it + rhs, res);
    }
}

TEST_F(str_rev_iter_test, greater)
{
    auto it = rbegin(s);
    auto rand_diff = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    for (size_t i = 0; i < 1000; ++i) {
        auto lhs = rand_diff(mt);
        auto rhs = rand_diff(mt);
        auto res = lhs > rhs;
        ASSERT_EQ(it + lhs > it + rhs, res);
    }
}

TEST_F(str_rev_iter_test, random_access_read)
{
    auto it = std::rbegin(s);
    auto cmp_it = std::rbegin(stls);
    auto rand = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);

    for (size_t i = 0; i < 1000; ++i) {
        auto idx = rand(mt);
        ASSERT_EQ(*(it + idx), *(cmp_it + idx));
        ASSERT_EQ(*(it + idx), s[s.size() - 1 - static_cast<size_t>(idx)]);
    }
}

TEST_F(str_rev_iter_test, random_access_write)
{
    auto it = std::rbegin(s);
    auto cmp_it = std::rbegin(stls);
    auto rand = std::uniform_int_distribution<ptrdiff_t>(0,
            static_cast<ptrdiff_t>(s.size()) - 1);
    auto rand_char = std::uniform_int_distribution<char>('a', 'z');

    for (size_t i = 0; i < 1000; ++i) {
        auto idx = rand(mt);
        auto c = rand_char(mt);
        *it = c;
        *cmp_it = c;
        ASSERT_EQ(*it, c);
        ASSERT_EQ(*(it + idx), *(cmp_it + idx));
        ASSERT_EQ(*(it + idx), s[s.size() - 1 - static_cast<size_t>(idx)]);
    }
}

TEST_F(str_rev_iter_test, stl_sort_default)
{
    std::sort(rbegin(s), rend(s));
    std::sort(rbegin(stls), rend(stls));

    ASSERT_STREQ(s.c_str(), stls.c_str());
}

TEST_F(str_rev_iter_test, stl_sort_greater_eq)
{
    std::sort(rbegin(s), rend(s), std::greater_equal<char>{});
    std::sort(rbegin(stls), rend(stls), std::greater_equal<char>{});

    ASSERT_STREQ(s.c_str(), stls.c_str());
}

TEST_F(str_rev_iter_test, stl_sort_greater)
{
    std::sort(rbegin(s), rend(s), std::greater<char>{});
    std::sort(rbegin(stls), rend(stls), std::greater<char>{});

    ASSERT_STREQ(s.c_str(), stls.c_str());
}

TEST_F(str_rev_iter_test, stl_copy)
{
    auto buf = str();
    auto cmp_buf = std::string();

    buf.resize(s.size());
    cmp_buf.resize(stls.size());

    std::copy(rbegin(s), rend(s), rbegin(buf));
    std::copy(rbegin(stls), rend(stls), rbegin(cmp_buf));

    ASSERT_STREQ(buf.c_str(), cmp_buf.c_str());
}

TEST_F(str_rev_iter_test, stl_transform)
{
    auto buf = str();
    buf.resize(s.size());
    auto cmp_buf = std::string();
    cmp_buf.resize(s.size());

    std::transform(rbegin(s), rend(s), rbegin(buf), [](char c) { return std::toupper(c); });
    std::transform(rbegin(stls), rend(stls), rbegin(cmp_buf), [](char c) { return std::toupper(c); });

    ASSERT_STREQ(buf.c_str(), cmp_buf.c_str());
}

TEST_F(str_rev_iter_test, stl_find)
{
    auto rand_c = std::uniform_int_distribution<char>('a', 'z');

    for (auto i = 0; i < 1000; ++i) {
        auto c = rand_c(mt);

        auto res1 = std::find(rbegin(s), rend(s), c) == rend(s);
        auto res2 = std::find(rbegin(stls), rend(stls), c) == rend(stls);
        ASSERT_EQ(res1, res2);
    }
}

TEST_F(str_rev_iter_test, stl_reverse)
{
    std::reverse(rbegin(s), rend(s));
    std::reverse(rbegin(stls), rend(stls));

    ASSERT_STREQ(s.c_str(), stls.c_str());
}

TEST_F(str_rev_iter_test, stl_count)
{
    for (size_t i = 0; i < s.size(); ++i) {
        auto cnt1 = std::count(rbegin(s), rend(s), s[i]);
        auto cnt2 = std::count(rbegin(stls), rend(stls), stls[i]);
        ASSERT_EQ(cnt1, cnt2);
    }
}

TEST_F(str_rev_iter_test, stl_distance1)
{
    auto rand_idx = std::uniform_int_distribution<size_t>(0, s.size());
    for (size_t i = 0; i < 10000; ++i) {
        auto idx1 = rand_idx(mt);
        auto idx2 = rand_idx(mt);
        auto moved_it1 = rbegin(s);
        auto moved_it2 = rbegin(s);

        std::advance(moved_it1, idx1);
        std::advance(moved_it2, idx2);

        ASSERT_EQ(std::distance(rbegin(s), moved_it1), moved_it1 - rbegin(s));
        ASSERT_EQ(std::distance(rbegin(s), moved_it2), moved_it2 - rbegin(s));
    }
}

TEST_F(str_rev_iter_test, stl_distance2)
{
    auto rand_idx = std::uniform_int_distribution<size_t>(0, s.size());
    for (size_t i = 0; i < 10000; ++i) {
        auto idx1 = rand_idx(mt);
        auto idx2 = rand_idx(mt);

        auto it1 = rbegin(s);
        auto it2 = rbegin(s);
        std::advance(it1, idx1);
        std::advance(it2, idx2);

        auto dist = std::distance(it1, it2);
        ASSERT_EQ(dist, idx2 - idx1);
    }
}

TEST_F(str_rev_iter_test, stl_fill)
{
    std::fill(rbegin(s), rend(s), 'c');
    std::fill(rbegin(stls), rend(stls), 'c');
    ASSERT_STREQ(s.c_str(), stls.c_str());
}
