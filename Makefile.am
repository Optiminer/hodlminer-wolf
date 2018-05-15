AM_CFLAGS = -Wall

if WANT_JANSSON
JANSSON_INCLUDES= -I$(top_srcdir)/compat/jansson
else
JANSSON_INCLUDES=
endif

EXTRA_DIST	= example-cfg.json nomacro.pl

SUBDIRS		= compat

AM_CFLAGS 	+= $(PTHREAD_FLAGS) -march=native -Ofast -flto -std=gnu11 $(JANSSON_INCLUDES)

bin_PROGRAMS	= hodlminer

dist_man_MANS	= hodlminer.1

hodlminer_SOURCES	= elist.h miner.h compat.h \
		  	  sha2.c hodl-miner.c util.c hodl.c aes.c sha512_sse2.c sha512_avx2.c

hodlminer_LDFLAGS	= $(PTHREAD_FLAGS)
hodlminer_LDADD		= @LIBCURL@ @JANSSON_LIBS@ @PTHREAD_LIBS@ @WS2_LIBS@ -lssl -lcrypto
hodlminer_CPPFLAGS	= @LIBCURL_CPPFLAGS@
