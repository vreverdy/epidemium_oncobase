// ============================== FTP MANAGER =============================== //
// Project:         epidemium_oncobase
// Name:            ftp_manager.hpp
// Description:     A tool to manage a ftp repository
// Creator:         Vincent Reverdy
// Contributor(s):  Vincent Reverdy [2015-2016]
// License:         GNU GPLv3
// ========================================================================== //
/** 
 * ftp_manager.hpp
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
#ifndef _FTP_MANAGER_HPP_INCLUDED
#define _FTP_MANAGER_HPP_INCLUDED
// ========================================================================== //



// ============================== PREPROCESSOR ============================== //
// Include C++
#include <regex>
#include <cctype>
#include <string>
#include <thread>
#include <vector>
#include <cstdlib>
#include <algorithm>
#include <stdexcept>
#include <experimental/filesystem>
// Include others
#include "file.hpp"
#include "string_view.hpp"
// Miscellaneous
namespace epidemium_oncobase {
// ========================================================================== //



/* ****************************** FTP MANAGER ******************************* */
// Ftp manager class definition
class ftp_manager
{
    // Lifecycle
    public:
    ftp_manager(ftp_manager&& other) = default;
    explicit ftp_manager(const std::string& path);

    // Assignment
    public:
    ftp_manager& operator=(ftp_manager&& other) = default;
    
    // Access
    public:
    const std::string& address() const noexcept;
    const std::vector<file>& files() const noexcept;
    
    // Management
    public:
    void update(bool verbose = 0);
    void upgrade(const std::string& path, bool unzip = 0, bool verbose = 0);
    void unzip(const std::string& path, bool verbose = 0);
    
    // Implementation details: data members
    private:
    std::string _address;
    std::vector<file> _files;
};
/* ************************************************************************** */



// ------------------------- FTP MANAGER: LIFECYCLE ------------------------- //
// Constructs a ftp manager from a link
ftp_manager::
ftp_manager(const std::string& path) 
: _address(path)
{
}
// -------------------------------------------------------------------------- //



// --------------------------- FTP MANAGER: ACCESS -------------------------- //
// Gets the address of the ftp
const std::string& 
ftp_manager::
address() 
const noexcept
{
    return _address;
}

// Gets the current list of files from the ftp
const std::vector<file>& 
ftp_manager::
files() 
const noexcept
{
    return _files;
}
// -------------------------------------------------------------------------- //



// ------------------------- FTP MANAGER: MANAGEMENT ------------------------ //
// Updates the internal list of files
void 
ftp_manager::
update(bool verbose)
{
    static const std::string space = " ";
    file tmp = file::make_temporary(".txt");
    std::string command = "wget";
    std::string contents;
    int code = 0;
    std::regex regex;
    std::sregex_iterator regex_begin = std::sregex_iterator();
    std::sregex_iterator regex_end = regex_begin;
    std::string line;
    string_view view(line); 
    std::string path;
    std::string name;
    std::string date;
    std::ptrdiff_t bytes = -1;
    (void)(code);
    _files.clear();
    command += space + "--output-document=" + std::string(tmp.path());
    command += space + _address;
    command += space + ">/dev/null 2>&1";
    if (verbose) {
        std::cout<<command<<std::endl;
    }
    code = std::system(command.data());
    contents = tmp.read();
    if (verbose) {
        std::cout<<"rm "<<std::string(tmp.path())<<std::endl;
    }
    tmp.remove();
    regex = std::regex("(.+<a.*>[A-Za-z0-9].*<\\/a>.+)");
    regex_begin = std::sregex_iterator(contents.begin(), contents.end(), regex);
    for (std::sregex_iterator i = regex_begin; i != regex_end; ++i) {
        bytes = 0;
        line = (*i).str();
        view = string_view(line).partition("<a")[0].strip();
        date = view.rpartition(" ")[0].strip().to_string();
        view = string_view(line).partition("\"")[2].partition("\"")[0].strip();
        path = view.to_string();
        view = string_view(std::cend(view), std::cend(line));
        view = view.partition(">")[2].partition("<")[0].strip();
        name = view.to_string();
        view = string_view(std::cend(view), std::cend(line));
        view = view.partition("(")[2].partition(")")[0].strip();
        if (view.size()) {
            bytes = static_cast<std::ptrdiff_t>(std::stoull(view.to_string())); 
        }
        _files.emplace_back(path, bytes, date);
    }  
}

