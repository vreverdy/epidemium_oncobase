// ================================== FILE ================================== //
// Project:         epidemium_oncobase
// Name:            file.hpp
// Description:     A file and its properties
// Creator:         Vincent Reverdy
// Contributor(s):  Vincent Reverdy [2015-2016]
// License:         GNU GPLv3
// ========================================================================== //
/** 
 * file.hpp
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
#ifndef _FILE_HPP_INCLUDED
#define _FILE_HPP_INCLUDED
// ========================================================================== //



// ============================== PREPROCESSOR ============================== //
// Include C++
#include <regex>
#include <cctype>
#include <chrono>
#include <random>
#include <string>
#include <fstream>
#include <sstream>
#include <utility>
#include <iostream>
#include <experimental/filesystem>
// Include others
// Miscellaneous
namespace epidemium_oncobase {
// ========================================================================== //



/* ********************************** FILE ********************************** */
// File class definition
class file
{
    // Types
    public:
    using byte_type = unsigned char;
    using path_type = std::experimental::filesystem::path;
    using file_type = std::experimental::filesystem::file_type;
    using perms_type = std::experimental::filesystem::perms;
    using size_type = std::uintmax_t;
    using time_type = std::experimental::filesystem::file_time_type;
    using string_type = std::string;
    using binary_type = std::vector<char>;
    using facet_type = std::codecvt_utf8<wchar_t>;
    using copy_type = std::experimental::filesystem::copy_options;
    
    // Constants
    public:
    static constexpr size_type bits = std::numeric_limits<byte_type>::digits;
    static constexpr size_type buffer_size = 1 << bits;
    static constexpr size_type reserve = 1 << 20;
    static constexpr file_type regular = file_type::regular;
    static constexpr file_type directory = file_type::directory;
    static constexpr copy_type skip = copy_type::skip_existing;
    static constexpr auto time_fmt = "%Y-%m-%d-%H-%M-%S";
    
    // Lifecycle
    public:
    file(file&& other) = default;
    explicit file(path_type p);
    file(path_type p, size_type s, time_type t, file_type f = regular);
    file(path_type p, size_type s, string_type t = "", file_type f = regular);

    // Assignment
    public:
    file& operator=(file&& other) = default;
    
    // Existing properties
    public:
    path_type get_current_path() const;
    path_type get_absolute_path() const;
    path_type get_relative_path() const;
    bool get_existence() const;
    file_type get_type() const;
    perms_type get_permissions() const;
    size_type get_size() const;
    time_type get_time() const;
    string_type get_time_string(string_type fmt = time_fmt) const;
    
    // Internal properties
    public:
    path_type path() const;
    file_type type() const;
    size_type size() const;
    time_type time() const;
    string_type time_string(string_type fmt = time_fmt) const;
    
    // Input and output
    public:
    string_type read() const;
    string_type read_wide() const;
    binary_type read_binary() const;
    file create(const string_type& data = string_type(), copy_type copy = skip);
    file remove();
    
    // File types
    public:
    bool is_block_file() const;
    bool is_character_file() const;
    bool is_directory() const;
    bool is_empty() const;
    bool is_fifo() const;
    bool is_other() const;
    bool is_regular_file() const;
    bool is_socket() const;
    bool is_symlink() const;
    
    // Components
    public:
    string_type root() const;
    string_type parent() const;
    string_type filename() const;
    string_type stem() const;
    string_type extension() const;
    
    // Directory
    public:
    std::vector<file> get_contents() const;
    template <class F> std::vector<file> get_contents(F&& f) const;
    std::vector<file> get_recursive_contents() const;
    template <class F> std::vector<file> get_recursive_contents(F&& f) const;
    
    // Factories
    public:
    static file make_temporary(const string_type& extension = "");
    
    // Implementation details: data members
    private:
    path_type _path;
    file_type _type;
    size_type _size;
    time_type _time;
};
/* ************************************************************************** */



