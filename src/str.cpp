#include "str.hpp"

#include <algorithm>
#include <cstddef>
#include <iterator>
#include <vector>
#include <cstring>
#include <stdexcept>
#include <utility>

str::str(const_pointer s)
{
    if (!s) [[unlikely]]
        return;

    m_size = std::strlen(s);
    reserve(m_size);
    std::strcpy(bptr, s);
}

str::str(const str& s)
    : m_size{s.m_size}
{
    reserve(m_size);
    std::strcpy(bptr, s.bptr);
}

str::str(str&& s)
    : m_capacity{s.m_capacity}
    , m_size{s.m_size}
    , sbo{s.sbo}
{
    if (sbo) {
        std::strcpy(smb, s.smb);
    } else {
        bptr = dynb = std::exchange(s.bptr, nullptr);
        s.dynb = nullptr; // nulling the dynb so that the moved out dyn buffer
                          // address doesn't get swapped into a tmp obj and get
                          // destroyed when reused again
    }
}

str::str(const_iterator first, const_iterator last)
    : m_size{static_cast<size_type>(std::distance(first, last))}
{
    reserve(m_size);
    std::copy(first, last, this->begin());
}

str::~str()
{
    if (!sbo)
        delete[] bptr;
}

str& str::operator=(str s)
{
    swap(*this, s);
    return *this;
}

void swap(str& lhs, str& rhs)
{
    using std::swap;

    swap(lhs.m_capacity, rhs.m_capacity);
    swap(lhs.m_size, rhs.m_size);
    swap(lhs.sbo, rhs.sbo);
    if (lhs.sbo && rhs.sbo) {
        swap(lhs.smb, rhs.smb);
        lhs.bptr = lhs.smb;
        rhs.bptr = rhs.smb;
    } else if (lhs.sbo) {
        // rhs was sbo, lhs was dyn
        auto* tmp_dynb = lhs.dynb;
        swap(lhs.smb, rhs.smb);
        lhs.bptr = lhs.smb;
        rhs.bptr = rhs.dynb = tmp_dynb;
    } else if (rhs.sbo) {
        // lhs was sbo, rhs was dyn
        lhs.bptr = lhs.dynb = std::exchange(rhs.dynb, nullptr);
    } else {
        swap(lhs.dynb, rhs.dynb);
        swap(lhs.bptr, rhs.bptr);
    }
}

void str::push_back(value_type c)
{ this->append(1, c); }

void str::pop_back()
{ bptr[--m_size] = 0; }

str& str::append(size_type count, value_type c)
{
    reserve(m_size + count);
    std::memset(bptr + m_size, c, count);
    m_size += count;
    bptr[m_size] = 0;

    return *this;
}

str& str::append(const str& s, size_type n)
{ return this->append(s.c_str(), n); }

str& str::append(const_pointer s, size_type n)
{
    auto copy_size = std::min(std::strlen(s), n);
    if (!copy_size) [[unlikely]]
        return *this;

    reserve(m_size + copy_size + 1);
    std::memcpy(bptr + m_size, s, copy_size);
    m_size += copy_size;
    bptr[m_size] = 0;

    return *this;
}

str& str::resize(size_type count, value_type c)
{
    reserve(count);
    for (auto i = m_size; i < count; ++i)
        bptr[i] = c;
    m_size = count;
    bptr[m_size] = 0;

    return *this;
}

void str::reserve(size_type new_size)
{
    if (new_size < m_capacity)
        return;

    do {
        m_capacity <<= 1;
    } while (m_capacity <= new_size);

    auto* new_buf = new value_type[m_capacity]{};
    std::strcpy(new_buf, bptr);
    if (!std::exchange(sbo, false))
        delete[] bptr;
    bptr = dynb = new_buf;
}