// Upgrades the destination according to the ftp state
void 
ftp_manager::
upgrade(const std::string& path, bool unzip, bool verbose)
{
    file directory(path);
    std::vector<file> contents;
    bool found = false;
    bool remove = false;
    std::string link;
    std::string space = " ";
    std::string wget = "wget --directory-prefix=";
    std::string untar = "tar --directory";
    std::string gzip = "gzip -k -d -f";
    std::string zip = "unzip -o -d";
    std::string devnull = ">/dev/null 2>&1";
    std::string command;
    std::string cmd;
    std::string filename;
    file current(".");
    int code = 0;
    auto unzipper = [=, &cmd](){int x = std::system(cmd.data()); (void)(x);};
    std::thread thread;
    (void)(code);
    if (directory.is_directory()) {
        wget += std::string(directory.get_absolute_path());
        untar += space + std::string(directory.get_absolute_path());
        untar += space + "-xvzf";
        zip += space + std::string(directory.get_absolute_path());
        contents = directory.get_contents();
        for (auto&& remote: _files) {
            if (remote.size() > 0) {
                found = false;
                remove = false;
                link.clear();
                for (auto&& local: contents) {
                    if (remote.path().filename() == local.path().filename()) {
                        found = true;
                        if (remote.size() != local.size()) {
                            link = remote.path();
                            remove = true;
                        }
                        break;
                    }
                }
                if (!found) {
                    link = remote.path();
                }
                if (link.size() && link.find(".tmp") == std::string::npos) {
                    filename = remote.path().filename();
                    command = wget + space + link + space + devnull;
                    if (verbose) {
                        std::cout<<command<<std::endl;
                    }
                    if (remove) {
                        current = file(directory.path().append(filename));
                        if (current.get_existence()) {
                            current.remove();
                        }
                    }
                    code = std::system(command.data());
                    current = file(directory.path().append(filename));
                    if (current.get_existence()) {
                        filename = current.get_absolute_path();
                        if (unzip) {
                            cmd.clear();
                            if (link.find(".tar.gz") != std::string::npos) {
                                cmd = untar + space + filename;
                            } else if (link.find(".gz") != std::string::npos) {
                                cmd = gzip + space + filename;
                            } else if (link.find(".zip") != std::string::npos) {
                                cmd = zip + space + filename;
                            }
                            if (cmd.size()) {
                                if (thread.joinable()) {
                                    thread.join();
                                }
                                cmd += space + devnull;
                                if (verbose) {
                                    std::cout<<cmd<<std::endl;
                                }
                                thread = std::thread(unzipper);
                            }
                        }
                    }
                }
            }
        }
    } else if (verbose) {
        std::cout<<directory.path().c_str()<<" not found"<<std::endl;
    }
    if (thread.joinable()) {
        thread.join();
    }
}

// Unzips the contents of the destination folder
void 
ftp_manager::
unzip(const std::string& path, bool verbose)
{
    file directory(path);
    std::vector<file> files;
    std::string space = " ";
    std::string untar = "tar --directory";
    std::string gzip = "gzip -k -d -f";
    std::string zip = "unzip -o -d";
    std::string devnull = ">/dev/null 2>&1";
    std::string cmd;
    std::string filename;
    int code = 0;
    (void)(code);
    if (directory.is_directory()) {
        untar += space + std::string(directory.get_absolute_path());
        untar += space + "-xvzf";
        zip += space + std::string(directory.get_absolute_path());
        files = directory.get_contents();
        for (auto&& current: files) {
            if (current.is_regular_file()) {
                filename = current.get_absolute_path();
                if (filename.find(".tmp") != std::string::npos) {
                    cmd.clear();
                } else if (filename.find(".tar.gz") != std::string::npos) {
                    cmd = untar + space + filename;
                } else if (filename.find(".gz") != std::string::npos) {
                    cmd = gzip + space + filename;
                } else if (filename.find(".zip") != std::string::npos) {
                    cmd = zip + space + filename;
                }
                if (cmd.size()) {
                    cmd += space + devnull;
                    if (verbose) {
                        std::cout<<cmd<<std::endl;
                    }
                    code = std::system(cmd.data());
                }
            }
        }
    }
}
// -------------------------------------------------------------------------- //



// ========================================================================== //
} // namespace epidemium_oncobase
#endif // _FTP_MANAGER_HPP_INCLUDED
// ========================================================================== //
