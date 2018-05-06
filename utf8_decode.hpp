// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.
#ifndef UTF8_DECODE_HPP
#define UTF8_DECODE_HPP

#include <stdint.h>

#define UTF8_ACCEPT 0
#define UTF8_REJECT 1

namespace mk {
namespace libjson {

uint32_t utf8_decode(uint32_t *state, uint32_t *codep, uint32_t byte) noexcept;

} // namespace libjson
} // namespace mk
#endif