// ---------------------------- FILE: LIFECYCLE ----------------------------- //
// Constructs a file from a path
file::
file(path_type p) 
: _path(p)
, _type()
, _size()
, _time()
{
    if (std::experimental::filesystem::exists(p)) {
        _type = std::experimental::filesystem::status(p).type();
        if (_type != directory) {
            _size = std::experimental::filesystem::file_size(p);
        }
        _time = std::experimental::filesystem::last_write_time(p);
    }
}

// Constructs file properties from a path, a size, a timestamp and a type
file::
file(path_type p, size_type s, time_type t, file_type f)
: _path(p)
, _type(f)
, _size(s)
, _time(t)
{
}

// Constructs file properties from a path, a size, a time string and a type
file::
file(path_type p, size_type s, string_type t, file_type f)
: _path(p)
, _type(f)
, _size(s)
, _time(time_type::clock::now())
{
    auto lower = [](char c){return std::tolower(c);};
    tm time = tm();
    size_type npos = string_type::npos;
    string_type::iterator begin = std::begin(t);
    string_type::iterator end = std::end(t);
    std::regex regex;
    std::sregex_iterator regex_begin = std::sregex_iterator();
    std::sregex_iterator regex_end = regex_begin;
    std::transform(begin, end, begin, lower);
    string_type tmp;
    int year = -1;
    int month = -1;
    int day = -1;
    int hour = -1;
    int min = -1;
    int sec = -1; 
    std::chrono::system_clock::time_point point;
    if (t.size()) {
        regex = std::regex("(\\d{4}-\\d+-\\d+-\\d+-\\+-\\d+)");
        regex_begin = std::sregex_iterator(begin, end, regex);
        for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
            tmp = (*i).str();
            std::sscanf(tmp.data(), "%d-%d-%d-%d-%d-%d", 
                        &year, &month, &day, &hour, &min, &sec);
        }
        if (year < 0) {
            regex = std::regex("(\\d{4}/\\d+/\\d+/\\d+/\\+/\\d+)");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                tmp = (*i).str();
                std::sscanf(tmp.data(), "%d/%d/%d/%d/%d/%d", 
                            &year, &month, &day, &hour, &min, &sec);
            }
        }
        if (year < 0) {
            regex = std::regex("(\\d{4}-\\d+-\\d+)");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) {
                tmp = (*i).str();
                std::sscanf(tmp.data(), "%d-%d-%d", &year, &month, &day);
            }
        }
        if (year < 0) {
            regex = std::regex("(\\d+-\\d+-\\d{4})");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                tmp = (*i).str();
                std::sscanf(tmp.data(), "%d-%d-%d", &day, &month, &year);
            }
        }
        if (year < 0) {
            regex = std::regex("(\\d+/\\d+/\\d{4})");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                tmp = (*i).str();
                std::sscanf(tmp.data(), "%d/%d/%d", &day, &month, &year);
            }
        }
        if (year < 0) {
            regex = std::regex("(\\d{4}/\\d+/\\d+)");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                tmp = (*i).str();
                std::sscanf(tmp.data(), "%d/%d/%d", &year, &month, &day);
            }
        }
        if (month < 0) {
            regex = std::regex("(\\d{4})");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                year = std::stoull((*i).str());
            } 
            if (t.find("jan") != npos) {
                month = 1;
            } else if (t.find("feb") != npos || t.find("fev") != npos) {
                month = 2;
            } else if (t.find("mar") != npos || t.find("mar") != npos) {
                month = 3;
            } else if (t.find("apr") != npos || t.find("avr") != npos) {
                month = 4;
            } else if (t.find("may") != npos || t.find("mai") != npos) {
                month = 5;
            } else if (t.find("jun") != npos || t.find("juin") != npos) {
                month = 6;
            } else if (t.find("jul") != npos || t.find("juil") != npos) {
                month = 7;
            } else if (t.find("aug") != npos || t.find("ao") != npos) {
                month = 8;
            } else if (t.find("sep") != npos) {
                month = 9;
            } else if (t.find("oct") != npos) {
                month = 10;
            } else if (t.find("nov") != npos) {
                month = 11;
            } else if (t.find("dec") != npos) {
                month = 12;
            }
        }
        if (month > 0 && day < 0) {
            regex = std::regex(std::string("\\s+((\\d{1,2}(?=st))|(\\d")
                  + "{1,2}(?=nd))|(\\d{1,2}(?=rd))|(\\d{1,2}(?=th)))");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                day = std::stoull((*i).str());
            }
        }
        if (month > 0 && day < 0) {
            regex = std::regex("(\\s+\\d{1,2}\\s+)|(\\s+\\d{1,2},)");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                day = std::stoull((*i).str());
            }
        }
        if (hour < 0) {
            regex = std::regex("(\\d{1,2}:\\d{1,2}:\\d{1,2})");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) {
                tmp = (*i).str();
                std::sscanf(tmp.data(), "%d:%d:%d", &hour, &min, &sec);
            }
        }
        if (hour < 0) {
            regex = std::regex("(\\d{1,2}:\\d{1,2})");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) {
                tmp = (*i).str();
                std::sscanf(tmp.data(), "%d:%d", &hour, &min);
            }
        }
        if (hour >= 0) {
            regex = std::regex("(\\d+\\s*pm\\s*)");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                if (hour >= 1 && hour <= 11) {
                    hour += 12;
                }
            } 
            regex = std::regex("(\\d+\\s*am\\s*)");
            regex_begin = std::sregex_iterator(begin, end, regex);
            for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) { 
                if (hour == 12) {
                    hour -= 12;
                }
            } 
        }
        year >= 0 ? (time.tm_year = year - 1900) : time.tm_year;
        month >= 0 ? (time.tm_mon = month - 1) : time.tm_mon;
        day >= 0 ? (time.tm_mday = day) : time.tm_mday;
        hour >= 0 ? (time.tm_hour = hour) : time.tm_hour;
        min >= 0 ? (time.tm_min = min) : time.tm_min;
        sec >= 0 ? (time.tm_sec = sec) : time.tm_sec;
        _time = time_type::clock::from_time_t(std::mktime(&time));
    }
}
// -------------------------------------------------------------------------- //



