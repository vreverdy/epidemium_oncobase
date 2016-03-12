// ============================== STRING VIEW =============================== //
// Project:         epidemium_oncobase
// Name:            string_view.hpp
// Description:     A view over a string with parsing methods
// Creator:         Vincent Reverdy
// Contributor(s):  Vincent Reverdy [2015-2016]
// License:         GNU GPLv3
// ========================================================================== //
/** 
 * string_view.hpp
 * Copyleft 2015-2016 by Vincent Reverdy
 * This file is part of epidemium_oncobase. 
 * 
 * epidemium_oncobase is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by the Free 
 * Software Foundation, either version 3 of the License, or any later version.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for 
 * more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
// ========================================================================== //
#ifndef _STRING_VIEW_HPP_INCLUDED
#define _STRING_VIEW_HPP_INCLUDED
// ========================================================================== //



// ============================== PREPROCESSOR ============================== //
// Include C++
#include <array>
#include <cctype>
#include <string>
#include <vector>
#include <cassert>
#include <iomanip>
#include <utility>
#include <iostream>
#include <algorithm>
#include <stdexcept>
// Include others
// Miscellaneous
namespace epidemium_oncobase {
// ========================================================================== //



/* ******************************* STRING VIEW ****************************** */
// String view class definition
class string_view
{
    // Types
    public:
    using string_type = std::string;
    using value_type = string_type::value_type;
    using size_type = string_type::size_type;
    using difference_type = string_type::difference_type;
    using reference = string_type::reference;
    using const_reference = string_type::const_reference;
    using pointer = string_type::pointer;
    using const_pointer = string_type::const_pointer;
    using iterator = string_type::iterator;
    using const_iterator = string_type::const_iterator;
    using reverse_iterator = string_type::reverse_iterator;
    using const_reverse_iterator = string_type::const_reverse_iterator;
    
    // Lifecycle
    public:
    string_view() noexcept;
    template <class I> string_view(I first, I last);
    template <class C> string_view(C&& container);
    
    // Conversion
    public:
    explicit operator std::string() const;
    
    // Iterators
    public:
    const_iterator begin() const noexcept;
    const_iterator cbegin() const noexcept;
    const_iterator end() const noexcept;
    const_iterator cend() const noexcept;
    const_reverse_iterator rbegin() const noexcept;
    const_reverse_iterator crbegin() const noexcept;
    const_reverse_iterator rend() const noexcept;
    const_reverse_iterator crend() const noexcept;

    // Element access
    public:
    const_reference at(size_type pos) const;
    const_reference operator[](size_type pos) const;
    const_reference front() const;
    const_reference back() const;
    const_pointer data() const;
    
    // Capacity
    public:
    size_type size() const noexcept;
    size_type max_size() const noexcept;
    bool empty() const noexcept;
    
    // Partitioning
    public:
    std::vector<string_view> split(const string_type& str = "") const;
    std::array<string_view, 3> partition(const string_type& str) const;
    std::array<string_view, 3> rpartition(const string_type& str) const;
    
    // Stripping
    public:
    string_view lstrip() const;
    string_view lstrip(value_type x) const;
    template <class... T> string_view lstrip(T... xn) const;
    template <class F> string_view lstrip(F f) const;
    string_view rstrip() const;
    string_view rstrip(value_type x) const;
    template <class... T> string_view rstrip(T... xn) const;
    template <class F> string_view rstrip(F f) const;
    string_view strip() const;
    string_view strip(value_type x) const;
    template <class... T> string_view strip(T... xn) const;
    template <class F> string_view strip(F f) const;
    
    // Selecting
    public:
    template <class S> string_view select(S&& selection);
    template <class I, class S> string_view select(I&& ignore, S&& selection);
    template <class S> string_view rselect(S&& selection);
    template <class I, class S> string_view rselect(I&& ignore, S&& selection);

    // Operators
    public:
    void swap(string_view& other) noexcept;
    string_type to_string() const;
    bool operator==(string_view other) const;
    bool operator!=(string_view other) const;

    // Streaming
    public:
    friend std::ostream& operator<<(std::ostream &os, string_view v);
    
    // Implementation details: data members
    private:
    const_iterator _first;
    const_iterator _last;
};

