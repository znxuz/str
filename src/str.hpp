#pragma once

#include <iterator>
#include <type_traits>
#include <vector>

class str
{
public:
    using value_type = char;
    using size_type = std::size_t;
    using reference = value_type&;
    using const_reference = const value_type&;
    using pointer = value_type*;
    using const_pointer = const value_type*;
    using difference_type = std::ptrdiff_t;
    using iterator_category = std::contiguous_iterator_tag;
    using iterator = pointer;
    using const_iterator = const_pointer;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    static constexpr size_type npos = static_cast<size_type>(-1);

    str() = default;

    str(const_pointer);

    str(const str&);

    str(str&&);

    str(const_iterator, const_iterator);

    ~str();

    str& operator=(str);

    friend void swap(str&, str&);

    reference operator*()
    { return *bptr; }

    const_reference operator*() const
    { return *bptr; }

    pointer operator->()
    { return bptr; }

    const_pointer operator->() const
    { return bptr; }

    reference operator[](size_type i)
    { return bptr[i]; }

    const_reference operator[](size_type i) const
    { return bptr[i]; }

    const_pointer c_str() const
    { return bptr; }

    bool empty() const
    { return !this->m_size; }

    size_type size() const
    { return this->m_size; }

    size_type capacity() const
    { return this->m_capacity; }

    reference front()
    { return bptr[0]; }

    reference front() const
    { return const_cast<str&>(*this).front(); }

    reference back()
    { return bptr[m_size - 1]; }

    reference back() const
    { return const_cast<str&>(*this).back(); }

    iterator begin()
    { return bptr; }

    iterator end()
    { return iterator(&bptr[m_size]); }

    const_iterator begin() const
    { return const_iterator(&bptr[0]); }

    const_iterator end() const
    { return const_iterator(&bptr[m_size]); }

    const_iterator cbegin() const
    { return begin(); }

    const_iterator cend() const
    { return end(); }

    reverse_iterator rbegin()
    { return std::make_reverse_iterator(this->end()); }

    reverse_iterator rend()
    { return std::make_reverse_iterator(this->begin()); }

    const_reverse_iterator rbegin() const
    { return std::make_reverse_iterator(this->end()); }

    const_reverse_iterator rend() const
    { return std::make_reverse_iterator(this->begin()); }

    const_reverse_iterator crbegin() const
    { return rbegin(); }

    const_reverse_iterator crend() const
    { return rend(); }

    void push_back(value_type);

    void pop_back();

    str& append(size_type, value_type);

    str& append(const str&, size_type count = npos);

    str& append(const_pointer, size_type count = npos);

    template<typename input_iter>
        str& append(input_iter first, input_iter last)
        {
            while (first != last)
                this->append(1, *first++);

            return *this;
        }

    str& insert(size_type, size_type, int);

    str& insert(size_type, const str&);

    str& replace(size_type, size_type, size_type, value_type);

    str& resize(size_type, value_type c = '\0');

    void reserve(size_type);

    str& clear();

    str& erase(size_type, size_type count = npos);

    str& erase(const_iterator pos);

    str& erase(const_iterator, const_iterator);

    size_type find(const str&, size_type pos = 0) const;

    size_type rfind(const str&, size_type pos = npos) const;

    size_type find(value_type, size_type pos = 0) const;

    size_type rfind(value_type, size_type pos = npos) const;

    int compare(size_type, size_type, const str&, size_type, size_type = str::npos) const;

    int compare(size_type, size_type, const str&) const;

    int compare(const str&) const;

    str& remove_newline();

private:
    static constexpr unsigned int SBO_SIZE = 15;
    size_type m_capacity{SBO_SIZE};
    size_type m_size{0};
    union {
        value_type smb[SBO_SIZE]{};
        value_type* dynb;
    };
    bool sbo{true};
    value_type* bptr{smb};

    template<typename iter>
    str::size_type find_char(iter from, iter to, value_type c) const
    {
        static_assert(std::is_same_v<iter, const_iterator>
                || std::is_same_v<iter, const_reverse_iterator>);

        using std::begin, std::rbegin;
        while (from < to) {
            if (*from == c) {
                if constexpr (std::is_same_v<iter, const_reverse_iterator>)
                    return static_cast<str::size_type>(std::distance(rbegin(*this), from));
                else
                    return static_cast<str::size_type>(std::distance(begin(*this), from));
            }
            ++from;
        }
        return str::npos;
    }
};


template<typename iter>
std::vector<str::size_type> gen_lps(iter begin, iter end)
{
    str::size_type size = static_cast<str::size_type>(std::distance(begin, end));
    auto lps = std::vector<str::size_type>(size, 0);

    str::size_type i = 0, j = 1;

    while (begin + j != end) {
        if (*(begin + i) == *(begin + j)) {
            lps[j++] = ++i;
        } else {
            if (!i)
                ++j;
            else
                i = lps[i - 1];
        }
    }
    return lps;
}

template<typename iter>
str::size_type kmp(str::size_type pos, iter h_begin, iter h_end,
        iter n_begin, iter n_end, const std::vector<str::size_type>& lps)
{
    auto n_size = std::distance(n_begin, n_end);
    for (str::size_type j = 0; h_begin + pos + n_size - 1 < h_end;) {
        for (; n_begin + j < n_end && *(h_begin + pos + j) == *(n_begin + j); ++j);
        if (n_begin + j == n_end)
            return pos;

        auto skip = (j) ? j - lps[j - 1] : 1;   // 1: get lps from the matched
                                                //    substr len
        pos += skip;                              // 2: skip the substr len
        j = (skip != 1) ? lps[j - 1] : 0;       // 3: skip the already matched
                                                //    prefix too
    }
    return str::npos;
}
