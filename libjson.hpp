// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.
#ifndef MEASUREMENT_KIT_LIBJSON_LIBJSON_HPP
#define MEASUREMENT_KIT_LIBJSON_LIBJSON_HPP

#include <stdint.h>

#include <memory>
#include <string>

#define MK_LIBJSON_MAJOR 0
#define MK_LIBJSON_MINOR 3
#define MK_LIBJSON_PATCH 0

namespace mk {
namespace libjson {

// ArrayKeys
// =========
//
// Helper to iterate over arrays.
class ArrayKeys {
 public:
  class Iter {
   public:
    // TODO(bassosimone): implement iterator traits
    // xref: http://anderberg.me/2016/07/04/c-custom-iterators/

    Iter(const ArrayKeys &ak, size_t pos) noexcept;

    std::string operator*() const noexcept;

    Iter &operator++() noexcept;

    Iter &operator++(int /*dummy*/) noexcept;

    bool operator==(const Iter &other) const noexcept;

    bool operator!=(const Iter &other) const noexcept;

   private:
    const ArrayKeys &ak_;
    size_t pos_{};
  };

  ArrayKeys() noexcept;

  ArrayKeys(std::string path, size_t size) noexcept;

  Iter begin() const noexcept;

  Iter end() const noexcept;

  size_t size() const noexcept;

 private:
  friend class Iter;
  std::string path_{};
  size_t size_{};
};

// Json
// ====
//
// Wrapper for a JSON serializable object.
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

  bool get_array_keys(std::string path, ArrayKeys *ak) const noexcept;

  bool push_boolean(std::string path, bool value) noexcept;

  bool push_float(std::string path, double value) noexcept;

  bool push_integer(std::string path, int64_t value) noexcept;

  bool push_string(std::string path, std::string value) noexcept;

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
