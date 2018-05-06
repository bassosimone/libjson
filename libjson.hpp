// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.
#ifndef MEASUREMENT_KIT_LIBJSON_LIBJSON_HPP
#define MEASUREMENT_KIT_LIBJSON_LIBJSON_HPP

#include <stdint.h>

#include <memory>

#define MK_LIBJSON_MAJOR 0
#define MK_LIBJSON_MINOR 1
#define MK_LIBJSON_PATCH 0

namespace mk {
namespace libjson {

class Json {
 public:
  // Scalar operations
  // -----------------

  bool set_boolean(std::string path, bool value) noexcept;

  bool set_float(std::string path, double value) noexcept;

  bool set_integer(std::string path, int64_t value) noexcept;

  bool set_string(std::string path, std::string value) noexcept;

  bool get_boolean(std::string path, bool *value) const noexcept;

  bool get_float(std::string path, double *value) const noexcept;

  bool get_integer(std::string path, int64_t *value) const noexcept;

  bool get_string(std::string path, std::string *value) const noexcept;

  // Array operations
  // ----------------

  bool get_size(std::string path, size_t *value) const noexcept;

  // Serialize/parse
  // ---------------

  bool serialize(std::string *str) const noexcept;

  bool parse(std::string str) noexcept;

  // Ctor/dtor
  // ---------

  Json() noexcept;

  ~Json() noexcept;

 private:
  class Impl;
  std::unique_ptr<Impl> impl_;
};

}  // namespace libjson
}  // namespace mk
#endif
