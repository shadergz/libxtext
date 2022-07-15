prefix = /usr/local

libinc = include

libname = libbin

lib_objs = generic.o binio.o error.o memhand.o unload.o identify.o elfhandler.o fshand.o binparser.o binman.o

libver = 0.1

override CFLAGS=-Wall -Werror -pedantic -std=gnu11 -I$(libinc)

libobject = $(libname).$(libver).so

all: $(libobject)

$(libobject): $(lib_objs)
	$(CC) $(CFLAGS) -fPIC -shared -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

install:
	install -m 644 $(libobject) $(prefix)/lib/$(libobject)
	ln -sf $(prefix)/lib/$(libobject) $(prefix)/lib/$(libname).so
	ln -sf $(prefix)/lib/$(libobject) $(prefix)/lib/$(libname).0.so

	if [ ! -d $(prefix)/include ]; then \
		mkdir $(prefix)/include; \
	fi

	if [ ! -d $(prefix)/include/$(libname) ]; then \
		mkdir $(prefix)/include/$(libname); \
	fi
	
	install -m 644 $(libinc)/libbin/* $(prefix)/include/$(libname)

uninstall:
	rm -f $(prefix)/lib/$(libname)*
	rm -rf $(prefix)/include/$(libname)*

clean:
	rm -f $(lib_objs)\
		$(libobject)