// Helpers
template <class T> 
constexpr bool equal_to_any(T&& x);
template <class T, class T0> 
constexpr bool equal_to_any(T&& x, T0&& x0);
template <class T, class T0, class... TN> 
constexpr bool equal_to_any(T&& x, T0&& x0, TN&&... xn);
template <class T> 
constexpr bool equal_to_all(T&& x);
template <class T, class T0> 
constexpr bool equal_to_all(T&& x, T0&& x0);
template <class T, class T0, class... TN> 
constexpr bool equal_to_all(T&& x, T0&& x0, TN&&... xn);
/* ************************************************************************** */



// ------------------------- STRING VIEW: LIFECYCLE ------------------------- //
// Constructs an empty view
string_view::
string_view() 
noexcept
: _first()
, _last()
{
}
    
// Constructs a view from a range of iterators
template <class I> 
string_view::
string_view(I first, I last)
: _first(first)
, _last((assert(first <= last), last))
{
}

// Constructs a view from a string 
template <class C>
string_view::
string_view(C&& container)
: _first(std::cbegin(std::forward<C>(container)))
, _last(std::cend(std::forward<C>(container)))
{
}
// -------------------------------------------------------------------------- //



// ------------------------- STRING VIEW: CONVERSION ------------------------ //
// Converts the string view to a string
string_view::
operator std::string() const
{
    return std::string(_first, _last);
}
// -------------------------------------------------------------------------- //



// ------------------------- STRING VIEW: ITERATORS ------------------------- //
// Returns an iterator to the first element of the view 
string_view::const_iterator 
string_view::
begin() 
const noexcept
{
    return const_iterator(_first);
}

// Returns a constant iterator to the first element of the view 
string_view::const_iterator 
string_view::
cbegin() 
const noexcept
{
    return const_iterator(_first);
}

// Returns an iterator to the last element of the view
string_view::const_iterator 
string_view::
end() 
const noexcept
{
    return const_iterator(_last);
}

// Returns a constant iterator to the last element of the view 
string_view::const_iterator 
string_view::
cend() 
const noexcept
{
    return const_iterator(_last);
}

// Returns an iterator to the first element of the reversed view 
string_view::const_reverse_iterator 
string_view::
rbegin() 
const noexcept
{
    return const_reverse_iterator(_last);
}

// Returns a constant iterator to the first element of the reversed view 
string_view::const_reverse_iterator 
string_view::
crbegin() 
const noexcept
{
    return const_reverse_iterator(_last);
}

// Returns an iterator to the last element of the reversed view 
string_view::const_reverse_iterator 
string_view::
rend() 
const noexcept
{
    return const_reverse_iterator(_first);
}

// Returns a constant iterator to the last element of the reversed view 
string_view::const_reverse_iterator 
string_view::
crend() 
const noexcept
{
    return const_reverse_iterator(_first);
}
// -------------------------------------------------------------------------- //



// ---------------------- STRING VIEW: ELEMENT ACCESS ----------------------- //
// Accesses the specified element with bounds checking
string_view::const_reference
string_view::
at(size_type pos) 
const
{
    if (!(_first + pos < _last)) {
        throw std::out_of_range("at");
    }
    return _first[pos];
}

// Accesses the specified element  
string_view::const_reference
string_view::
operator[](size_type pos) 
const
{
    return _first[(assert(_first + pos < _last), pos)];
}

// Accesses the first element  
string_view::const_reference
string_view::
front() 
const
{
    return *_first;
}

// Accesses the last element 
string_view::const_reference
string_view::
back() 
const
{
    return *(_last-1);
}

// Returns a pointer to the first element of the view  
string_view::const_pointer
string_view::
data() 
const
{
    return &*_first;
}
// -------------------------------------------------------------------------- //



// ------------------------- STRING VIEW: CAPACITY -------------------------- //
// Returns the number of elements 
string_view::size_type
string_view::
size() 
const noexcept
{
    return _last - _first;
}

// Returns the maximum number of elements  
string_view::size_type
string_view::
max_size() 
const noexcept
{
    return std::array<value_type, 0>().max_size();
}

// Checks whether the view is empty  
bool 
string_view::
empty() 
const noexcept
{
    return !(_first < _last);
}
// -------------------------------------------------------------------------- //



