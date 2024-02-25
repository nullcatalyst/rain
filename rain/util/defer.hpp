#pragma once

namespace rain::util {

template <typename T>
class Defer {
    T _value;

  public:
    Defer(T value) : _value(std::move(value)) {}
    ~Defer() { _value(); }
};

}  // namespace rain::util
