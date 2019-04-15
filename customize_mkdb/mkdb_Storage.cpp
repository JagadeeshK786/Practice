#include "mkdb_Storage.h"
#include "redis_hash.h"
#include <memory>
#include <stdbool.h>
#include <utility>
/* Implementation for class mkdb_Storage */

#ifndef _Included_mkdb_Storage_Impl
#define _Included_mkdb_Storage_Impl

#define mkdb_redis_address "127.0.0.1"
#define mkdb_redis_port 6379
#define mkdb_redis_hash_name "mkdb"

std::unique_ptr<char[]> get_jbyte_array(JNIEnv *env, jbyteArray key) {
  auto buff = std::unique_ptr<char[]>(new char[env->GetArrayLength(key)]);
  env->GetByteArrayRegion(key, 0, env->GetArrayLength(key),
                          reinterpret_cast<jbyte *>(buff.get()));
  return buff;
}

jbyteArray new_jbyte_array(JNIEnv *env, weak_array src) {
  jbyteArray jbytes = env->NewByteArray(src.size);
  env->SetByteArrayRegion(jbytes, 0, src.size,
                          reinterpret_cast<jbyte *>(src.data));
  return jbytes;
}

redis_hash get_new_redis_hash() {
  return redis_hash(mkdb_redis_address, mkdb_redis_port, mkdb_redis_hash_name);
}

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     mkdb_Storage
 * Method:    walk
 * Signature: (JLmkdb/Storage/IWalk;)V
 */
JNIEXPORT void JNICALL Java_mkdb_Storage_walk(JNIEnv *env, jobject jthis,
                                              jlong jhandle, jobject jiw) {
  jclass clazz = env->GetObjectClass(jiw);
  jmethodID jiwalk_onrecord = env->GetMethodID(clazz, "onRecord", "([B[B)Z");
  if (jiwalk_onrecord == 0)
    return;
  redis_hash hash(get_new_redis_hash());
  hash.for_each([env, jiw, jiwalk_onrecord](const weak_array key,
                                            const weak_array value) -> bool {
    return env->CallBooleanMethod(jiw, jiwalk_onrecord,
                                  new_jbyte_array(env, key),
                                  new_jbyte_array(env, value)) == JNI_FALSE;
  });
}

/*
 * Class:     mkdb_Storage
 * Method:    browse
 * Signature: (JLmkdb/Storage/IWalk;I)V
 */
JNIEXPORT void JNICALL Java_mkdb_Storage_browse(JNIEnv *, jobject, jlong,
                                                jobject, jint) {
  // do nothing
}

/*
 * Class:     mkdb_Storage
 * Method:    exist
 * Signature: (J[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage_exist(JNIEnv *env, jobject, jlong,
                                                   jbyteArray key, jint) {
  return get_new_redis_hash().exists(
      weak_array(get_jbyte_array(env, key).get(), env->GetArrayLength(key)));
}

/*
 * Class:     mkdb_Storage
 * Method:    find
 * Signature: (J[BI)[B
 */
JNIEXPORT jbyteArray JNICALL Java_mkdb_Storage_find(JNIEnv *env, jobject, jlong,
                                                    jbyteArray key, jint) {
  auto result = get_new_redis_hash().find(
      weak_array(get_jbyte_array(env, key).get(), env->GetArrayLength(key)));
  if (!std::get<0>(result))
    return 0;
  return new_jbyte_array(env, std::get<1>(result));
}

