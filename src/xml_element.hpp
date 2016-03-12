// =============================== XML ELEMENT ============================== //
// Project:         epidemium_oncobase
// Name:            xml_element.hpp
// Description:     A xml element of a tree to parse nxml files
// Creator:         Vincent Reverdy
// Contributor(s):  Vincent Reverdy [2015-2016]
// License:         GNU GPLv3
// ========================================================================== //
/** 
 * xml_element.hpp
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
#ifndef _XML_ELEMENT_HPP_INCLUDED
#define _XML_ELEMENT_HPP_INCLUDED
// ========================================================================== //



// ============================== PREPROCESSOR ============================== //
// Include C++
#include <map>
#include <cctype>
#include <string>
#include <vector>
// Include others
#include "string_view.hpp"
// Miscellaneous
namespace epidemium_oncobase {
// ========================================================================== //



/* ******************************* XML ELEMENT ****************************** */
// Xml element class definition
class xml_element
{
    // Types
    public:
    using string_type = std::string;
    using view_type = string_view;
    using value_type = xml_element;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = value_type&;
    using const_reference = const reference;
    using pointer = value_type*;
    using const_pointer = const pointer;
    
    // Lifecycle
    explicit xml_element(view_type v = view_type(), pointer p = nullptr);
    ~xml_element();
    
    // Implementation details: data members
    private:
    pointer _parent;
    std::vector<pointer> _children;
    view_type _tag;
    std::map<view_type, view_type> _attributes;
    view_type _content;
};
/* ************************************************************************** */



// ------------------------- XML ELEMENT: LIFECYCLE ------------------------- //
// Constructs the xml element from a xml doc
xml_element::
xml_element(view_type v, pointer p)
: _parent(p)
, _children()
, _tag()
, _attributes()
, _content(v)
{
    view_type w;
    view_type x;
    v = v.lstrip([](auto c){return std::isspace(c) && !std::isprint(c);});
    if (v[0] == '<' && v.size() > 1) {
        v.
        v = v.lstrip([](auto c){return std::isspace(c);});
        w = v.lstrip([](auto c){return std::isgraph(c);});
        x = view_type(std::begin(v), std::begin(w));
        if (std::isalpha(w[0])) {
            _tag = x;
            for (auto&& c: w) {
                
            }
        }
    } else {
    }
}

// Deletes the xml element and its children
xml_element::
~xml_element()
{
    for (auto&& p: _children) {
        delete p;
    }
}
// -------------------------------------------------------------------------- //



// ========================================================================== //
} // namespace epidemium_oncobase
#endif // _XML_ELEMENT_HPP_INCLUDED
// ========================================================================== //
