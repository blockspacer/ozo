#pragma once

#include <ozo/core/concept.h>
#include <ozo/detail/istream.h>
#include <ozo/detail/endian.h>
#include <ozo/detail/float.h>
#include <ozo/detail/typed_buffer.h>

#include <boost/hana/for_each.hpp>

namespace ozo::impl {

using detail::istream;

template <typename T>
inline Require<RawDataWritable<T>, istream&> read(istream& in, T& out) {
    using std::data;
    using std::size;
    in.read(data(out), size(out));
    if (!in) {
        throw system_error(error::unexpected_eof);
    }
    return in;
}

template <typename T>
inline Require<Integral<T> && sizeof(T) == 1, istream&> read(istream& in, T& out) {
    istream::traits_type::int_type c = in.get();
    if (!in) {
        throw system_error(error::unexpected_eof);
    }
    out = istream::traits_type::to_char_type(c);
    return in;
}

template <typename T>
inline Require<Integral<T> && sizeof(T) != 1, istream&> read(istream& in, T& out) {
    detail::typed_buffer<T> buf;
    read(in, buf);
    out = detail::convert_from_big_endian(buf.typed);
    return in;
}

template <typename T>
inline Require<FloatingPoint<T>, istream&> read(istream& in, T& out) {
    detail::floating_point_integral_t<T> tmp;
    read(in, tmp);
    out = detail::to_floating_point(tmp);
    return in;
}

inline istream& read(istream& in, bool& out) {
    char tmp = 0;
    read(in, tmp);
    out = tmp;
    return in;
}

template <typename T>
inline Require<HanaStruct<T>, istream&> read(istream& in, T& out) {
    hana::for_each(hana::keys(out), [&in, &out](auto key) {
        read(in, hana::at_key(out, key));
    });
    return in;
}

} // namespace ozo::impl
