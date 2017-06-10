#ifndef CIRCBUF_H
#define CIRCBUF_H

#include <stdint.h>
#include "config.h"

#ifndef CIRCBUF_SIZE
#error Define circular buffer size.
#endif

typedef uint8_t        circbuf_data_t;
typedef circbuf_data_t circbuf_array_t[CIRCBUF_SIZE];
typedef uint8_t        circbuf_index_t;

typedef struct circbuf_t {
	circbuf_array_t data;
	circbuf_index_t start;
	circbuf_index_t end;
} circbuf_t;


#define cbuf_wrapi(index)        do { if ((index) >= (CIRCBUF_SIZE)) (index) = 0; } while(0)
#define cbuf_nexti(index)        (((index) + 1) % (CIRCBUF_SIZE))

#define cbuf_incwrap(index)      do { (index)++; cbuf_wrapi(index); } while(0)

#define cbuf_availr(buf)         ((buf).start != (buf).end)
#define cbuf_availw(buf)         ((buf).start != cbuf_nexti((buf).end))

#define cbuf_read(buf,src)       do { (src) = (buf).data[(buf).start]; } while(0)
#define cbuf_write(buf,dest)     do { (buf).data[(buf).end] = (dest); } while(0)

#define cbuf_reada(buf,src) do {  \
	cbuf_read(buf,src);           \
	cbuf_incwrap((buf).start);    \
} while(0)
#define cbuf_writea(buf,dest) do { \
	cbuf_write(buf,dest);          \
	cbuf_incwrap((buf).end);       \
} while(0)

#define cbuf_reset(buf) do { (buf).start = 0; (buf).end = 0; } while(0)

#endif