// ------------------------ FILE: EXISTING PROPERTIES ----------------------- //
// Gets the current path
file::path_type 
file::
get_current_path() 
const
{
    return std::experimental::filesystem::current_path();
}

// Gets the absolute path of the file
file::path_type 
file::
get_absolute_path() 
const
{
    return std::experimental::filesystem::absolute(_path);
}

// Gets the relative path of the file
file::path_type 
file::
get_relative_path() 
const
{
    return _path;
}

// Gets the existence of the file
bool
file::
get_existence() 
const
{
    return std::experimental::filesystem::exists(_path);
}

// Gets the type of the file
file::file_type
file::
get_type() 
const
{
    return std::experimental::filesystem::status(_path).type();
}

// Gets the type of the file
file::perms_type
file::
get_permissions() 
const
{
    return std::experimental::filesystem::status(_path).permissions();
}

// Gets the size of the file
file::size_type
file::
get_size() 
const
{
    return std::experimental::filesystem::file_size(_path);
}

// Gets the timestamp of the file
file::time_type 
file::
get_time()
const
{
    return std::experimental::filesystem::last_write_time(_path);
}

// Gets the time string of the file
file::string_type 
file::
get_time_string(string_type fmt)
const
{
    char buffer[buffer_size];
    time_type time = std::experimental::filesystem::last_write_time(_path);
    std::time_t ctime = time_type::clock::to_time_t(time);
    std::strftime(buffer, buffer_size, fmt.data(), std::localtime(&ctime));
	return buffer;
}
// -------------------------------------------------------------------------- //



// ------------------------ FILE: INTERNAL PROPERTIES ----------------------- //
// Returns the internal path value
file::path_type 
file::
path() 
const
{
    return _path;
}

// Returns the internal file type value
file::file_type 
file::
type() 
const
{
    return _type;
}

// Returns the internal size value
file::size_type 
file::
size() 
const
{
    return _size;
}

// Returns the internal timestamp value
file::time_type 
file::
time() 
const
{
    return _time;
}

