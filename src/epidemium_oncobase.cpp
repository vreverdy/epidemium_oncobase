// =========================== EPIDEMIUM ONCOBASE =========================== //
// Project:         epidemium_oncobase
// Name:            epidemium_oncobase.cpp
// Description:     Main program (test and debug at this point)
// Creator:         Vincent Reverdy
// Contributor(s):  Vincent Reverdy [2015-2016]
// License:         GNU GPLv3
// ========================================================================== //
/** 
 * epidemium_oncobase.cpp
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
// Compilation:     g++ -std=c++14 -Wall -Wextra -pedantic -g -O3 epidemium_
//                  oncobase.cpp -o epidemium_oncobase -lstdc++fs -lpthread
// ========================================================================== //



// ============================== PREPROCESSOR ============================== //
// Include C++
#include <cctype>
#include <vector>
#include <iostream>
#include <algorithm>
// Include others
#include "file.hpp"
#include "table.hpp"
#include "article.hpp"
#include "ftp_manager.hpp"
#include "string_view.hpp"
// Miscellaneous
using namespace epidemium_oncobase;
// ========================================================================== //



// -------------------------- CONTAINER OPERATIONS -------------------------- //
// Finds a value in a container
template <class C, class T>
bool contains(C&& container, T&& value)
{
    using container_type = typename std::decay<C>::type;
    using value_type = typename container_type::value_type;
    value_type x(std::forward<T>(value));
    auto begin = std::begin(std::forward<C>(container));
    auto end = std::end(std::forward<C>(container));
    return std::find(begin, end, x) != end;
}

// Finds a value in a container of pairs, for the first type
template <class C, class T>
bool contains_first(C&& container, const T& value)
{
    using container_type = typename std::decay<C>::type;
    using value_type = typename container_type::value_type;
    auto same = [&value](const value_type& p){return p.first == value;};
    auto begin = std::begin(std::forward<C>(container));
    auto end = std::end(std::forward<C>(container));
    return std::find_if(begin, end, same) != end;
}

// Finds a value in a container of pairs, for the second type
template <class C, class T, class X>
bool contains_second(C&& container, const T& value)
{
    using container_type = typename std::decay<C>::type;
    using value_type = typename container_type::value_type;
    auto same = [&value](const value_type& p){return p.second == value;};
    auto begin = std::begin(std::forward<C>(container));
    auto end = std::end(std::forward<C>(container));
    return std::find_if(begin, end, same) != end;
}

// Sort by first member
template <class C>
void sort_by_first_member(C&& container, bool reverse = false)
{
    auto comparator = [](const auto& x, const auto& y){
        bool less = x.first < y.first;
        return less || (x.first == y.first && x.second < y.second);
    };
    auto begin = std::begin(std::forward<C>(container));
    auto end = std::end(std::forward<C>(container));
    std::sort(begin, end, comparator);
    if (reverse) {
        std::reverse(begin, end);
    }
}

// Sort by second member
template <class C>
void sort_by_second_member(C&& container, bool reverse = false)
{
    auto comparator = [](const auto& x, const auto& y){
        bool less = x.second < y.second;
        return less || (x.second == y.second && x.first < y.first);
    };
    auto begin = std::begin(std::forward<C>(container));
    auto end = std::end(std::forward<C>(container));
    std::sort(begin, end, comparator);
    if (reverse) {
        std::reverse(begin, end);
    }
}
// -------------------------------------------------------------------------- //



/* ********************************** MAIN ********************************** */
// Test and debug
int main(int argc, char** argv)
{
    // Types
    using word_distribution = article::word_distribution;
    
    // Constants
    static const std::string nullstr = std::string();
    static const std::string cancer = "cancer";
    std::vector<std::string> cancer_words = {
        "breast", "treatment", "carcinoma", "chemotherapy", "colorectal", 
        "ovarian", "gastric", "doxorubicin", "cytoplasmic", "gemcitabine", 
        "carboplatin", "fibroblasts", "irinotecan", "macrophages", "arm",
        "peptide", "intracellular", "papillomavirus", "pregnancy", "calcium",
        "lung", "serum", "prostate", "melanoma", "renal"
    };
    
    // Variables
    const std::string pubmed = argc > 1 ? std::string(argv[1]) : nullstr;
    const std::string dictionary = argc > 2 ? std::string(argv[2]) : nullstr;
    auto filter = [](auto&& p){return p.extension() == ".txt";};
    auto articles = file(pubmed).get_recursive_contents(filter);
    auto medical_dictionary = file(dictionary).read_wide();
    auto view = string_view(medical_dictionary);
    std::vector<string_view> words = view.split("\n");
    auto rem = [=](auto&& w){
        return std::any_of(std::begin(w), std::end(w), [](auto&& c){
            return (std::isupper(c) || std::isdigit(c));
        });
    };
    auto word_it = std::remove_if(std::begin(words), std::end(words), rem);
    std::vector<std::string> medical_words(std::begin(words), word_it);
    std::size_t medical_size = medical_words.size();
    word_distribution input_distribution;
    word_distribution output_distribution;
    article paper;
    std::size_t total = 0;
    std::size_t count = 0;
    std::size_t i = 0;
    std::map<std::string, std::size_t> map;
    std::map<std::string, std::map<std::string, std::size_t>> word_map;
    
    // Produces the medical dictionary
    std::sort(std::begin(medical_words), std::end(medical_words));
    
    // Prepares the word map
    for (auto&& word1: cancer_words) {
        for (auto&& word2: cancer_words) {
            word_map[word1][word2] = 0;
        }
    }
    
    // Loops over articles
    for (const auto& f: articles) {
        std::cout<<count<<" "<<std::string(f.get_absolute_path())<<std::endl;
        i = 0;
        paper.load(std::string(f.get_absolute_path()));
        input_distribution = paper.compute_word_distribution();
        std::sort(std::begin(input_distribution), std::end(input_distribution));
        for (auto&& input: input_distribution) {
            if (input.second > 3) {
                while (i < medical_size && medical_words[i] < input.first) {
                    ++i;
                }
                if (i < medical_words.size()) {
                    if (input.first == medical_words[i]) {
                        output_distribution.push_back(input);
                    }
                } else {
                    break;
                }
            }
        }
        sort_by_second_member(output_distribution, true);
        if (contains_first(output_distribution, cancer)) {
            for (auto&& item: output_distribution) {
                if (map.find(item.first) == std::end(map)) {
                    map[item.first] = item.second;
                } else {
                    map[item.first] += item.second;
                }
            }
            for (auto&& word1: cancer_words) {
                if (contains_first(output_distribution, word1)) {
                    for (auto&& word2: cancer_words) {
                        if (contains_first(output_distribution, word2)) {
                            word_map[word1][word2] += 1;
                        }
                    }
                }
            }
            ++count;
        }
        paper.clear();
        output_distribution.clear();
        ++total;
    }
    for (auto&& item: map) {
        output_distribution.push_back(item);
    }
    sort_by_second_member(output_distribution);
    std::cout<<"========================================"<<std::endl;
    for (auto&& item: output_distribution) {
        std::cout<<item.first<<" "<<item.second<<std::endl;
    }
    std::cout<<"========================================"<<std::endl;
    std::cout<<count<<" "<<total<<std::endl;
    std::cout<<"========================================"<<std::endl;
    for (auto&& word1: cancer_words) {
        for (auto&& word2: cancer_words) {
            std::cout<<word1<<" "<<word2<<" "<<word_map[word1][word2];
            std::cout<<std::endl;
        }
    }
    return 0;
}
/* ************************************************************************** */