// ------------------------ STRING VIEW: PARTITIONING ----------------------- //
// Splits and strips the view using non printable or specified characters
std::vector<string_view> 
string_view::
split(const string_type& str) 
const
{
    using type = const_reference;
    auto is_graph = [](type e){return std::isgraph(e);};
    auto is_not_excluded = [&str](type e){return str.find(e) == str.npos;};
    auto first = begin();
    auto last = first;
    std::vector<string_view> vector;
    if (str.empty()) {
        while (last < end()) {
            first = std::find_if(first, end(), is_graph);
            last = std::find_if_not(first, end(), is_graph);
            if (first < last) {
                vector.emplace_back(first, last);
            }
            first = last;
        }
    } else {
        while (last < end()) {
            first = std::find_if(first, end(), is_not_excluded);
            last = std::find_if_not(first, end(), is_not_excluded);
            if (first < last) {
                vector.emplace_back(first, last);
            }
            first = last;
        }
    }
    return vector;
}

// Divides the view in three elements using the provided string
std::array<string_view, 3> 
string_view::
partition(const string_type& str) 
const
{
    auto left = std::search(begin(), end(), str.begin(), str.end());
    auto right = std::min(end(), left + str.size());
    auto current = string_view(left, right);
    auto before = string_view(begin(), current.begin());
    auto after = string_view(current.end(), end());
    return {before, current, after};
}

// Divides the reversed view in three elements using the provided string
std::array<string_view, 3> 
string_view::
rpartition(const string_type& str) 
const
{
    auto right = std::search(rbegin(), rend(), str.rbegin(), str.rend()).base();
    auto left = std::max(begin(), right - str.size());
    auto current = string_view(left, right);
    auto before = string_view(begin(), current.begin());
    auto after = string_view(current.end(), end());
    return {before, current, after};
}
// -------------------------------------------------------------------------- //



// ------------------------- STRING VIEW: STRIPPING ------------------------- //
// Left strips the view from non printable characters
string_view 
string_view::
lstrip() 
const
{
    auto is_graph = [](value_type c){return std::isgraph(c);};
    auto left = std::find_if(begin(), end(), is_graph);
    return string_view(left, end());
}

// Left strips the view from the specified character
string_view 
string_view::
lstrip(value_type x) 
const
{
    auto is_not_excluded = [=](value_type c){return c != x;};
    auto left = std::find_if(begin(), end(), is_not_excluded);
    return string_view(left, end());
}

// Left strips the view from all the specified characters
template <class... T> 
string_view 
string_view::
lstrip(T... xn) 
const
{
    auto is_not_excluded = [=](value_type c){return !equal_to_any(c, xn...);};
    auto left = std::find_if(begin(), end(), is_not_excluded);
    return string_view(left, end());
}

// Left strips the view when a predicate is true
template <class F> 
string_view 
string_view::
lstrip(F f) 
const
{
    auto not_f = [=](value_type c){return !f(c);};
    auto left = std::find_if(begin(), end(), not_f);
    return string_view(left, end());
}

// Right strips the view from non printable characters
string_view 
string_view::
rstrip() 
const
{
    auto is_graph = [](value_type c){return std::isgraph(c);};
    auto right = std::find_if(rbegin(), rend(), is_graph).base();
    return string_view(begin(), right);
}

// Right strips the view from the specified character
string_view 
string_view::
rstrip(value_type x) 
const
{
    auto is_not_excluded = [=](value_type c){return c != x;};
    auto right = std::find_if(rbegin(), rend(), is_not_excluded).base();
    return string_view(begin(), right);
}

// Right strips the view from all the specified characters
template <class... T> 
string_view 
string_view::
rstrip(T... xn) 
const
{
    auto is_not_excluded = [=](value_type c){return !equal_to_any(c, xn...);};
    auto right = std::find_if(rbegin(), rend(), is_not_excluded).base();
    return string_view(begin(), right);
}

// Right strips the view when a predicate is true
template <class F> 
string_view 
string_view::
rstrip(F f) 
const
{
    auto not_f = [=](value_type c){return !f(c);};
    auto right = std::find_if(rbegin(), rend(), not_f).base();
    return string_view(begin(), right);
}

