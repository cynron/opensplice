/*
 * in_abstractSendBufferBasic.h
 *
 *  Created on: Feb 8, 2009
 *      Author: frehberg
 */

#ifndef IN_SENDBUFFER_H_
#define IN_SENDBUFFER_H_

#include "in_abstractSendBuffer.h"
#include "in_commonTypes.h"

/**
 * Allow usage of this C code from C++ code. Always include this in a header
 * file.
 */
#if defined (__cplusplus)
extern "C" {
#endif

#define in_sendBuffer(_obj) \
	(in_sendBuffer)(_obj)

/** \brief constructor
 * \param poritive integer between 0..2^16-1 */
in_sendBuffer
in_sendBufferNew(in_long capacity);

/** \brief destructor */
void
in_sendBufferFree(in_sendBuffer _this);

/* \brief reset the length attribute and check locator's refcounter is set to 1 */
void
in_sendBufferReset(in_sendBuffer _this);

/* Close the brace that allows the usage of this code in C++. */
#if defined (__cplusplus)
}
#endif

#endif /* IN_SENDBUFFER_H_ */