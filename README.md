# C++ String (`str`) Implementation

This custom string class provides an educational implementation of a string-like
container, with efficient memory management via Small Buffer Optimization (SBO)
and STL compatibility.

## Key Features

- Small Buffer Optimization
- Rule of Five Compliance
- Iterator Support:
	- `iterator`, `const_iterator`, `reverse_iterator`, and
	  `const_reverse_iterator`
	- Implements contiguous iterator category for STL compatibility
- String Operations:
    - $O(n + m)$ time complexity for `find()`, `rfind()` via Knuth–Morris–Pratt
      algorithm
	- Includes `append()`, `insert()`, `replace()`, `erase()`, `clear()`
- Copy-Swap Idiom:
	- Simplifies assignment operator by handling both copy and move assignment
	  utilizing `swap()`
    - strong exception safety by copying the resource before modifying the
      original object state
- Compile-Time Optimization: Uses `constexpr` for template arguments, optimizing
  away runtime branching and overhead

## Testing

Unit Tests via Google Test (`gtest`): Over 100 test cases utilizing `rng` for
improving test case coverage and validating the functionality of the `str`
class, including SBO behavior, string operations, search algorithms and iterator
compatibility with STL algorithms.
