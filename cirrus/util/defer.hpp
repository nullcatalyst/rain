#pragma once

namespace cirrus::util {

template <typename T>
class Defer {
    T _value;

  public:
    Defer(T value) : _value(std::move(value)) {}
    ~Defer() { _value(); }
};

}  // namespace cirrus::util