// Returns the internal time string
file::string_type 
file::
time_string(string_type fmt) 
const
{
    char buffer[buffer_size];
    std::time_t ctime = time_type::clock::to_time_t(_time);
    std::strftime(buffer, buffer_size, fmt.data(), std::localtime(&ctime));
	return buffer;
}
// -------------------------------------------------------------------------- //



// ------------------------- FILE: INPUT AND OUTPUT ------------------------- //
// Reads a text file, optionally using converting from wide characters
file::string_type 
file::
read() 
const
{
    string_type data;
    string_type filename = std::experimental::filesystem::absolute(_path);
    std::ifstream stream(filename, std::ios::in);
    std::stringstream sstream;
    if (stream.good()) {
        sstream << stream.rdbuf();
        data = static_cast<const std::stringstream&>(sstream).str();
        stream.close();
    }
    return data;
}

// Reads a wide character file
file::string_type 
file::
read_wide() 
const
{
    static const std::locale locale(std::locale(), new facet_type);
    string_type data;
    string_type filename = std::experimental::filesystem::absolute(_path);
    std::wstring_convert<facet_type, wchar_t> converter;
    std::wifstream stream(filename, std::ios::in);
    std::wstringstream sstream;
    std::wstring text;
    stream.imbue(locale);
    sstream.imbue(locale);
    if (stream.good()) {
        sstream << stream.rdbuf();
        text = static_cast<const std::wstringstream&>(sstream).str();
        data = converter.to_bytes(text);
        stream.close();
    }
    return data;
}

// Reads a binary file
file::binary_type 
file::
read_binary() 
const
{
    binary_type data;
    string_type filename = std::experimental::filesystem::absolute(_path);
    std::ifstream stream(filename, std::ios::in | std::ios::binary);
    std::streampos first = 0;
    std::streampos last = 0;
    if (stream.good()) {
        stream.seekg(0, std::ios::beg);
        first = stream.tellg();
        stream.seekg(0, std::ios::end);
        last = stream.tellg();
        stream.seekg(first);
        if (stream.good() && first != -1 && last != -1) {
            if (first < last) {
                data.resize(last - first);
                stream.read(data.data(), data.size() * sizeof(byte_type));
            }
        }
        stream.close();
    }
    return data;
}

// Creates a text file
file 
file::
create(const string_type& data, copy_type copy) 
{
    string_type filename = std::experimental::filesystem::absolute(_path);
    bool exists = std::experimental::filesystem::exists(filename);
    std::ofstream stream;
    if (!exists || static_cast<int>(copy & copy_type::overwrite_existing)) {
        stream.open(filename, std::ios::out);
        if (stream.good()) {
            if (data.size()) {
                stream << data;
            }
            stream.close();
        }
    }
    return file(_path);
}

// Removes the file
file 
file::
remove() 
{
    string_type filename = std::experimental::filesystem::absolute(_path);
    bool exists = std::experimental::filesystem::exists(filename);
    if (exists) {
        std::remove(filename.data());
    }
    return file(_path);
}
// -------------------------------------------------------------------------- //



// ---------------------------- FILE: FILE TYPES ---------------------------- //
// Checks whether the given path refers to block device
bool 
file::
is_block_file() 
const
{
    return std::experimental::filesystem::is_block_file(_path);
}

// Checks whether the given path refers to character device
bool 
file::
is_character_file() 
const
{
    return std::experimental::filesystem::is_character_file(_path);
}

// Checks whether the given path refers to a directory
bool 
file::
is_directory() 
const
{
    return std::experimental::filesystem::is_directory(_path);
}

// Checks whether the given path refers to an empty file or directory 
bool 
file::
is_empty() 
const
{
    return std::experimental::filesystem::is_empty(_path);
}

// Checks whether the given path refers to a named pipe
bool 
file::
is_fifo() 
const
{
    return std::experimental::filesystem::is_fifo(_path);
}

// Checks whether the given path refers to an other file
bool 
file::
is_other() 
const
{
    return std::experimental::filesystem::is_other(_path);
}

