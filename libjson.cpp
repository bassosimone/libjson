// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.

#include "libjson.hpp"

#include <sstream>

#include "base64_encode.hpp"
#include "nlohmann_json.hpp"
#include "utf8_decode.hpp"

namespace mk {
namespace libjson {

using Exception = nlohmann::json::exception;

class Json::Impl {
 public:
  nlohmann::json json;
};

// Misc
// ----

static std::string possibly_encode(std::string value) noexcept {
  uint32_t codepoint = 0;
  uint32_t state = UTF8_ACCEPT;
  for (char ch : value) {
    (void)utf8_decode(&state, &codepoint, (uint8_t)ch);
    if (state == UTF8_REJECT) {
      break;
    }
  }
  if (state != UTF8_ACCEPT) {
    std::string s = base64_encode((uint8_t *)value.c_str(), value.size());
    std::swap(s, value);
  }
  return value;
}

// Scalar operations
// -----------------

#define SCALAR_SET_IMPL_(path, value)                      \
  try {                                                    \
    nlohmann::json::json_pointer pointer{std::move(path)}; \
    impl_->json[std::move(pointer)] = value;               \
  } catch (const Exception &) {                            \
    return false;                                          \
  }                                                        \
  return true

bool Json::set_boolean(std::string path, bool value) noexcept {
  SCALAR_SET_IMPL_(path, value);
}

bool Json::set_float(std::string path, double value) noexcept {
  SCALAR_SET_IMPL_(path, value);
}

bool Json::set_integer(std::string path, int64_t value) noexcept {
  SCALAR_SET_IMPL_(path, value);
}

bool Json::set_string(std::string path, std::string value) noexcept {
  SCALAR_SET_IMPL_(path, possibly_encode(std::move(value)));
}

#define SCALAR_GET_IMPL_(path, value)                      \
  if (!value) {                                            \
    return false;                                          \
  }                                                        \
  try {                                                    \
    nlohmann::json::json_pointer pointer{std::move(path)}; \
    *value = impl_->json.at(std::move(pointer));           \
  } catch (const Exception &) {                            \
    return false;                                          \
  }                                                        \
  return true

bool Json::get_boolean(std::string path, bool *value) const noexcept {
  SCALAR_GET_IMPL_(path, value);
}

bool Json::get_float(std::string path, double *value) const noexcept {
  SCALAR_GET_IMPL_(path, value);
}

bool Json::get_integer(std::string path, int64_t *value) const noexcept {
  SCALAR_GET_IMPL_(path, value);
}

bool Json::get_string(std::string path, std::string *value) const noexcept {
  SCALAR_GET_IMPL_(path, value);
}

// Array operations
// ----------------

bool Json::get_array_size(std::string path, size_t *size) const noexcept {
  if (!size) {
    return false;
  }
  try {
    nlohmann::json::json_pointer pointer{std::move(path)};
    auto &maybe_array = impl_->json.at(std::move(pointer));
    if (!maybe_array.is_array()) {
      return false;
    }
    *size = maybe_array.size();
  } catch (const Exception &) {
    return false;
  }
  return true;
}

/*static*/ std::string  //
Json::make_array_path(std::string path, size_t size) noexcept {
  std::stringstream stream;
  stream << path;
  if (path.size() > 0 && path[path.size() - 1] != '/') {
    stream << "/";
  }
  stream << size;
  return stream.str();
}

// TODO(bassosimone): write more tests for this macro.
#define ARRAY_PUSH_IMPL_(type, path, value)                \
  try {                                                    \
    nlohmann::json::json_pointer pointer{std::move(path)}; \
    impl_->json[std::move(pointer)].push_back(value);      \
  } catch (const Exception &) {                            \
    return false;                                          \
  }                                                        \
  return true

bool Json::push_boolean(std::string path, bool value) noexcept {
  ARRAY_PUSH_IMPL_(boolean, path, value);
}

bool Json::push_float(std::string path, double value) noexcept {
  ARRAY_PUSH_IMPL_(float, path, value);
}

bool Json::push_integer(std::string path, int64_t value) noexcept {
  ARRAY_PUSH_IMPL_(integer, path, value);
}

bool Json::push_string(std::string path, std::string value) noexcept {
  ARRAY_PUSH_IMPL_(string, path, possibly_encode(std::move(value)));
}

// Serialize/parse
// ---------------

bool Json::serialize(std::string *str) const noexcept {
  if (!str) {
    return false;
  }
  try {
    *str = impl_->json.dump();
  } catch (const Exception &) {
    return false;
  }
  return true;
}

bool Json::parse(std::string str) noexcept {
  try {
    impl_->json = nlohmann::json::parse(std::move(str));
  } catch (const Exception &) {
    return false;
  }
  return true;
}

// Ctor/dtor
// ---------

Json::Json() noexcept { impl_.reset(new Json::Impl); }

Json::~Json() noexcept {}

}  // namespace libjson
}  // namespace mk
