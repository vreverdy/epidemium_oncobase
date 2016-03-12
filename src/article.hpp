// ================================ ARTICLE ================================= //
// Project:         epidemium_oncobase
// Name:            article.hpp
// Description:     A scientific article
// Creator:         Vincent Reverdy
// Contributor(s):  Vincent Reverdy [2015-2016]
// License:         GNU GPLv3
// ========================================================================== //
/** 
 * article.hpp
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
#ifndef _ARTICLE_HPP_INCLUDED
#define _ARTICLE_HPP_INCLUDED
// ========================================================================== //



// ============================== PREPROCESSOR ============================== //
// Include C++
#include <map>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
// Include others
#include "file.hpp"
// Miscellaneous
namespace epidemium_oncobase {
// ========================================================================== //



/* ******************************** ARTICLE ********************************* */
// Article class definition
class article
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
    using word_distribution = std::vector<std::pair<string_type, std::size_t>>;
    
    // Lifecycle
    public:
    explicit article(const std::string& filename = "");

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
    
    // Management
    public:
    void load(const std::string& filename = "");
    void clear();
    
    // Algorithms
    public:
    word_distribution compute_word_distribution();
    
    // Streaming
    public:
    friend std::ostream& operator<<(std::ostream &os, const article& a);
    
    // Implementation details: data members
    private:
    std::string _text;
    file _file;
};
/* ************************************************************************** */



// --------------------------- ARTICLE: LIFECYCLE --------------------------- //
// Constructs an article without loading the data
article::
article(const std::string& filename)
: _text()
, _file(filename)
{
}
// -------------------------------------------------------------------------- //



// --------------------------- ARTICLE: ITERATORS --------------------------- //
// Returns an iterator to the first element of the text 
article::const_iterator 
article::
begin() 
const noexcept
{
    return _text.cbegin();
}

// Returns a constant iterator to the first element of the text 
article::const_iterator 
article::
cbegin() 
const noexcept
{
    return _text.cbegin();
}

// Returns an iterator to the last element of the text
article::const_iterator 
article::
end() 
const noexcept
{
    return _text.cend();
}

// Returns a constant iterator to the last element of the text 
article::const_iterator 
article::
cend() 
const noexcept
{
    return _text.cend();
}

// Returns an iterator to the first element of the reversed text 
article::const_reverse_iterator 
article::
rbegin() 
const noexcept
{
    return _text.crbegin();
}

// Returns a constant iterator to the first element of the reversed text 
article::const_reverse_iterator 
article::
crbegin() 
const noexcept
{
    return _text.crbegin();
}

// Returns an iterator to the last element of the reversed text 
article::const_reverse_iterator 
article::
rend() 
const noexcept
{
    return _text.crend();
}

// Returns a constant iterator to the last element of the reversed text 
article::const_reverse_iterator 
article::
crend() 
const noexcept
{
    return _text.crend();
}
// -------------------------------------------------------------------------- //



// --------------------------- ARTICLE: MANAGEMENT -------------------------- //
// Loads the current file or a new file
void
article::
load(const std::string& filename)
{
    if (filename.size()) {
        _file = file(filename);
        if (_file.get_existence()) {
            _file = file(_file.get_absolute_path());
        }
    }
    if (_file.get_existence()) {
        if (_file.extension() == ".txt") {
            _text = _file.read_wide();
        } else if (_file.extension() == ".nxml") {
            _text = _file.read_wide();
        }
    }
}

// Clears the current contents
void
article::
clear()
{
    _text.clear();
    _text.shrink_to_fit();
}
// -------------------------------------------------------------------------- //



// --------------------------- ARTICLE: ALGORITHMS -------------------------- //
// Computes the word distribution in the article
article::word_distribution
article::
compute_word_distribution()
{
    using pair = word_distribution::value_type;
    using associative_container = std::map<pair::first_type, pair::second_type>;
    auto sorter = [](const pair& p, const pair& q){return p.second > q.second;};
    auto finder = [](char c){return std::isspace(c) || std::iscntrl(c);};
    auto punct = [](char c){return std::ispunct(c);};
    auto low = [](char c){return std::tolower(c);};
    auto first = begin();
    auto last = first;
    auto rfirst = rbegin();
    auto rlast = rfirst;
    associative_container map;
    word_distribution distribution;
    std::string w;
    while (last < end()) {
        first = std::find_if_not(last, end(), finder);
        last = std::find_if(first, end(), finder);
        if (first < last) {
            first = std::find_if_not(first, last, punct);
            rfirst = std::reverse_iterator<decltype(last)>(last);
            rlast = std::reverse_iterator<decltype(first)>(first);
            rfirst = std::find_if_not(rfirst, rlast, punct);
            last = rfirst.base();
            if (first < last) {
                w = std::string(first, last);
                std::transform(std::begin(w), std::end(w), std::begin(w), low);
                ++map[w];
            }
        }
    }
    distribution.reserve(map.size());
    for (auto it = std::begin(map); it != std::end(map); ++it) {
        distribution.emplace_back(*it);
    }
    std::sort(std::begin(distribution), std::end(distribution), sorter);
    return distribution;
}
// -------------------------------------------------------------------------- //



// --------------------------- ARTICLE: STREAMING --------------------------- //
// Writes the text of the article to an output stream
std::ostream& 
operator<<(std::ostream &os, const article& a)
{
    return os << a._text;
}
// -------------------------------------------------------------------------- //



// ========================================================================== //
} // namespace epidemium_oncobase
#endif // _ARTICLE_HPP_INCLUDED
// ========================================================================== //