// Checks whether the given path refers to a regular file
bool 
file::
is_regular_file() 
const
{
    return std::experimental::filesystem::is_regular_file(_path);
}

// Checks whether the given path refers to a named IPC socket
bool 
file::
is_socket() 
const
{
    return std::experimental::filesystem::is_socket(_path);
}

// Checks whether the given path refers to a symbolic link 
bool 
file::
is_symlink() 
const
{
    return std::experimental::filesystem::is_symlink(_path);
}
// -------------------------------------------------------------------------- //



// ---------------------------- FILE: COMPONENTS ---------------------------- //
// Returns the root path
file::string_type 
file::
root() 
const
{
    return std::string(_path.root_path());
}

// Returns the parent path
file::string_type 
file::
parent() 
const
{
    return std::string(_path.parent_path());
}

// Returns the filename
file::string_type 
file::
filename() 
const
{
    return std::string(_path.filename());
}

// Returns the filename without the extension
file::string_type 
file::
stem() 
const
{
    return std::string(_path.stem());
}

// Returns the file extension
file::string_type 
file::
extension() 
const
{
    return std::string(_path.extension());
}
// -------------------------------------------------------------------------- //



// ----------------------------- FILE: DIRECTORY ---------------------------- //
// Gets the content of the directory
std::vector<file> 
file::
get_contents() 
const
{
    std::vector<file> contents;
    std::experimental::filesystem::directory_iterator dir(_path);
    if (std::experimental::filesystem::is_directory(_path)) {
        contents.reserve(reserve);
        for(auto& p: dir) {
            contents.emplace_back(p);
        }
    }
    return contents;
}

// Gets the content of the directory while filtering the path
template <class F>
std::vector<file> 
file::
get_contents(F&& f) 
const
{
    std::vector<file> contents;
    std::experimental::filesystem::directory_iterator dir(_path);
    if (std::experimental::filesystem::is_directory(_path)) {
        contents.reserve(reserve);
        for(auto& p: dir) {
            if (std::forward<F>(f)(p.path())) {
                contents.emplace_back(p);
            }
        }
    }
    return contents;
}

// Gets the content of the directory, recursively
std::vector<file> 
file::
get_recursive_contents() 
const
{
    std::vector<file> contents;
    std::experimental::filesystem::recursive_directory_iterator dir(_path);
    if (std::experimental::filesystem::is_directory(_path)) {
        contents.reserve(reserve);
        for(auto& p: dir) {
            contents.emplace_back(p);
        }
    }
    return contents;
}

// Gets the content of the directory, recursively, while filtering the path
template <class F>
std::vector<file> 
file::
get_recursive_contents(F&& f) 
const
{
    std::vector<file> contents;
    std::experimental::filesystem::recursive_directory_iterator dir(_path);
    if (std::experimental::filesystem::is_directory(_path)) {
        contents.reserve(reserve);
        for(auto& p: dir) {
            if (std::forward<F>(f)(p.path())) {
                contents.emplace_back(p);
            }
        }
    }
    return contents;
}
// -------------------------------------------------------------------------- //



// ---------------------------- FILE: FACTORIES ----------------------------- //
// Makes a temporary file
file 
file::
make_temporary(const string_type& extension)
{
    path_type directory = std::experimental::filesystem::temp_directory_path();
    string_type filename;
    std::random_device device;
    std::default_random_engine engine(device());
    std::uniform_int_distribution<int> distribution('a', 'z');
    string_type rdstring(bits, static_cast<char>(0));
    auto random_char = [&](char& c){c = distribution(engine);};
    char buffer[buffer_size];
    std::time_t ctime = time_type::clock::to_time_t(time_type::clock::now());
    std::strftime(buffer, buffer_size, time_fmt, std::localtime(&ctime));
    std::for_each(std::begin(rdstring), std::end(rdstring), random_char);
    filename = std::string(buffer) + "-" + rdstring + extension;
    return file(directory.append(filename));
}
// -------------------------------------------------------------------------- //



// ========================================================================== //
} // namespace epidemium_oncobase
#endif // _FILE_HPP_INCLUDED
// ========================================================================== //
