#pragma once

#include <iostream>
#include <string>
#include <string_view>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ADT/SmallVector.h"

namespace cirrus::util {

class Twine final {
    enum class Kind {
        Empty,
        String,
        Rope,
    };

    Kind _kind = Kind::Empty;
    union {
        std::string_view                       _str;
        llvm::SmallVector<std::string_view, 2> _rope;
    };

  public:
    using Iterator = size_t;

    Twine() { _kind = Kind::Empty; }

    /*implicit*/ Twine(const char* str) : Twine(std::string_view(str)) {}

    /*implicit*/ Twine(const std::string_view str) {
        if (str.empty()) {
            _kind = Kind::Empty;
        } else {
            _kind = Kind::String;
            new (&_str) std::string_view(str);
        }
    }

    /*implicit*/ Twine(const llvm::ArrayRef<std::string_view> segments) {
        if (segments.empty()) {
            _kind = Kind::Empty;
        } else if (segments.size() == 1) {
            _kind = Kind::String;
            new (&_str) std::string_view(segments[0]);
        } else {
            _kind = Kind::Rope;
            new (&_rope) llvm::SmallVector<std::string_view, 2>{segments.begin(), segments.end()};
        }
    }

    Twine(const Twine& other) {
        _kind = other._kind;
        switch (_kind) {
            case Kind::Empty:
                break;

            case Kind::String:
                new (&_str) std::string_view(other._str);
                break;

            case Kind::Rope:
                new (&_rope) llvm::SmallVector<std::string_view, 2>(other._rope);
                break;
        }
    }

    Twine& operator=(const Twine& other) {
        if (this == &other) {
            return *this;
        }

        _destroy();

        _kind = other._kind;
        switch (_kind) {
            case Kind::Empty:
                break;

            case Kind::String:
                new (&_str) std::string_view(other._str);
                break;

            case Kind::Rope:
                new (&_rope) llvm::SmallVector<std::string_view, 2>(other._rope);
                break;
        }

        return *this;
    }

    Twine(Twine&& other) {
        _kind       = other._kind;
        other._kind = Kind::Empty;
        switch (_kind) {
            case Kind::Empty:
                break;

            case Kind::String:
                new (&_str) std::string_view(std::move(other._str));
                break;

            case Kind::Rope:
                new (&_rope) llvm::SmallVector<std::string_view, 2>(std::move(other._rope));
                break;
        }
    }

    Twine& operator=(Twine&& other) {
        if (this == &other) {
            return *this;
        }

        _destroy();

        _kind       = other._kind;
        other._kind = Kind::Empty;
        switch (_kind) {
            case Kind::Empty:
                break;

            case Kind::String:
                new (&_str) std::string_view(std::move(other._str));
                break;

            case Kind::Rope:
                new (&_rope) llvm::SmallVector<std::string_view, 2>(std::move(other._rope));
                break;
        }

        return *this;
    }

    ~Twine() { _destroy(); }

    [[nodiscard]] constexpr Iterator begin() const { return 0; }
    [[nodiscard]] constexpr Iterator end() const { return size(); }

    [[nodiscard]] constexpr bool empty() const { return _kind == Kind::Empty; }

    [[nodiscard]] constexpr int size() const {
        switch (_kind) {
            case Kind::Empty:
                return 0;

            case Kind::String:
                return _str.size();

            case Kind::Rope: {
                int size = 0;
                for (const auto& segment : _rope) {
                    size += segment.size();
                }
                return size;
            }
        }
    }

    [[nodiscard]] std::string str() const {
        switch (_kind) {
            case Kind::Empty:
                return std::string();

            case Kind::String:
                return std::string(_str);

            case Kind::Rope: {
                std::string result;
                result.reserve(size());
                for (const auto& segment : _rope) {
                    result += segment;
                }
                return result;
            }
        }
    }

    [[nodiscard]] constexpr char operator[](Iterator index) const {
        switch (_kind) {
            case Kind::Empty:
                return '\0';

            case Kind::String: {
                if (index < 0 || index >= _str.size()) {
                    return '\0';
                }
                return _str[index];
            }

            case Kind::Rope: {
                if (index < 0) {
                    return '\0';
                }

                for (const auto& segment : _rope) {
                    if (index < segment.size()) {
                        return segment[index];
                    }
                    index -= segment.size();
                }

                return '\0';
            }
        }
    }

    Twine& concat(const char* other) { return concat(std::string_view(other)); }

    Twine& concat(const std::string_view other) {
        if (other.empty()) {
            return *this;
        }

        switch (_kind) {
            case Kind::Empty:
                *this = other;
                return *this;

            case Kind::String: {
                const auto str = _str;
                _destroy();

                _kind = Kind::Rope;
                new (&_rope) llvm::SmallVector<std::string_view, 2>{
                    str,
                    other,
                };
                return *this;
            }

            case Kind::Rope:
                _rope.push_back(other);
                return *this;
        }
    }

    Twine& concat(const llvm::ArrayRef<std::string_view> other) {
        if (other.empty()) {
            return *this;
        }

        switch (_kind) {
            case Kind::Empty:
                *this = other;
                return *this;

            case Kind::String: {
                const auto str = _str;
                _destroy();

                _kind = Kind::Rope;
                new (&_rope) llvm::SmallVector<std::string_view, 2>();

                _rope.push_back(str);
                _rope.append(other.begin(), other.end());
                return *this;
            }

            case Kind::Rope:
                _rope.append(other.begin(), other.end());
                return *this;
        }
    }

