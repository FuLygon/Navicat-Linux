#pragma once
#include <stddef.h>
#include <stdint.h>
#include <type_traits>

namespace nkg {

    template<typename ptr1_t, typename ptr2_t>
    [[nodiscard]]
    inline ptrdiff_t address_delta(ptr1_t ptr1, ptr2_t ptr2) noexcept {
        static_assert(std::is_pointer_v<ptr1_t> && std::is_pointer_v<ptr2_t>);
        return reinterpret_cast<const volatile char*>(ptr1) - reinterpret_cast<const volatile char*>(ptr2);
    }

    template<typename ptr_t>
    [[nodiscard]]
    inline ptr_t address_offset(ptr_t ptr, ptrdiff_t off) noexcept {
        static_assert(std::is_pointer_v<ptr_t>);
        return reinterpret_cast<ptr_t>(
            const_cast<char*>(
                reinterpret_cast<const volatile char*>(ptr) + off
            )
        );
    }

    template<typename return_ptr_t, typename ptr_t>
    [[nodiscard]]
    inline return_ptr_t address_offset_cast(ptr_t ptr, ptrdiff_t off) noexcept {
        static_assert(std::is_pointer_v<return_ptr_t> && std::is_pointer_v<ptr_t>);
        return reinterpret_cast<return_ptr_t>(address_offset(ptr, off));
    }

    template<typename ptr_t, typename begin_ptr_t, typename end_ptr_t>
    [[nodiscard]]
    inline bool is_address_in_range(ptr_t ptr, begin_ptr_t begin, end_ptr_t end) {
        static_assert(std::is_pointer_v<ptr_t> && std::is_pointer_v<begin_ptr_t> && std::is_pointer_v<end_ptr_t>);
        auto _ptr = reinterpret_cast<const volatile char*>(ptr);
        auto _begin = reinterpret_cast<const volatile char*>(begin);
        auto _end = reinterpret_cast<const volatile char*>(end);
        return _begin <= _ptr && _ptr < _end;
    }

    template<typename ptr_t, typename base_ptr_t>
    [[nodiscard]]
    inline bool is_address_in_range(ptr_t ptr, base_ptr_t base, size_t size) {
        static_assert(std::is_pointer_v<ptr_t> && std::is_pointer_v<base_ptr_t>);
        return is_address_in_range(ptr, base, address_offset(base, size));
    }

    template<typename ptr1_t, typename ptr2_t, typename begin_ptr_t, typename end_ptr_t>
    [[nodiscard]]
    inline bool is_address_in_range(ptr1_t ptr1, ptr2_t ptr2, begin_ptr_t begin, end_ptr_t end) {
        static_assert(std::is_pointer_v<ptr1_t> && std::is_pointer_v<ptr2_t> && std::is_pointer_v<begin_ptr_t> && std::is_pointer_v<end_ptr_t>);
        auto _ptr1 = reinterpret_cast<const volatile char*>(ptr1);
        auto _ptr2 = reinterpret_cast<const volatile char*>(ptr2);
        auto _begin = reinterpret_cast<const volatile char*>(begin);
        auto _end = reinterpret_cast<const volatile char*>(end);
        return _begin <= _ptr1 && _ptr1 <= _ptr2 && _ptr2 <= _end;
    }

    template<typename ptr_t, typename begin_ptr_t, typename end_ptr_t>
    [[nodiscard]]
    inline bool is_address_in_range(ptr_t ptr, size_t size, begin_ptr_t begin, end_ptr_t end) {
        static_assert(std::is_pointer_v<ptr_t> && std::is_pointer_v<begin_ptr_t> && std::is_pointer_v<end_ptr_t>);
        return is_address_in_range(ptr, address_offset(ptr, size), begin, end);
    }

    template<typename ptr1_t, typename ptr2_t, typename base_ptr_t>
    [[nodiscard]]
    inline bool is_address_in_range(ptr1_t ptr1, ptr2_t ptr2, base_ptr_t base, size_t size) {
        static_assert(std::is_pointer_v<ptr1_t> && std::is_pointer_v<ptr2_t> && std::is_pointer_v<base_ptr_t>);
        return is_address_in_range(ptr1, ptr2, base, address_offset(base, size));
    }

    template<typename ptr_t, typename base_ptr_t>
    [[nodiscard]]
    inline bool is_address_in_range(ptr_t p, size_t s, base_ptr_t base, size_t size) {
        static_assert(std::is_pointer_v<ptr_t>);
        static_assert(std::is_pointer_v<base_ptr_t>);
        return is_address_in_range(p, address_offset(p, s), base, address_offset(base, size));
    }

//    template<typename __ReadType, typename __PtrType>
//    [[nodiscard]]
//    inline __ReadType AddressRead(__PtrType p) noexcept {
//        static_assert(std::is_trivial_v<__ReadType> && std::is_standard_layout_v<__ReadType>);
//        static_assert(std::is_pointer_v<__PtrType>);
//        return *reinterpret_cast<const volatile __ReadType*>(p);
//    }
//
//    template<typename __ReadType, typename __PtrType>
//    [[nodiscard]]
//    inline __ReadType AddressRead(__PtrType p, ptrdiff_t offset) noexcept {
//        static_assert(std::is_trivial_v<__ReadType> && std::is_standard_layout_v<__ReadType>);
//        static_assert(std::is_pointer_v<__PtrType>);
//        return *reinterpret_cast<const volatile __ReadType*>(
//            reinterpret_cast<const volatile char*>(p) + offset
//        );
//    }
//
//    template<typename __ReadType, typename __PtrType>
//    [[nodiscard]]
//    inline __ReadType AddressRead(__PtrType p, size_t scale, ptrdiff_t index) noexcept {
//        static_assert(std::is_trivial_v<__ReadType> && std::is_standard_layout_v<__ReadType>);
//        static_assert(std::is_pointer_v<__PtrType>);
//        return *reinterpret_cast<const volatile __ReadType*>(
//            reinterpret_cast<const volatile char*>(p) + scale * index
//        );
//    }
//
//    template<typename __WriteType, typename __PtrType>
//    inline void AddressWrite(__PtrType p, const __WriteType& value) noexcept {
//        static_assert(std::is_trivial_v<__WriteType> && std::is_standard_layout_v<__WriteType>);
//        static_assert(std::is_pointer_v<__PtrType>);
//        *reinterpret_cast<volatile __WriteType*>(p) = value;
//    }
//
//    template<typename __WriteType, typename __PtrType>
//    inline void AddressWrite(__PtrType p, ptrdiff_t offset, const __WriteType& value) noexcept {
//        static_assert(std::is_trivial_v<__WriteType> && std::is_standard_layout_v<__WriteType>);
//        static_assert(std::is_pointer_v<__PtrType>);
//        *reinterpret_cast<volatile __WriteType*>(
//            reinterpret_cast<volatile char*>(p) + offset
//        ) = value;
//    }
//
//    template<typename __WriteType, typename __PtrType>
//    inline void AddressWrite(__PtrType p, size_t scale, ptrdiff_t index, const __WriteType& value) noexcept {
//        static_assert(std::is_trivial_v<__WriteType> && std::is_standard_layout_v<__WriteType>);
//        static_assert(std::is_pointer_v<__PtrType>);
//        *reinterpret_cast<volatile __WriteType*>(
//            reinterpret_cast<volatile char*>(p) + scale * index
//        ) = value;
//    }

}
