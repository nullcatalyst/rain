#pragma once

namespace cirrus::util {

template <typename T>
class BeforeReturn {
    T _value;

  public:
    BeforeReturn(T value) : _value(std::move(value)) {}
    ~BeforeReturn() { _value(); }
};

}  // namespace cirrus::util