    Twine& concat(const Twine& other) {
        if (other._kind == Kind::Empty) {
            return *this;
        }

        switch (_kind) {
            case Kind::Empty:
                *this = other;
                return *this;

            case Kind::String: {
                const auto str = _str;
                _destroy();

                _kind = Kind::Rope;
                new (&_rope) llvm::SmallVector<std::string_view, 2>();

                if (other._kind == Kind::String) {
                    _rope.push_back(str);
                    _rope.push_back(other._str);
                } else {  // other._kind == Kind::Rope
                    _rope.push_back(str);
                    _rope.append(other._rope.begin(), other._rope.end());
                }
                return *this;
            }

            case Kind::Rope:
                if (other._kind == Kind::String) {
                    _rope.push_back(other._str);
                } else {  // other._kind == Kind::Rope
                    _rope.append(other._rope.begin(), other._rope.end());
                }
                return *this;
        }
    }

    Twine& insert(int index, const Twine& other) {
        if (index < 0 || index > size()) {
            return *this;
        }

        if (_kind == Kind::Empty) {
            return *this = other;
        }
        if (other._kind == Kind::Empty) {
            return *this;
        }

        if (_kind == Kind::String && other._kind == Kind::String) {
            const auto first = _str.substr(0, index);
            const auto third = _str.substr(index);

            _destroy();

            _kind = Kind::Rope;
            new (&_rope) llvm::SmallVector<std::string_view, 2>();

            if (index == size()) {
                _rope.push_back(_str);
                _rope.push_back(other._str);
            } else {
                _rope.push_back(first);
                _rope.push_back(other._str);
                _rope.push_back(third);
            }
        }

        if (_kind == Kind::Rope) {
            // Find location to insert
            for (int i = 0; i < _rope.size(); ++i) {
                auto& segment = _rope[i];

                // The insertion point is conveniently between two segments of the rope
                if (index == 0) {
                    if (other._kind == Kind::String) {
                        _rope.insert(_rope.begin() + i, other._str);
                    } else {
                        _rope.insert(_rope.begin() + i, other._rope.begin(), other._rope.end());
                    }
                    return *this;
                }

                // The insertion point is within the current segment, split it and insert it between
                // those new segments
                if (index < segment.size()) {
                    if (other._kind == Kind::String) {
                        const auto first = segment.substr(0, index);
                        const auto third = segment.substr(index);
                        _rope[i]         = first;
                        _rope.insert(_rope.begin() + i + 1, other._str);
                        _rope.insert(_rope.begin() + i + 2, third);
                    } else {
                        const auto first = segment.substr(0, index);
                        const auto third = segment.substr(index);
                        _rope[i]         = first;
                        _rope.insert(_rope.begin() + i + 1, other._rope.begin(), other._rope.end());
                        _rope.insert(_rope.begin() + i + 2, third);
                    }
                    return *this;
                }

                index -= segment.size();
            }
        }

        return *this;
    }

    [[nodiscard]] Twine slice(Iterator begin, Iterator end) const {
        if (begin < 0 || begin > size() || begin > end) {
            return Twine();
        }

        switch (_kind) {
            case Kind::Empty:
                return Twine();

            case Kind::String:
                return Twine(_str.substr(begin, end - begin));

            case Kind::Rope: {
                Twine    twine;
                Iterator curr = 0;
                for (const auto& segment : _rope) {
                    if (curr < begin) {
                        curr += segment.size();
                        continue;
                    }

                    if (end <= curr + segment.size()) {
                        twine.concat(segment.substr(begin, end - begin));
                        break;
                    } else {
                        twine.concat(segment);
                    }

                    curr += segment.size();
                }
                return twine;
            }
        }
    }

    [[nodiscard]] Twine substr(Iterator begin, size_t n) const { return slice(begin, begin + n); }

    [[nodiscard]] Iterator find(char c, Iterator from = 0) const {
        if (from < 0 || from >= size()) {
            return -1;
        }

        switch (_kind) {
            case Kind::Empty:
                return -1;

            case Kind::String:
                return _str.find(c, from);

            case Kind::Rope: {
                Iterator curr = 0;
                for (const auto& segment : _rope) {
                    if (from < curr + segment.size()) {
                        const auto index = segment.find(c, from - curr);
                        if (index != std::string_view::npos) {
                            return curr + index;
                        }
                    }
                    curr += segment.size();
                }
                return -1;
            }
        }
    }

    [[nodiscard]] Iterator rfind(const char c, Iterator from) const noexcept {
        if (from < 0 || from >= size()) {
            return -1;
        }

        switch (_kind) {
            case Kind::Empty:
                return -1;

            case Kind::String:
                return _str.rfind(c, from);

            case Kind::Rope: {
                Iterator curr = 0;
                for (auto it = _rope.rbegin(); it != _rope.rend(); ++it) {
                    const auto& segment = *it;
                    if (from >= curr) {
                        const auto index = segment.rfind(c, from - curr);
                        if (index != std::string_view::npos) {
                            return curr + index;
                        }
                    }
                    curr += segment.size();
                }
                return -1;
            }
        }
    }

    [[nodiscard]] operator std::string() const { return str(); }

    friend std::ostream& operator<<(std::ostream& os, const Twine& twine) {
        switch (twine._kind) {
            case Kind::Empty:
                return os;

            case Kind::String:
                return os << twine._str;

            case Kind::Rope:
                for (const auto& segment : twine._rope) {
                    os << segment;
                }
                return os;
        }
    }

  private:
    void _destroy() {
        switch (_kind) {
            case Kind::Empty:
                break;

            case Kind::String:
                _str.~basic_string_view();
                break;

            case Kind::Rope:
                _rope.~SmallVector();
                break;
        }
        _kind = Kind::Empty;
    }
};

}  // namespace cirrus::util
