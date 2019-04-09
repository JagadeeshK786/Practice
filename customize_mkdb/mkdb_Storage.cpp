#include "mkdb_Storage.h"
#include "redis_hash.h"
#include <stdbool.h>
/* Implementation for class mkdb_Storage */

#ifndef _Included_mkdb_Storage_Impl
#define _Included_mkdb_Storage_Impl
#ifdef __cplusplus
extern "C" {
#endif

/*
 * Class:     mkdb_Storage
 * Method:    walk
 * Signature: (JLmkdb/Storage/IWalk;)V
 */
JNIEXPORT void JNICALL Java_mkdb_Storage_walk(JNIEnv *, jobject, jlong,
                                              jobject) {
  // foreach
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
JNIEXPORT jboolean JNICALL Java_mkdb_Storage_exist(JNIEnv *, jobject, jlong,
                                                   jbyteArray, jint) {
  return false;
}

/*
 * Class:     mkdb_Storage
 * Method:    find
 * Signature: (J[BI)[B
 */
JNIEXPORT jbyteArray JNICALL Java_mkdb_Storage_find(JNIEnv *, jobject, jlong,
                                                    jbyteArray, jint) {
  return 0;
}

/*
 * Class:     mkdb_Storage
 * Method:    replace
 * Signature: (J[BI[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage_replace(JNIEnv *, jobject, jlong,
                                                     jbyteArray, jint,
                                                     jbyteArray, jint) {
  return false;
}

/*
 * Class:     mkdb_Storage
 * Method:    insert
 * Signature: (J[BI[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage_insert(JNIEnv *, jobject, jlong,
                                                    jbyteArray, jint,
                                                    jbyteArray, jint) {
  return false;
}

/*
 * Class:     mkdb_Storage
 * Method:    remove
 * Signature: (J[BI)Z
 */
JNIEXPORT jboolean JNICALL Java_mkdb_Storage_remove(JNIEnv *, jobject, jlong,
                                                    jbyteArray, jint) {
  return false;
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