str& str::insert(size_type index, size_type count, int c)
{
    if (!count) [[unlikely]]
        return *this;
    if (index > m_size) [[unlikely]]
        throw std::out_of_range("accessing index beyond the underlying buffer size");

    reserve(m_size + count);
    // a b c d e f      idx = 2, count = 2 =>
                        // 1. arg = index + count = 2 + 2 = 4
                        // 2. arg = index = 2
                        // 3. arg = m_size - index + 1 = 6 - 2 + 1 = 5
    auto* dest = bptr + index + count;
    auto* src = bptr + index;
    auto n = m_size - index + 1;
    std::memmove(dest, src, n);
    for (size_type i = 0; i < count; ++i)
        bptr[index + i] = static_cast<value_type>(c);
    m_size += count;
    return *this;
}

str& str::insert(size_type index, const str& s)
{
    if (!s.m_size) [[unlikely]]
        return *this;
    if (index > m_size) [[unlikely]]
        throw std::out_of_range("erase index out of range");

    reserve(m_size + s.m_size);
    std::memmove(bptr + index + s.m_size, bptr + index, m_size - index + 1);
    std::memcpy(bptr + index, s.bptr, s.m_size);
    m_size += s.m_size;
    return *this;
}

str& str::clear()
{
    m_size = 0;
    bptr[m_size] = 0;
    return *this;
}

str& str::remove_newline()
{
    while (m_size && (bptr[m_size - 1] == '\r' || bptr[m_size - 1] == '\n'))
        bptr[--m_size] = '\0';
    return *this;
}

str& str::replace(size_type index, size_type count, size_type count2, value_type c)
{
    if (!count || !count2) [[unlikely]]
        return *this;
    if (index > m_size) [[unlikely]]
        throw std::out_of_range("accessing index beyond the underlying buffer size");

    if (count == count2) {
        auto new_size = index + count;
        reserve(new_size);
        for (size_type i = 0; i < count; ++i)
            bptr[index + i] = c;
        m_size = std::max(m_size, new_size);
    } else if (count < count2) {
        // 0 1 2 3 4 5 6 7 8
        // a a a a a            idx = 2, cnt = 2, cnt2 = 4, char = b
        // a a b b b b a
        //  realloc = m_size - std::min(count, m_size - index) + count2 = 5 - 2 + 4 = 7
        auto new_size = m_size + count2 - std::min(count, m_size - index);
        reserve(new_size);

        //  std::memmove():
        //      1. arg dest: idx + cnt2 = 2 + 4 = 6
        //      2. arg src: idx + cnt = 2 + 2 = 4
        //      3. arg n: m_size - idx - cnt = 5 - 2 - 2 = 1
        auto* dest = bptr + index + count2;
        auto* src = bptr + index + count;
        auto n = m_size - index - count;
        m_size = new_size;
        if (dest < bptr + m_size)
            std::memmove(dest, src, n);
        for (size_type i = 0; i < count2; ++i)
            bptr[index + i] = c;
    } else {
        // 0 1 2 3 4 5 6 7
        // a a a a a        index = 1, count = 10, count2 = 5
        // a b b b b b
        auto replace = std::min(count, m_size - index);
        for (size_type i = 0; i < replace; ++i)
            bptr[index + i] = 0;

        if (replace < count2) {
            reserve(m_size + count2 - replace);
            m_size = m_size + count2 - replace;
        } else {
            // 0 1 2 3 4 5 6 7 8    len = 9
            // a a a a a c c c c    index = 1, count = 4, count2 = 2, char = b
            // a b b c c c c
            // std::memmove():
            //      1. arg dest: 3 = index + count2
            //      2. arg src: 5 = index + count
            //      3. arg n: 4 = m_size - index - count = 9 - 1 - 4
            auto* dest = bptr + index + count2;
            auto* src = bptr + index + replace;
            auto n = m_size - index - replace;
            if (dest < bptr + m_size)
                std::memmove(dest, src, n);
            m_size -= replace - count2;
        }

        for (size_type i = 0; i < count2; ++i)
            bptr[index + i] = c;
        bptr[m_size] = 0;
    }

    return *this;
}