// Left and right strips the view from non printable characters
string_view 
string_view::
strip() 
const
{
    auto is_graph = [](value_type c){return std::isgraph(c);};
    auto left = std::find_if(begin(), end(), is_graph);
    auto right = std::find_if(rbegin(), rend(), is_graph).base();
    return string_view(std::min(left, right), right);
}

// Left and right strips the view from the specified character
string_view 
string_view::
strip(value_type x) 
const
{
    auto is_not_excluded = [=](value_type c){return c != x;};
    auto left = std::find_if(begin(), end(), is_not_excluded);
    auto right = std::find_if(rbegin(), rend(), is_not_excluded).base();
    return string_view(std::min(left, right), right);
}

// Left and right strips the view from all the specified characters
template <class... T> 
string_view 
string_view::
strip(T... xn) 
const
{
    auto is_not_excluded = [=](value_type c){return !equal_to_any(c, xn...);};
    auto left = std::find_if(begin(), end(), is_not_excluded);
    auto right = std::find_if(rbegin(), rend(), is_not_excluded).base();
    return string_view(std::min(left, right), right);
}

// Left and right strips the view when a predicate is true
template <class F> 
string_view 
string_view::
strip(F f) 
const
{
    auto not_f = [=](value_type c){return !f(c);};
    auto left = std::find_if(begin(), end(), not_f);
    auto right = std::find_if(rbegin(), rend(), not_f).base();
    return string_view(std::min(left, right), right);
}
// -------------------------------------------------------------------------- //



// ------------------------- STRING VIEW: OPERATORS ------------------------- //
// Exchanges the view with another one
void 
string_view::
swap(string_view& other)
noexcept
{
    const string_view tmp = other;
    other = *this;
    *this = tmp;
}

// Creates a string from the view 
string_view::string_type
string_view::
to_string()
const
{
    return string_type(begin(), end());
}
    
// Compares all elements of the two views and checks for equality
bool 
string_view::
operator==(string_view other) 
const
{
    return size() == other.size() && std::equal(begin(), end(), other.begin());
}

// Compares all elements of the two views and checks for differences
bool 
string_view::
operator!=(string_view other) 
const
{
    return size() != other.size() || !std::equal(begin(), end(), other.begin());
}
// -------------------------------------------------------------------------- //



// ------------------------- STRING VIEW: STREAMING ------------------------- //
// Writes the string view to an output stream
std::ostream& 
operator<<(std::ostream &os, string_view v)
{
    return os.write(v.data(), v.size());
}
// -------------------------------------------------------------------------- //



// -------------------------------- HELPERS --------------------------------- //
// Checks if a variable is equal to any of a set of variables: empty version
template <class T> 
constexpr 
bool 
equal_to_any(T&& x)
{
    return false;
}

// Checks if a variable is equal to any of a set of variables: normal version
template <class T, class T0> 
constexpr 
bool 
equal_to_any(T&& x, T0&& x0)
{
    return std::forward<T>(x) == std::forward<T0>(x0);
}

// Checks if a variable is equal to any of a set of variables: variadic version
template <class T, class T0, class... TN> 
constexpr 
bool 
equal_to_any(T&& x, T0&& x0, TN&&... xn)
{
    return equal_to_any(std::forward<T>(x), std::forward<T0>(x0))
        || equal_to_any(std::forward<T>(x), std::forward<TN>(xn)...);
}

// Checks if a variable is equal to every variables of a set: empty version
template <class T> 
constexpr 
bool 
equal_to_all(T&& x)
{
    return true;
}

// Checks if a variable is equal to every variables of a set: normal version
template <class T, class T0> 
constexpr 
bool 
equal_to_all(T&& x, T0&& x0)
{
    return std::forward<T>(x) == std::forward<T0>(x0);
}

// Checks if a variable is equal to every variables of a set: variadic version
template <class T, class T0, class... TN> 
constexpr 
bool 
equal_to_all(T&& x, T0&& x0, TN&&... xn)
{
    return equal_to_all(std::forward<T>(x), std::forward<T0>(x0))
        && equal_to_all(std::forward<T>(x), std::forward<TN>(xn)...);
}
// -------------------------------------------------------------------------- //



// ========================================================================== //
} // namespace epidemium_oncobase
#endif // _STRING_VIEW_HPP_INCLUDED
// ========================================================================== //
