/*
 * Copyright 2020 Redpanda Data, Inc.
 *
 * Use of this software is governed by the Business Source License
 * included in the file licenses/BSL.md
 *
 * As of the Change Date specified in that file, in accordance with
 * the Business Source License, use of this software will be governed
 * by the Apache License, Version 2.0
 */

#pragma once

#include "seastarx.h"

#include <seastar/core/chunked_fifo.hh>
#include <seastar/core/lowres_clock.hh>
#include <seastar/core/print.hh>

#include <fmt/core.h>

#include <optional>
#include <ostream>
#include <variant>

namespace std {

template<typename T>
std::ostream& operator<<(std::ostream& os, const std::optional<T>& opt) {
    if (opt) {
        fmt::print(os, "{{{}}}", *opt);
        return os;
    }
    return os << "{nullopt}";
}

template<typename... T>
requires(sizeof...(T) > 0) std::ostream&
operator<<(std::ostream& os, const std::variant<T...>& v) {
    std::visit([&os](auto& arg) { fmt::print(os, "{{{}}}", arg); }, v);
    return os;
}

inline std::ostream&
operator<<(std::ostream& o, const ss::lowres_clock::duration& d) {
    fmt::print(
      o,
      "{}",
      std::chrono::duration_cast<std::chrono::milliseconds>(d).count());
    return o;
}

} // namespace std

template<typename T>
struct fmt::formatter<ss::chunked_fifo<T>> {
    using type = ss::chunked_fifo<T>;

    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template<typename FormatContext>
    typename FormatContext::iterator
    format(const type& fifo, FormatContext& ctx) const {
        format_to(ctx.out(), "[");
        if (!fifo.empty()) {
            auto it = fifo.begin();
            format_to(ctx.out(), "{}", *(it++));

            for (; it != fifo.end(); ++it) {
                format_to(ctx.out(), ", {}", *it);
            }
            return ctx.out();
        }

        format_to(ctx.out(), "]");
        return ctx.out();
    }
};