str& str::erase(size_type index, size_type count)
{
    if (index > m_size)
        throw std::out_of_range("accessing index beyond the underlying buffer size");

    if (count == npos || index + count >= m_size) {
        bptr[index] = 0;
        m_size = index;
        return *this;
    }

    // 0 1 2 3 4
    // h e l l o
    //              index = 1, count 2
    //              dest = index = 1
    //              src = index + count = 1 + 2 = 3
    //              n = m_size - index - count = 5 - 1 - 2 = 2
    auto* dest = bptr + index;
    auto* src = bptr + index + count;
    auto n = m_size - index - count;
    std::memmove(dest, src, n);
    m_size -= count;
    bptr[m_size] = 0;

    return *this;
}

str& str::erase(const_iterator it)
{
    auto index = static_cast<size_type>(it - this->begin());
    return this->erase(index, 1);
}

str& str::erase(const_iterator first, const_iterator last)
{
    auto index = static_cast<size_type>(first - this->begin());
    auto cnt = static_cast<size_type>(last - first);
    return this->erase(index, cnt);
}

str::size_type str::find(const str& needle, size_type pos) const
{
    if (needle.empty())
        return pos;
    if (needle.size() > m_size)
        return npos;

    using std::begin, std::end;
    const auto lps = gen_lps(begin(needle), end(needle));
    auto h_begin = begin(*this);
    auto h_end = end(*this);
    auto n_begin = begin(needle);
    auto n_end = end(needle);

    return kmp(pos, h_begin, h_end, n_begin, n_end, lps);
}

str::size_type str::rfind(const str& needle, size_type pos) const
{
    if (needle.empty())
        return std::min(m_size, pos);
    if (needle.size() > m_size)
        return npos;

    using std::rbegin, std::rend;
    const auto lps = gen_lps(rbegin(needle), rend(needle));

    size_type rpos = 0;
    if (m_size - needle.size() >= pos)
        rpos = m_size - needle.size() - pos;
    auto h_rbegin = rbegin(*this);
    auto h_rend = rend(*this);
    auto n_rbegin = rbegin(needle);
    auto n_rend = rend(needle);

    auto res = kmp(rpos, h_rbegin, h_rend, n_rbegin, n_rend, lps);
    if (res != npos)
        res = m_size - needle.size() - res;
    return res;
}

str::size_type str::find(value_type c, size_type pos) const
{
    using std::begin, std::end;
    auto be = begin(*this) + pos;
    auto en = end(*this);
    return this->find_char(be, en, c);
}

str::size_type str::rfind(value_type c, size_type pos) const
{
    using std::rbegin, std::rend;
    pos = (pos >= m_size) ? 0 : m_size - pos - 1;
    auto be = rbegin(*this) + static_cast<ptrdiff_t>(pos);
    auto en = rend(*this);
    pos = this->find_char(be, en, c);
    if (pos != str::npos)
        pos = m_size - 1 - pos;
    // 0 1 2 3 4 5
    // 5 4 3 2 1 0
    return pos;
}

int str::compare(size_type pos1, size_type count1,
        const str& s, size_type pos2, size_type count2) const
{
    auto rlen = std::min(count1, count2);
    auto size1 = this->size();
    auto size2 = s.size();

    for (size_type i = 0; i < rlen; ++i) {
        if (pos1 + i == size1 && pos2 + i == size2)
            return 0;
        else if (pos1 + i == size1)
            return -1;
        else if (pos2 + i == size2)
            return 1;
        auto res = bptr[pos1 + i] - s[pos2 + i];
        if (res)
            return res;
    }

    return (count1 == count2) ? 0 : (count1 < count2) ? -1 : 1;
}

int str::compare(size_type pos1, size_type count1, const str& s) const
{
    return this->compare(pos1, count1, s, 0, s.size());
}

int str::compare(const str& s) const
{
    return this->compare(0, this->size(), s, 0, s.size());
}
