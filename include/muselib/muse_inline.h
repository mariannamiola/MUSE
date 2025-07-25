#ifndef MUSE_INLINE_H
#define MUSE_INLINE_H
#endif

// hacky trick to define uint in windows systems.
// NOTE: this belongs elsewhere!
#ifdef _WIN32
typedef unsigned int uint;
#endif

#ifndef STATIC_MUSELIB
#define MUSE_INLINE inline
#else
#define MUSE_INLINE
#endif
