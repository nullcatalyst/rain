#pragma once

#include <type_traits>

namespace cirrus::util {

template <typename Vtbl, typename Data>
struct NoopMemMgr {
    static constexpr void retain(const Vtbl* const vtbl, Data* const data) noexcept {}
    static constexpr void release(const Vtbl* const vtbl, Data* const data) noexcept {}
};

/**
 * A fat pointer.
 *
 * This stores the vtable pointer and the data pointer together. Doing so is almost always more
 * efficient than storing the vtable pointer inside the data structure, since it removes one level
 * of indirection.
 *
 * Normally, member functions require 3 memory accesses to call a method:
 *      1. load the vtable pointer from the front of the data structure; `*data`
 *      2. load the data pointed to by the vtable pointer; `*data->__vtbl`
 *      3. load the instruction data to be executed; `*data->__vtbl->methodName`
 *
 * With a fat pointer, only 2 memory accesses are required:
 *      1. load the data pointed to by the vtable pointer; `*vtbl`
 *      2. load the instruction data to be executed; `*vtbl->methodName`
 */
template <typename This, typename Vtbl, typename Data, typename MemMgr = NoopMemMgr<Vtbl, Data>>
struct Ptr {
  protected:
    const Vtbl* _vtbl = nullptr;
    Data*       _data = nullptr;

    constexpr Ptr(const Vtbl* const vtbl, Data* const data) noexcept : _vtbl(vtbl), _data(data) {
        MemMgr::retain(vtbl, data);
    }

    template <typename This2, typename Vtbl2, typename Data2, typename MemMgr2>
    friend struct Ptr;

  public:
    constexpr Ptr() noexcept = default;
    ~Ptr() noexcept { MemMgr::release(_vtbl, _data); }

    constexpr Ptr(const Ptr& other) noexcept : _vtbl(other._vtbl), _data(other._data) {
        MemMgr::retain(_vtbl, _data);
    }

    constexpr Ptr(Ptr&& other) noexcept {
        _vtbl       = other._vtbl;
        _data       = other._data;
        other._vtbl = nullptr;
        other._data = nullptr;
    }

    constexpr Ptr& operator=(const Ptr& other) noexcept {
        MemMgr::release(_vtbl, _data);
        _vtbl = other._vtbl;
        _data = other._data;
        MemMgr::retain(_vtbl, _data);
        return *this;
    }

    constexpr Ptr& operator=(Ptr&& other) noexcept {
        MemMgr::release(_vtbl, _data);
        _vtbl       = other._vtbl;
        _data       = other._data;
        other._vtbl = nullptr;
        other._data = nullptr;
        return *this;
    }

    template <typename This2, typename Vtbl2, typename Data2>
    constexpr Ptr(const Ptr<This2, Vtbl2, Data2, MemMgr>& other) noexcept {
        // Allow implicitly casting to a base type, but not to an unrelated type
        static_assert(std::is_base_of_v<Vtbl, Vtbl2>, "cannot cast to unrelated vtbl type");
        static_assert(std::is_base_of_v<Data, Data2>, "cannot cast to unrelated data type");

        _vtbl = static_cast<const Vtbl*>(other._vtbl);
        _data = static_cast<Data*>(other._data);
        MemMgr::retain(_vtbl, _data);
    }

    template <typename This2, typename Vtbl2, typename Data2>
    constexpr Ptr(Ptr<This2, Vtbl2, Data2, MemMgr>&& other) noexcept {
        // Allow implicitly casting to a base type, but not to an unrelated type
        static_assert(std::is_base_of_v<Vtbl, Vtbl2>, "cannot cast to unrelated vtbl type");
        static_assert(std::is_base_of_v<Data, Data2>, "cannot cast to unrelated data type");

        _vtbl       = static_cast<const Vtbl*>(other._vtbl);
        _data       = static_cast<Data*>(other._data);
        other._vtbl = nullptr;
        other._data = nullptr;
    }

    constexpr Vtbl* vtbl() const noexcept { return const_cast<Vtbl*>(_vtbl); }
    constexpr Data* data() const noexcept { return _data; }

    static constexpr This from_raw(const Vtbl* const vtbl, Data* const data) noexcept {
        return This(vtbl, data);
    }

    template <typename This2, typename Vtbl2, typename Data2, typename MemMgr2>
    static constexpr This from(const Ptr<This2, Vtbl2, Data2, MemMgr2>& other) noexcept {
        // Allow implicitly casting to a base type, but not to an unrelated type
        static_assert(std::is_base_of_v<Vtbl2, Vtbl>, "cannot cast to unrelated type");
        static_assert(std::is_base_of_v<Data2, Data>, "cannot cast to unrelated type");

        const auto vtbl = static_cast<const Vtbl*>(other._vtbl);
        const auto data = static_cast<Data*>(other._data);
        return This(vtbl, data);
    }
};

}  // namespace cirrus::util
