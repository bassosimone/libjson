// Part of Measurement Kit <https://measurement-kit.github.io/>.
// Measurement Kit is free software under the BSD license. See AUTHORS
// and LICENSE for more information on the copying conditions.

#define CATCH_CONFIG_MAIN

#include "libjson.hpp"

#include <stdio.h>

#include "catchorg_catch.hpp"
#include "nlohmann_json.hpp"

using namespace mk::libjson;

// Scalar setter
// -------------
//
// Make sure that setting a value does not provoke an error. The special case
// of an invalid UTF-8 string is checked near the end of this file.

#define SETTER_CHECK(descr, setter, path, value) \
  TEST_CASE(descr) {                             \
    Json doc;                                    \
    REQUIRE(doc.set_##setter(path, value));      \
  }

SETTER_CHECK("We can set a boolean", boolean, "/value", true)
SETTER_CHECK("We can set a nested boolean", boolean, "/x/value", true)
SETTER_CHECK("We can set a float", float, "/value", 17.0)
SETTER_CHECK("We can set a nested float", float, "/x/value", 17.0)
SETTER_CHECK("We can set a integer", integer, "/value", 17)
SETTER_CHECK("We can set a nested integer", integer, "/x/value", 17)
SETTER_CHECK("We can set a string", string, "/value", "foo")
SETTER_CHECK("We can set a nested string", string, "/x/value", "foo")

// Scalar empty getter
// -------------------
//
// Make sure that getting a non-existing value is not possible, that the
// value is not changed, that the internals do not change.

#define EMPTY_GETTER_CHECK(descr, getter, path, type) \
  TEST_CASE(descr) {                                  \
    type value{};                                     \
    Json doc;                                         \
    REQUIRE(!doc.get_##getter(path, &value));         \
    REQUIRE(value == type{});                         \
    {                                                 \
      std::string s;                                  \
      REQUIRE(doc.serialize(&s));                     \
      REQUIRE(s == "null");                           \
    }                                                 \
  }

EMPTY_GETTER_CHECK("We cannot get a nonexistent toplevel boolean", boolean,
                   "/value", bool);
EMPTY_GETTER_CHECK("We cannot get a nonexistent nested boolean", boolean,
                   "/x/value", bool);
EMPTY_GETTER_CHECK("We cannot get a nonexistent toplevel float", float,
                   "/value", double);
EMPTY_GETTER_CHECK("We cannot get a nonexistent nested float", float,
                   "/x/value", double);
EMPTY_GETTER_CHECK("We cannot get a nonexistent toplevel integer", integer,
                   "/value", int64_t);
EMPTY_GETTER_CHECK("We cannot get a nonexistent nested integer", integer,
                   "/x/value", int64_t);
EMPTY_GETTER_CHECK("We cannot get a nonexistent toplevel string", string,
                   "/value", std::string);
EMPTY_GETTER_CHECK("We cannot get a nonexistent nested string", string,
                   "/x/value", std::string);

// Scalar setter-getter
// --------------------
//
// This set of tests checks that we can get the values that we set.

#define SETTER_GETTER_CHECK(descr, getter_setter, path, type, value) \
  TEST_CASE(descr) {                                                 \
    Json doc;                                                        \
    REQUIRE(doc.set_##getter_setter(path, value));                   \
    type outvalue{};                                                 \
    REQUIRE(doc.get_##getter_setter(path, &outvalue));               \
    REQUIRE(outvalue == value);                                      \
  }

SETTER_GETTER_CHECK("We can set and then get a toplevel boolean", boolean,
                    "/value", bool, true);
SETTER_GETTER_CHECK("We can set and then get a nested boolean", boolean,
                    "/x/value", bool, true);
SETTER_GETTER_CHECK("We can set and then get a toplevel float", float,
                    "/value", double, 17.0);
SETTER_GETTER_CHECK("We can set and then get a nested float", float,
                    "/x/value", double, 17.0);
SETTER_GETTER_CHECK("We can set and then get a toplevel integer", integer,
                    "/value", int64_t, 11);
SETTER_GETTER_CHECK("We can set and then get a nested integer", integer,
                    "/x/value", int64_t, 11);
SETTER_GETTER_CHECK("We can set and then get a toplevel string", string,
                    "/value", std::string, "antani");
SETTER_GETTER_CHECK("We can set and then get a nested string", string,
                    "/x/value", std::string, "antani");

// Parse
// -----
//
// Make sure that we can parse a JSON and that we can access its fields.

TEST_CASE("We can parse and process a JSON") {
  Json doc;
  {
    std::string input = R"abcxyz(
      {
        "annotations": {
          "engine_name": "libmeasurement_kit",
          "platform": "macos"
        },
        "inputs": [
          "www.kernel.org",
          "www.x.org"
        ],
        "name": "Ndt",
        "version_major": 17,
        "elapsed": 1.14
      }
    )abcxyz";
    REQUIRE(doc.parse(std::move(input)));
  }
  {
    std::string s;
    REQUIRE(doc.get_string("/annotations/engine_name", &s));
    REQUIRE(s == "libmeasurement_kit");
  }
  {
    std::string s;
    REQUIRE(doc.get_string("/annotations/platform", &s));
    REQUIRE(s == "macos");
  }
  {
    size_t size = 0;
    REQUIRE(doc.get_size("/inputs", &size));
    REQUIRE(size == 2);
    {
      std::string s;
      REQUIRE(doc.get_string("/inputs/0", &s));
      REQUIRE(s == "www.kernel.org");
    }
    {
      std::string s;
      REQUIRE(doc.get_string("/inputs/1", &s));
      REQUIRE(s == "www.x.org");
    }
  }
  {
    std::string s;
    REQUIRE(doc.get_string("/name", &s));
    REQUIRE(s == "Ndt");
  }
  {
    int64_t value = 0;
    REQUIRE(doc.get_integer("/version_major", &value));
    REQUIRE(value == 17);
  }
  {
    double value = 0;
    REQUIRE(doc.get_float("/elapsed", &value));
    REQUIRE(value == 1.14);
  }
}

// Serialize
// ---------
//
// We can construct and serialize a JSON.

TEST_CASE("We can construct and serialize a JSON") {
  Json doc;
  REQUIRE(doc.set_string("/annotations/engine_name", "libmeasurement_kit"));
  REQUIRE(doc.set_string("/annotations/platform", "macos"));
  REQUIRE(doc.set_string("/inputs/0", "www.kernel.org"));
  REQUIRE(doc.set_string("/inputs/1", "www.x.org"));
  REQUIRE(doc.set_string("/name", "Ndt"));
  REQUIRE(doc.set_integer("/version_major", 17));
  REQUIRE(doc.set_float("/elapsed", 1.14));
  std::string s;
  REQUIRE(doc.serialize(&s));
  std::clog << s << std::endl;
}

// Non-UTF8-input
// --------------
//
// Make sure that, if we pass a non-UTF8-string to the JSON, the string is
// correctly encoded as base64 and we can dump a JSON. This is what we
// typically do in Measurement Kit and OONI to handle invalid UTF-8 data
// when we're serializing a measurement result.

TEST_CASE("We correctly deal with non-UTF8 input") {
  std::string input;
  {
    // This is a `gzip -9`-ed version of `build.gcc.ninja`.
    std::vector<uint8_t> input_vector = {
        31,  139, 8,   0,   8,   13,  239, 90,  2,   3,   101, 144, 205, 10,
        194, 48,  16,  132, 239, 121, 138, 57,  136, 151, 146, 74,  69,  68,
        10,  250, 10,  30,  61,  74,  154, 172, 18,  141, 73,  73,  19,  232,
        193, 135, 215, 212, 150, 254, 120, 9,   153, 229, 219, 153, 221, 245,
        209, 16,  100, 219, 50,  64,  186, 215, 75,  88,  133, 35,  238, 89,
        6,   126, 17,  198, 124, 95,  106, 131, 23,  224, 53,  41,  97,  131,
        150, 224, 77,  80,  71,  153, 101, 69,  1,   126, 222, 130, 75,  172,
        180, 5,   119, 88,  185, 24,  152, 79,  118, 70,  219, 231, 191, 223,
        143, 72,  244, 143, 18,  126, 198, 8,   15,  233, 23,  136, 143, 118,
        198, 228, 155, 148, 181, 61,  173, 11,  188, 17,  136, 18,  153, 27,
        119, 103, 172, 138, 218, 40,  84,  162, 161, 253, 238, 74,  86,  58,
        69,  185, 43,  211, 98,  139, 162, 172, 235, 30,  142, 225, 118, 184,
        42,  154, 162, 211, 210, 8,   26,  93,  61,  26,  103, 7,   104, 144,
        255, 128, 40,  211, 22,  139, 33,  230, 57,  163, 89,  223, 27,  168,
        9,   131, 115, 247, 31,  109, 147, 44,  187, 99,  142, 9,   61,  63,
        109, 254, 238, 95,  166, 75,  117, 138, 125, 0,   96,  224, 123, 120,
        208, 1,   0,   0};
    input = std::string((char *)input_vector.data(), input_vector.size());
    REQUIRE(input.size() == input_vector.size());
  }
  Json doc;
  REQUIRE(doc.set_string("/x/binary_value", std::move(input)));  // We can set
  {
    std::string s;
    REQUIRE(doc.serialize(&s));  // We can serialize
  }
  // Base64 of the above input, computed using OpenSSL. Compared to the
  // original OpenSSL output, I have removed the trailing newlines.
  std::string output =
      "H4sIAAgN71oCA2WQzQrCMBCE73mKOYiXkkpFRAr6Ch49SpqsEo1JSRPowYfX1Jb+"
      "eAmZ5duZ3fXREGTbMkC610tYhSPuWQZ+EcZ8X2qDF+A1KWGDluBNUEeZZUUBft6C"
      "S6y0BXdYuRiYT3ZG2+e/349I9I8SfsYID+kXiI92xuSblLU9rQu8EYgSmRt3Z6yK"
      "2ihUoqH97kpWOkW5K9Nii6Ks6x6O4Xa4Kpqi09IIGl09GmcHaJD/gCjTFosh5jmj"
      "Wd8bqAmDc/cfbZMsu2OOCT0/bf7uX6ZLdYp9AGDge3jQAQAA";
  {
    std::string s;
    REQUIRE(doc.get_string("/x/binary_value", &s));  // We can get
    REQUIRE(s == output);                            // Encoded as expected
  }
}
