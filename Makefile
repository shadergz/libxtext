prefix = /usr/local

libinc = include

libname = libbin

lib_objs = src/generic.o\
	src/binio.o\
	src/error.o\
	src/memhand.o\
	src/unload.o\
	src/identify.o\
	src/elfhandler.o\
	src/fshand.o\
	src/binparser.o\
	src/binman.o

libver = 0.0.2

override CFLAGS=-fPIC -Wall -Werror -pedantic -std=gnu11 -g -I$(libinc)

libobject = $(libname).$(libver).so

all: $(libobject)

$(libobject): $(lib_objs)
	$(CC) $(CFLAGS) -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

install:
	install -m 644 $(libobject) $(prefix)/lib/$(libobject)
	ln -sf $(prefix)/lib/$(libobject) $(prefix)/lib/$(libname).so
	ln -sf $(prefix)/lib/$(libobject) $(prefix)/lib/$(libname).0.so

	if [ ! -d $(prefix)/include ]; then \
		mkdir $(prefix)/include; \
	fi

	if [ ! -d $(prefix)/include/bin ]; then \
		mkdir $(prefix)/include/bin; \
	fi
	
	install -m 644 $(libinc)/bin/* $(prefix)/include/bin

uninstall:
	rm -f $(prefix)/lib/$(libname)*
	rm -rf $(prefix)/include/bin*

clean:
	rm -f $(lib_objs)\
		$(libobject)

