/* This file includes all the needed networking stuff */

#ifndef H_NET_H
#define H_NET_H

/* Include the socket buffer library */
#include "sockbuf.h"

/* Include the socket library for the correct OS */
#ifdef WIN32
# include "net-win.h"
#else
# ifdef MSDOS
#  include "net-ibm.h"
# else
#  include "net-unix.h"
# endif
#endif

/* Include the various packet types and error codes */
#include "pack.h"

/* Include some bit-manipulation functions used in the networking code */
#include "bit.h"

#endif
