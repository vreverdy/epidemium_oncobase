// ================================= TABLE ================================== //
// Project:         epidemium_oncobase
// Name:            table.hpp
// Description:     A two dimensional table of data
// Creator:         Vincent Reverdy
// Contributor(s):  Vincent Reverdy [2015-2016]
// License:         GNU GPLv3
// ========================================================================== //
/** 
 * table.hpp
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
#ifndef _TABLE_HPP_INCLUDED
#define _TABLE_HPP_INCLUDED
// ========================================================================== //



// ============================== PREPROCESSOR ============================== //
// Include C++
#include <map>
#include <cctype>
#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <algorithm>
#include <stdexcept>
// Include others
// Miscellaneous
namespace epidemium_oncobase {
// ========================================================================== //



/* ********************************* TABLE ********************************** */
// Table class definition
template <class T>
class table
{
    // Types
    using value_type = T;
    using size_type = std::size_t;
    
    // Lifecycle
    public:
    table();
    table(table&& other) = default;
    table(size_type nrows, size_type mcolumns);

    // Assignment
    public:
    table& operator=(table&& other) = default;
    
    // Access
    public:
    T& at(size_type irow, size_type jcolumn);
    const T& at(size_type irow, size_type jcolumn) const;
    void title(const std::string& name);
    std::string& title();
    const std::string& title() const;
    void row(size_type irow, const std::string& name);
    std::string& row(size_type irow);
    const std::string& row(size_type irow) const;
    void column(size_type jcolumn, const std::string& name);
    std::string& column(size_type jcolumn);
    const std::string& column(size_type jcolumn) const;
    void description(const std::string& text);
    std::string& description();
    const std::string& description() const;

    // Capacity
    public:
    std::size_t row_count() const noexcept;
    std::size_t column_count() const noexcept;
    std::size_t max_size() const;
    std::pair<std::size_t, std::size_t> size() const noexcept;
    std::pair<std::size_t, std::size_t> capacity() const noexcept;
    void resize(size_type nrows, size_type mcolumns);
    void reserve(size_type nrows, size_type mcolumns);
    void shrink_to_fit();
    
    // Implementation details: data members
    private:
    std::string _title;
    std::vector<std::string> _rows;
    std::vector<std::string> _columns;
    std::vector<T> _contents;
    std::string _description;
};
/* ************************************************************************** */



// ---------------------------- TABLE: LIFECYCLE ---------------------------- //
// Constructs an empty table
template <class T>
table<T>::
table() 
: _title()
, _rows()
, _columns()
, _contents()
, _description()
{
}

// Constructs a table from a number of rows and a number of columns
template <class T>
table<T>::
table(size_type nrows, size_type mcolumns)
: _title()
, _rows(nrows)
, _columns(mcolumns)
, _contents(nrows * mcolumns)
, _description()
{
}
// -------------------------------------------------------------------------- //



// ----------------------------- TABLE: ACCESS ------------------------------ //
// Returns the element at the given row and column
template <class T> 
T& 
table<T>::
at(size_type irow, size_type jcolumn)
{
    return _contents.at(irow * _columns.size() + jcolumn);
}

// Returns the constant element at the given row and column
template <class T> 
const T& 
table<T>::
at(size_type irow, size_type jcolumn) 
const
{
    return _contents.at(irow * _columns.size() + jcolumn);
}

// Sets the title of the table
template <class T> 
void 
table<T>::
title(const std::string& name)
{
    _title = name;
}

// Returns the title of the table
template <class T> 
std::string& 
table<T>::
title()
{
    return _title;
}

// Returns the constant title of the table
template <class T> 
const std::string& 
table<T>::
title() 
const
{
    return _title;
}

// Sets the name of the given row
template <class T> 
void 
table<T>::
row(size_type irow, const std::string& name)
{
    _columns.at(irow) = name;
}

// Returns the name of the given row
template <class T> 
std::string& 
table<T>::
row(size_type irow)
{
    return _rows.at(irow);
}

// Returns the constant name of the given row
template <class T> 
const std::string& 
table<T>::
row(size_type irow) 
const
{
    return _rows.at(irow);
}

// Sets the name of the given column
template <class T> 
void 
table<T>::
column(size_type jcolumn, const std::string& name)
{
    _columns.at(jcolumn) = name;
}

// Returns the name of the given column
template <class T> 
std::string& 
table<T>::
column(size_type jcolumn)
{
    return _columns.at(jcolumn);
}

// Returns the constant name of the given column
template <class T> 
const std::string& 
table<T>::
column(size_type jcolumn) 
const
{
    return _columns.at(jcolumn);
}

// Sets the description of the table
template <class T> 
void 
table<T>::
description(const std::string& text)
{
    _description = text;
}

// Returns the description of the table
template <class T> 
std::string& 
table<T>::
description()
{
    return _description;
}

// Returns the constant description of the table
template <class T> 
const std::string& 
table<T>::
description() 
const
{
    return _description;
}
// -------------------------------------------------------------------------- //



// ---------------------------- TABLE: CAPACITY ----------------------------- //
// Returns the number of rows
template <class T> 
std::size_t 
table<T>::
row_count() 
const noexcept
{
    return _rows.size();
}

// Returns the number of columns
template <class T> 
std::size_t
table<T>::
column_count() 
const noexcept
{
    return _columns.size();
}

// Returns the maximum allocatable size
template <class T> 
std::size_t
table<T>::
max_size() 
const
{
    return _contents.max_size();
}

// Returns the row and column count
template <class T> 
std::pair<std::size_t, std::size_t>
table<T>::
size()
const noexcept
{
    return std::make_pair(_rows.size(), _columns.size());
}

// Returns the current row and column capacity
template <class T> 
std::pair<std::size_t, std::size_t>
table<T>::
capacity()
const noexcept
{
    return std::make_pair(_rows.capacity(), _columns.capacity());
}

// Resize the table
template <class T>
void
table<T>::
resize(size_type nrows, size_type mcolumns)
{
    std::vector<T> new_contents;
    if (mcolumns != _columns.size()) {
        new_contents.resize(nrows * mcolumns);
        for (size_type irow = 0; irow < _rows.size(); ++irow) {
            for (size_type jcolumn = 0; jcolumn < _columns.size(); ++jcolumn) {
                new_contents[irow * mcolumns + jcolumn] = 
                    _contents[irow * _columns.size() + jcolumn];
            }
        }
        _contents = std::move(new_contents);
        _columns.resize(mcolumns);
    } else {
        _contents.resize(nrows * mcolumns);
    }
    _rows.resize(nrows);
}

// Reserves new space for the contents
template <class T>
void
table<T>::
reserve(size_type nrows, size_type mcolumns)
{
    _rows.reserve(nrows);
    _columns.reserve(mcolumns);
    _contents.reserve(nrows * mcolumns);
}

// Shrinks allocated space to fit table size
template <class T>
void 
table<T>::
shrink_to_fit()
{
    _rows.shrink_to_fit();
    _columns.shrink_to_fit();
    _contents.shrink_to_fit();
}
// -------------------------------------------------------------------------- //



// ========================================================================== //
} // namespace epidemium_oncobase
#endif // _TABLE_HPP_INCLUDED
// ========================================================================== //
