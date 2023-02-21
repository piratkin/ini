#pragma once

#include <iostream>
#include <optional>
#include <fstream>
#include <sstream>
#include <limits>
#include <string>
#include <map>

namespace ini {
    using namespace std;

    template<typename T>
    struct Convert {
    };

    template<>
    struct Convert<uint16_t> {
        bool decode(const string &value, uint16_t &result) const {
            size_t pos;
            unsigned long long tmp;
            try {
                tmp = stoull(value, &pos);
            }
            catch (...) {
                return false;
            }
            if (pos != value.length() ||
                tmp > numeric_limits<uint16_t>::max())
                return false;
            result = static_cast<uint16_t>(tmp);
            return true;
        }
    };

    template<>
    struct Convert<uint32_t> {
        bool decode(const string &value, uint32_t &result) const {
            size_t pos;
            unsigned long long tmp;
            try {
                tmp = stoull(value, &pos);
            }
            catch (...) {
                return false;
            }
            if (pos != value.length() ||
                tmp > numeric_limits<uint32_t>::max())
                return false;
            result = static_cast<uint32_t>(tmp);
            return true;
        }
    };

    template<>
    struct Convert<string> {
        bool decode(const string_view value, string &result) const {
            result = value;
            return true;
        }
    };

    class Field {
    private:
        std::optional<string> _value;
    public:
        Field() : _value(std::nullopt) {}

        template<typename T>
        T as(const T &defVal) const {
            Convert<T> convert;
            T result;

            if (_value.has_value() &&
                convert.decode(_value.value(), result))
                return result;

            return defVal;
        }

        template<typename T>
        std::ostream &operator<<(std::ostream &os) {
            if (_value.has_value()) {
                os << _value.value();
            }
            return os;
        }

        template<typename T>
        Field &operator=(const T &value) {
            _value = value;
            return *this;
        }
    };

    class Section : public map<string, Field> {
    };

    class File : public map<string, Section> {
    private:
        static void trim(string &str) {
            size_t startPos = str.find_first_not_of(" \t\r\"");
            if (string::npos != startPos) {
                size_t endPos = str.find_last_not_of(" \t\r\"");
                str = str.substr(startPos, endPos - startPos + 1);
            } else
                str = "";
        }

        void eraseComments(string &str,
                           string::size_type startPos = 0) {
            const auto commentPrefix = "#"s;

            size_t prefixPos = str.find(commentPrefix, startPos);
            if (string::npos == prefixPos)
                return;

            if (0 != prefixPos && '\\' == str[prefixPos - 1]) {
                str.erase(prefixPos - 1, 1);
                eraseComments(str, prefixPos - 1 + commentPrefix.size());
            } else {
                str.erase(prefixPos);
            }
        }

        int _errors;
        bool _is_open;
        std::string _file;
    public:
        File()
                : _errors(0), _is_open(false) {}

        File(const string &filename)
                : _errors(0), _is_open(false) {
            load(filename);
        }

        int ok() { return _errors; }

        bool isOpen() { return _is_open; }

        bool load(const string &filename) {
            _errors = 0;
            _is_open = false;
            ifstream is(filename);
            if (is.is_open()) {
                decode(is);
                _is_open = true;
                _file = filename;
                is.close();
            }
            return _is_open;
        }

        void decode(istream &is) {
            clear();
            int lineNo = 0;
            Section *currentSection = nullptr;
            while (!is.eof() && !is.fail()) {
                string line;
                getline(is, line, '\n');
                eraseComments(line);
                trim(line);
                ++lineNo;

                if (line.size() == 0)
                    continue;

                if (line[0] == '[') {
                    size_t pos;
                    if (pos = line.find("]");
                            pos == string::npos ||
                            pos <= 1)
                        ++_errors;
                    else {
                        string secName = line.substr(1, pos - 1);
                        currentSection = &((*this)[secName]);
                    }
                } else {
                    size_t pos;
                    if (pos = line.find('=');
                            currentSection == nullptr ||
                            pos == string::npos)
                        ++_errors;
                    else {
                        string name = line.substr(0, pos);
                        trim(name);
                        string value = line.substr(pos + 1, string::npos);
                        trim(value);
                        (*currentSection)[name] = value;
                    }
                }
            }
        }

        bool save(std::optional<const std::string> file = std::nullopt) {
            if (!_is_open) return false;
            auto path = file.value_or(_file);
            auto op = dump();
            if (!op.has_value()) return false;
            std::ofstream os(path);
            if (!os.is_open()) return false;
            auto content = op.value();
            os.write(content.c_str(), content.size());
            os.close();
            return true;
        }

        std::optional<std::string> dump() {
            if (!_is_open) return std::nullopt;
            std::stringstream ss;
            for (const auto &section : *this) {
                ss << "["
                   << section.first
                   << "]"
                   << std::endl;
                for (const auto &field : section.second) {
                    ss << field.first
                       << "="
                       << field.second.template as<std::string>("")
                       << std::endl;
                }
            }
            return ss.str();
        }
    };
}