/*
 * Class:     mkdb_Storage
 * Method:    replace
 * Signature: (J[BI[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage_replace(JNIEnv *env, jobject,
                                                     jlong, jbyteArray key,
                                                     jint, jbyteArray value,
                                                     jint) {
  auto is_success = get_new_redis_hash().insert_or_assign(
      weak_array(get_jbyte_array(env, key).get(), env->GetArrayLength(key)),
      weak_array(get_jbyte_array(env, value).get(),
                 env->GetArrayLength(value)));
  return true; // always return true
}

/*
 * Class:     mkdb_Storage
 * Method:    insert
 * Signature: (J[BI[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage_insert(JNIEnv *env, jobject, jlong,
                                                    jbyteArray key, jint,
                                                    jbyteArray value, jint) {
  return get_new_redis_hash().insert(
      weak_array(get_jbyte_array(env, key).get(), env->GetArrayLength(key)),
      weak_array(get_jbyte_array(env, value).get(),
                 env->GetArrayLength(value)));
}

/*
 * Class:     mkdb_Storage
 * Method:    remove
 * Signature: (J[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage_remove(JNIEnv *env, jobject, jlong,
                                                    jbyteArray key, jint) {
  return get_new_redis_hash().erase(
      weak_array(get_jbyte_array(env, key).get(), env->GetArrayLength(key)));
}

/*
 * Class:     mkdb_Storage
 * Method:    open
 * Signature: (JLjava/lang/String;II)J
 */
JNIEXPORT jlong JNICALL Java_mkdb_Storage_open(JNIEnv *, jobject, jlong,
                                               jstring, jint, jint) {
  return 9710;
}

/*
 * Class:     mkdb_Storage
 * Method:    close
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mkdb_Storage_close(JNIEnv *, jobject, jlong) {}

/*
 * Class:     mkdb_Storage
 * Method:    snapshot_create
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mkdb_Storage_snapshot_1create(JNIEnv *, jobject,
                                                          jlong) {}

/*
 * Class:     mkdb_Storage
 * Method:    snapshot_release
 * Signature: (J)V
 */
JNIEXPORT void JNICALL Java_mkdb_Storage_snapshot_1release(JNIEnv *, jobject,
                                                           jlong) {}

/*
 * Class:     mkdb_Storage
 * Method:    _walk
 * Signature: (JLmkdb/Storage/IWalk;)V
 */
JNIEXPORT void JNICALL Java_mkdb_Storage__1walk(JNIEnv *, jobject, jlong,
                                                jobject) {}

/*
 * Class:     mkdb_Storage
 * Method:    _browse
 * Signature: (JLmkdb/Storage/IWalk;I)V
 */
JNIEXPORT void JNICALL Java_mkdb_Storage__1browse(JNIEnv *, jobject, jlong,
                                                  jobject, jint) {}

/*
 * Class:     mkdb_Storage
 * Method:    _find
 * Signature: (J[BI)[B
 */
JNIEXPORT jbyteArray JNICALL Java_mkdb_Storage__1find(JNIEnv *, jobject, jlong,
                                                      jbyteArray, jint) {
  return 0;
}

/*
 * Class:     mkdb_Storage
 * Method:    _replace
 * Signature: (J[BI[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage__1replace(JNIEnv *, jobject, jlong,
                                                       jbyteArray, jint,
                                                       jbyteArray, jint) {
  return false;
}

/*
 * Class:     mkdb_Storage
 * Method:    _insert
 * Signature: (J[BI[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage__1insert(JNIEnv *, jobject, jlong,
                                                      jbyteArray, jint,
                                                      jbyteArray, jint) {
  return false;
}

/*
 * Class:     mkdb_Storage
 * Method:    firstKey
 * Signature: (J)[B
 */
JNIEXPORT jbyteArray JNICALL Java_mkdb_Storage_firstKey(JNIEnv *, jobject,
                                                        jlong) {
  return 0;
}

/*
 * Class:     mkdb_Storage
 * Method:    nextKey
 * Signature: (J[BI)[B
 */
JNIEXPORT jbyteArray JNICALL Java_mkdb_Storage_nextKey(JNIEnv *, jobject, jlong,
                                                       jbyteArray, jint) {
  return 0;
}

/*
 * Class:     mkdb_Storage
 * Method:    _compress
 * Signature: ([BI)[B
 */
JNIEXPORT jbyteArray JNICALL Java_mkdb_Storage__1compress(JNIEnv *, jclass,
                                                          jbyteArray, jint) {
  return 0;
}

/*
 * Class:     mkdb_Storage
 * Method:    _uncompress
 * Signature: ([BI)[B
 */
JNIEXPORT jbyteArray JNICALL Java_mkdb_Storage__1uncompress(JNIEnv *, jclass,
                                                            jbyteArray, jint) {
  return 0;
}

#ifdef __cplusplus
}
#endif
#endif
