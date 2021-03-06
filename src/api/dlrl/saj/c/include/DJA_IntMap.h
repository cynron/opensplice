/*
 *                         OpenSplice DDS
 *
 *   This software and documentation are Copyright 2006 to 2013 PrismTech
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $OSPL_HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class DDS_IntMap */

#ifndef _Included_DDS_IntMap
#define _Included_DDS_IntMap
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     DDS_IntMap
 * Method:    jniLength
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_DDS_IntMap_jniLength
  (JNIEnv *, jobject);

/*
 * Class:     DDS_IntMap
 * Method:    jniClear
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_DDS_IntMap_jniClear
  (JNIEnv *, jobject);

/*
 * Class:     DDS_IntMap
 * Method:    jniAddedElements
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_DDS_IntMap_jniAddedElements
  (JNIEnv *, jobject);

/*
 * Class:     DDS_IntMap
 * Method:    jniModifiedElements
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_DDS_IntMap_jniModifiedElements
  (JNIEnv *, jobject);

/*
 * Class:     DDS_IntMap
 * Method:    jniGetKeys
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_DDS_IntMap_jniGetKeys
  (JNIEnv *, jobject);

/*
 * Class:     DDS_IntMap
 * Method:    jniRemove
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_DDS_IntMap_jniRemove
  (JNIEnv *, jobject, jint);

/*
 * Class:     DDS_IntMap
 * Method:    jniDeleteIntMap
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_DDS_IntMap_jniDeleteIntMap
  (JNIEnv *, jobject);

/*
 * Class:     DDS_IntMap
 * Method:    jniRemovedElements
 * Signature: ()[I
 */
JNIEXPORT jintArray JNICALL Java_DDS_IntMap_jniRemovedElements
  (JNIEnv *, jobject);

/*
 * Class:     DDS_IntMap
 * Method:    jniGetValues
 * Signature: ()[LDDS/ObjectRoot;
 */
JNIEXPORT jobjectArray JNICALL Java_DDS_IntMap_jniGetValues
  (JNIEnv *, jobject);

/*
 * Class:     DDS_IntMap
 * Method:    jniGet
 * Signature: (I)LDDS/ObjectRoot;
 */
JNIEXPORT jobject JNICALL Java_DDS_IntMap_jniGet
  (JNIEnv *, jobject, jint);

/*
 * Class:     DDS_IntMap
 * Method:    jniPut
 * Signature: (ILDDS/ObjectRoot;)V
 */
JNIEXPORT void JNICALL Java_DDS_IntMap_jniPut
  (JNIEnv *, jobject, jint, jobject);

#ifdef __cplusplus
}
#endif
#endif
