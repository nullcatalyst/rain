#pragma once

#include <cstddef>
#include <memory>

#include "absl/base/nullability.h"

namespace rain::util {

template <typename T>
class MaybeOwnedPtr {
    enum class State {
        None,
        Ptr,
        OwnedPtr,
    };

    union {
        absl::Nonnull<T*>  _ptr;
        std::unique_ptr<T> _owned_ptr;
    };
    State _type = State::None;

  public:
    MaybeOwnedPtr() : _type(State::None) {}
    ~MaybeOwnedPtr() { _reset(); }

    MaybeOwnedPtr(std::nullptr_t) : _type(State::None) {}
    MaybeOwnedPtr& operator=(std::nullptr_t) {
        _reset();
        _type = State::None;
        return *this;
    }

    MaybeOwnedPtr(absl::Nonnull<T*> ptr) : _ptr(ptr), _type(State::Ptr) {}
    MaybeOwnedPtr& operator=(absl::Nonnull<T*> ptr) {
        _reset();
        _ptr  = ptr;
        _type = State::Ptr;
        return *this;
    }

    MaybeOwnedPtr(std::unique_ptr<T> owned_ptr)
        : _owned_ptr(std::move(owned_ptr)), _type(State::OwnedPtr) {}
    MaybeOwnedPtr& operator=(std::unique_ptr<T> owned_ptr) {
        _reset();
        new (&_owned_ptr) std::unique_ptr<T>(std::move(owned_ptr));
        _type = State::OwnedPtr;
        return *this;
    }

    MaybeOwnedPtr(MaybeOwnedPtr&& other) : _type(other._type) {
        switch (_type) {
            case State::None:
                break;
            case State::Ptr:
                _ptr = other._ptr;
                break;
            case State::OwnedPtr:
                new (&_owned_ptr) std::unique_ptr<T>(std::move(other._owned_ptr));
                break;
        }
        other._type = State::None;
    }
    MaybeOwnedPtr& operator=(MaybeOwnedPtr&& other) {
        _reset();
        _type = other._type;
        switch (_type) {
            case State::None:
                break;
            case State::Ptr:
                _ptr = other._ptr;
                break;
            case State::OwnedPtr:
                new (&_owned_ptr) std::unique_ptr<T>(std::move(other._owned_ptr));
                break;
        }
        other._type = State::None;
    }

    bool operator==(const std::nullptr_t) const {
        switch (_type) {
            case State::None:
                return true;
            case State::Ptr:
                return _ptr == nullptr;
            case State::OwnedPtr:
                return _owned_ptr == nullptr;
        }
    }

    bool operator!=(const std::nullptr_t) const {
        switch (_type) {
            case State::None:
                return false;
            case State::Ptr:
                return _ptr != nullptr;
            case State::OwnedPtr:
                return _owned_ptr != nullptr;
        }
    }

    operator absl::Nullable<T*>() const {
        switch (_type) {
            case State::None:
                return nullptr;
            case State::Ptr:
                return _ptr;
            case State::OwnedPtr:
                return _owned_ptr.get();
        }
    }

    T* operator->() const {
        switch (_type) {
            case State::None:
                std::abort();
            case State::Ptr:
                return _ptr;
            case State::OwnedPtr:
                return _owned_ptr.get();
        }
    }

  private:
    void _reset() {
        if (_type == State::OwnedPtr) {
            _owned_ptr.~unique_ptr<T>();
        }
        _type = State::None;
    }
};

}  // namespace rain::util
