#pragma once

#include <cstddef>
#include <memory>

#include "absl/base/nullability.h"
#include "rain/util/log.hpp"
#include "rain/util/unreachable.hpp"

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

    MaybeOwnedPtr(const MaybeOwnedPtr& other) {
        switch (other._type) {
            case State::None:
                _type = State::None;
                break;
            case State::Ptr:
                _type = State::Ptr;
                _ptr  = other._ptr;
                break;
            case State::OwnedPtr:
                _type = State::Ptr;
                _ptr  = other.get();
                break;
        }
    }
    MaybeOwnedPtr& operator=(const MaybeOwnedPtr& other) {
        _reset();

        switch (other._type) {
            case State::None:
                _type = State::None;
                break;
            case State::Ptr:
                _type = State::Ptr;
                _ptr  = other._ptr;
                break;
            case State::OwnedPtr:
                _type = State::Ptr;
                _ptr  = other.get();
                break;
        }
    }

    MaybeOwnedPtr(MaybeOwnedPtr&& other) {
        switch (other._type) {
            case State::None:
                _type = State::None;
                break;
            case State::Ptr:
                _type = State::Ptr;
                _ptr  = other._ptr;
                break;
            case State::OwnedPtr:
                _type = State::OwnedPtr;
                new (&_owned_ptr) std::unique_ptr<T>(std::move(other._owned_ptr));
                break;
        }

        other._reset();
    }
    MaybeOwnedPtr& operator=(MaybeOwnedPtr&& other) {
        _reset();

        switch (other._type) {
            case State::None:
                _type = State::None;
                break;
            case State::Ptr:
                _type = State::Ptr;
                _ptr  = other._ptr;
                break;
            case State::OwnedPtr:
                _type = State::OwnedPtr;
                new (&_owned_ptr) std::unique_ptr<T>(std::move(other._owned_ptr));
                break;
        }

        other._reset();
        return *this;
    }

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

    bool operator==(const std::nullptr_t) const {
        switch (_type) {
            case State::None:
                return true;
            case State::Ptr:
                return _ptr == nullptr;
            case State::OwnedPtr:
                return _owned_ptr == nullptr;
        }
        util::unreachable();
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

    absl::Nullable<T*> get() const {
        switch (_type) {
            case State::None:
                return nullptr;
            case State::Ptr:
                return _ptr;
            case State::OwnedPtr:
                return _owned_ptr.get();
        }
    }

    absl::Nullable<T*> get_nonnull() const {
        switch (_type) {
            case State::None:
                util::console_error("trying to get a nonnull value from a null ptr");
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
