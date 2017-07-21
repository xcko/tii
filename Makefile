# tii - tiny ii frontend

include config.mk

SRC = tii.c
OBJ = ${SRC:.c=.o}

all: options tii

options:
	@echo tii build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

tii: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f tii ${OBJ} tii-${VERSION}.tar.gz

dist: clean
	@echo creating dist tarball
	@mkdir -p tii-${VERSION}
	@cp -R LICENSE Makefile README config.mk tii.1 tii.c tii-${VERSION}
	@tar -cf tii-${VERSION}.tar tii-${VERSION}
	@gzip tii-${VERSION}.tar
	@rm -rf tii-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f tii ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/tii
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < tii.1 > ${DESTDIR}${MANPREFIX}/man1/tii.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/tii.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/tii
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/tii.1

.PHONY: all options clean dist install uninstall
