#define REDIS_HASH_NAME MKDB
#ifndef _REDIS_HASH
#define _REDIS_HASH
#include "hiredis/hiredis.h"
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <type_traits>
#include <utility>
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

bool check_types(int type) { return true; }

template <typename _CurrentType, typename... _ExpectedReplyTypes>
bool check_types(int type, _CurrentType current_type,
                 _ExpectedReplyTypes... rest_reply_types) {
  static_assert(std::is_same_v<_CurrentType, int>,
                "the _ExpectedReplyTypes should be all int");
  if (current_type == type)
    return true;
  if (sizeof...(_ExpectedReplyTypes) == 0)
    return false;
  return check_types(type, rest_reply_types...);
}

template <int... _ExpectedReplyTypes> void verify_reply(redisReply *reply) {
  if (reply->type == REDIS_REPLY_ERROR)
    throw std::runtime_error(reply->str);
  if (!reply)
    throw std::runtime_error("command failed");
  if (!check_types(reply->type, _ExpectedReplyTypes...))
    throw std::runtime_error("reply->type not as expect");
}

} // namespace detail

struct weak_array {
  weak_array(const char *in_data)
      : data(const_cast<char *>(in_data)), size(strlen(in_data)) {}

  weak_array(const char *in_data, uint32_t in_size)
      : data(const_cast<char *>(in_data)), size(in_size) {}

  char *data;
  uint32_t size;
};

struct byte_array : public weak_array {
  byte_array() : byte_array(nullptr, 0) {}

  byte_array(const char *in_data) : byte_array(in_data, strlen(in_data)) {}

  byte_array(const char *in_data, uint32_t in_size)
      : weak_array(std::launder(static_cast<const char *>(malloc(in_size))),
                   in_size) {
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
    detail::verify_reply<REDIS_REPLY_INTEGER>(reply.get());
    return reply->integer == 1;
  }

  bool insert_or_assign(const weak_array key, const weak_array value) {
    std::string command_builder("HSET ");
    command_builder.append(this->hash_name.data, this->hash_name.size);
    command_builder.append(" ");
    command_builder.append(key.data, key.size);
    command_builder.append(" ");
    command_builder.append(value.data, value.size);

    auto reply = detail::make_unique_reply(
        redisCommand(this->context, command_builder.c_str()));
    detail::verify_reply<REDIS_REPLY_INTEGER>(reply.get());

    return reply->integer == 1;
  }

  bool erase(const weak_array key) {
    std::string command_builder("HDEL ");
    command_builder.append(this->hash_name.data, this->hash_name.size);
    command_builder.append(" ");
    command_builder.append(key.data, key.size);

    auto reply = detail::make_unique_reply(
        redisCommand(this->context, command_builder.c_str()));
    detail::verify_reply<REDIS_REPLY_INTEGER>(reply.get());

    return reply->integer == 1;
  }

  bool exists(const weak_array key) {
    std::string command_builder("HEXISTS ");
    command_builder.append(this->hash_name.data, this->hash_name.size);
    command_builder.append(" ");
    command_builder.append(key.data, key.size);

    auto reply = detail::make_unique_reply(
        redisCommand(this->context, command_builder.c_str()));
    detail::verify_reply<REDIS_REPLY_INTEGER>(reply.get());

    return reply->integer == 1;
  }

  std::pair<bool, byte_array> find(const weak_array key) {
    std::string command_builder("HGET ");
    command_builder.append(this->hash_name.data, this->hash_name.size);
    command_builder.append(" ");
    command_builder.append(key.data, key.size);

    auto reply = detail::make_unique_reply(
        redisCommand(this->context, command_builder.c_str()));
    detail::verify_reply<REDIS_REPLY_NIL, REDIS_REPLY_STRING>(reply.get());

    if (reply->type == REDIS_REPLY_NIL)
      return std::pair<bool, byte_array>{false, byte_array()};
    return std::pair<bool, byte_array>{true,
                                       byte_array(reply->str, reply->len)};
  }

  // if callback returns false,stop foreach
  // bool(const weak_array,const weak_array)
  template <typename _Callback> void for_each(_Callback callback) {
    std::string command_builder("HGETALL ");
    command_builder.append(this->hash_name.data, this->hash_name.size);

    auto reply = detail::make_unique_reply(
        redisCommand(this->context, command_builder.c_str()));
    detail::verify_reply<REDIS_REPLY_NIL, REDIS_REPLY_ARRAY>(reply.get());
    if (reply->elements % 2 != 0)
      throw std::runtime_error(
          "reply->elements % 2 != 0 in redis_hash::for_each");

    for (uint32_t i = 0; i < reply->elements; i += 2) {
      redisReply *key = reply->element[i], *value = reply->element[i + 1];
      detail::verify_reply<REDIS_REPLY_STRING>(key);
      detail::verify_reply<REDIS_REPLY_STRING>(value);

      if (callback(weak_array(key->str, key->len),
                   weak_array(value->str, value->len)))
        break;
    }
  }

private:
  byte_array hash_name;
  redisContext *context;
};

#endif