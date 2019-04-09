#define REDIS_HASH_NAME MKDB
#ifndef _REDIS_HASH
#define _REDIS_HASH
#include "hiredis/hiredis.h"
#include <exception>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <type_traits>
#include <vector>

namespace detail {

// need to explicitly specify type of value
template <typename _ValueType>
std::unique_ptr<_ValueType, void (*)(_ValueType *)>
make_unique_with_deleter(_ValueType *value, void (*deleter)(_ValueType *)) {
  return std::unique_ptr<_ValueType, void (*)(_ValueType *)>(value, deleter);
}

std::unique_ptr<redisReply, void (*)(redisReply *)>
make_unique_reply(void *reply) {
  return make_unique_with_deleter<redisReply>(
      static_cast<redisReply *>(reply),
      [](redisReply *object) { freeReplyObject(object); });
}

} // namespace detail

struct weak_array {
  weak_array(const char *in_data, uint32_t in_size)
      : data(const_cast<char *>(in_data)), size(in_size) {}

  char *data;
  uint32_t size;
};

struct byte_array : public weak_array {
  byte_array(const char *in_data, uint32_t in_size)
      : weak_array(static_cast<const char *>(malloc(in_size)), in_size) {
    memcpy(this->data, in_data, in_size);
  }

  byte_array(byte_array &&rhs) : weak_array(rhs.data, rhs.size) {
    rhs.data = 0;
    rhs.~byte_array();
  }

  ~byte_array() {
    free(data);
    this->data = 0;
  }
};

class redis_hash {
public:
  redis_hash(const char *address, uint32_t port, const char *input_hash_name)
      : hash_name(input_hash_name, strlen(input_hash_name)) {
    context = redisConnect(address, port);
    if (!context)
      throw std::bad_alloc();
    if (context->err)
      // https://stackoverflow.com/questions/10644910/does-stdruntime-error-copy-the-string-passed-in-the-constructor
      throw std::runtime_error(context->errstr);
  }

  redis_hash(const redis_hash &) = delete;

  redis_hash &operator=(const redis_hash &) = delete;

  ~redis_hash() { redisFree(context); }

public:
  bool insert(const weak_array key, const weak_array value) {
    std::string command_builder("HSETNX ");
    command_builder.append(this->hash_name.data, this->hash_name.size);
    command_builder.append(" ");
    command_builder.append(key.data, key.size);
    command_builder.append(" ");
    command_builder.append(value.data, value.size);

    auto reply = detail::make_unique_reply(
        redisCommand(this->context, command_builder.c_str()));
    if (!reply || reply->type != REDIS_REPLY_INTEGER)
      throw std::runtime_error("command failed");
    return reply->integer == 1;
  }

  bool insert_or_assign(const weak_array key, const weak_array value) {}

  bool erase(const weak_array key) {}

  byte_array find(const weak_array key) { return byte_array("", 0); }

  void for_each(void (*callback)(const weak_array, const weak_array)) {
    // callback("key", "value");
  }

private:
  byte_array hash_name;
  redisContext *context;
};

#endif