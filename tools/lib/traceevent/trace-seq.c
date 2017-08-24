/*
 * Copyright (C) 2009 Red Hat Inc, Steven Rostedt <srostedt@redhat.com>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation;
 * version 2.1 of the License (not later!)
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not,  see <http://www.gnu.org/licenses>
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include "event-parse.h"
#include "event-utils.h"

/*
                                                    
                                                
 */
#define TRACE_SEQ_POISON	((void *)0xdeadbeef)
#define TRACE_SEQ_CHECK(s)						\
do {									\
	if ((s)->buffer == TRACE_SEQ_POISON)			\
		die("Usage of trace_seq after it was destroyed");	\
} while (0)

/* 
                                                      
                                                         
 */
void trace_seq_init(struct trace_seq *s)
{
	s->len = 0;
	s->readpos = 0;
	s->buffer_size = TRACE_SEQ_BUF_SIZE;
	s->buffer = malloc_or_die(s->buffer_size);
}

/* 
                                                    
                                                    
  
                                                          
 */
void trace_seq_destroy(struct trace_seq *s)
{
	if (!s)
		return;
	TRACE_SEQ_CHECK(s);
	free(s->buffer);
	s->buffer = TRACE_SEQ_POISON;
}

static void expand_buffer(struct trace_seq *s)
{
	s->buffer_size += TRACE_SEQ_BUF_SIZE;
	s->buffer = realloc(s->buffer, s->buffer_size);
	if (!s->buffer)
		die("Can't allocate trace_seq buffer memory");
}

/* 
                                                            
                                
                             
  
                                                        
                      
  
                                                           
                                                          
                                                           
                                                     
                                               
 */
int
trace_seq_printf(struct trace_seq *s, const char *fmt, ...)
{
	va_list ap;
	int len;
	int ret;

	TRACE_SEQ_CHECK(s);

 try_again:
	len = (s->buffer_size - 1) - s->len;

	va_start(ap, fmt);
	ret = vsnprintf(s->buffer + s->len, len, fmt, ap);
	va_end(ap);

	if (ret >= len) {
		expand_buffer(s);
		goto try_again;
	}

	s->len += ret;

	return 1;
}

/* 
                                                             
                                
                             
  
                                                           
                                                          
                                                           
                                                     
                                               
 */
int
trace_seq_vprintf(struct trace_seq *s, const char *fmt, va_list args)
{
	int len;
	int ret;

	TRACE_SEQ_CHECK(s);

 try_again:
	len = (s->buffer_size - 1) - s->len;

	ret = vsnprintf(s->buffer + s->len, len, fmt, args);

	if (ret >= len) {
		expand_buffer(s);
		goto try_again;
	}

	s->len += ret;

	return len;
}

/* 
                                                            
                                
                                
  
                                                               
                                                               
                                                                
                      
 */
int trace_seq_puts(struct trace_seq *s, const char *str)
{
	int len;

	TRACE_SEQ_CHECK(s);

	len = strlen(str);

	while (len > ((s->buffer_size - 1) - s->len))
		expand_buffer(s);

	memcpy(s->buffer + s->len, str, len);
	s->len += len;

	return len;
}

int trace_seq_putc(struct trace_seq *s, unsigned char c)
{
	TRACE_SEQ_CHECK(s);

	while (s->len >= (s->buffer_size - 1))
		expand_buffer(s);

	s->buffer[s->len++] = c;

	return 1;
}

void trace_seq_terminate(struct trace_seq *s)
{
	TRACE_SEQ_CHECK(s);

	/*                                                 */
	s->buffer[s->len] = 0;
}

int trace_seq_do_printf(struct trace_seq *s)
{
	TRACE_SEQ_CHECK(s);
	return printf("%.*s", s->len, s->buffer);
}
