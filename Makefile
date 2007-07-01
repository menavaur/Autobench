# Makefile for autobench scripts
VERSION = 2.1.2
PREFIX = /usr/local

# Automatically set correct paths for building Debian packages
ifdef DEB_BUILD_ARCH
  ROOTBIN_PREFIX = /usr
  ROOTMAN_PREFIX = /usr/share
  PREFIX = $(DESTDIR)
endif

BINDIR = $(PREFIX)$(ROOTBIN_PREFIX)/bin
MANDIR = $(PREFIX)$(ROOTMAN_PREFIX)/man/man1
ETCDIR = $(PREFIX)/etc

ifdef DEB_BUILD_ARCH
  export AB_CFG = /etc/autobench.conf
else 
  export AB_CFG = $(ETCDIR)/autobench.conf
endif

# Compiler & flags
CC=gcc
CFLAGS=-O2 -Wall
DEBUG_FLAGS=-Wall -g

#---------------------------------------------------------------------------
# Release related 
PKG_NAME = autobench
BDIR = build

PKG_VER = $(PKG_NAME)-$(VERSION)
PKG_DIR = $(BDIR)/$(PKG_VER)
PKG_TAR = nca-$(VERSION).tar.gz

REL_BUILD_DIR = $(HOME)/build
REL_DIR = $(PKG_VER)
REL_BDIR = $(REL_BUILD_DIR)/$(REL_DIR)
REL_TGZ = $(REL_DIR).tar.gz
CHANGELOG = $(REL_BDIR)/ChangeLog

all: crfile sesslog

crfile: crfile.c
	$(CC) $(CFLAGS) -o crfile crfile.c

sesslog: sesslog.c
	$(CC) $(CFLAGS) -o sesslog sesslog.c

debug: crfile.c
	$(CC) $(DEBUG_FLAGS) -o crfile crfile.c

install: crfile sesslog
	mkdir -p $(BINDIR) $(ETCDIR) $(MANDIR)
	perl -pi -e 's/my \$$MASTER_CONFIG =.*$$/my \$$MASTER_CONFIG = "$$ENV{AB_CFG}";/' autobench
	cp crfile autobench autobenchd autobench_admin sesslog bench2graph $(BINDIR)
	cp autobenchd.1 autobench_admin.1 crfile.1 autobench.1 sesslog.1 bench2graph.1 $(MANDIR)
	cp autobench.conf $(ETCDIR)

clean: 
	rm -f crfile sesslog
	find . -type f \( -name "*~" -o -name "#*" -o -name ".#*" \) -exec rm {} \;

uninstall: 
	cd $(BINDIR)
	rm -f crfile sesslog autobench bench2graph autobenchd autobench_admin
	cd $(MANDIR)
	rm -f crfile.1 sesslog.1 autobench.1 bench2graph.1 autobenchd.1 autobench_admin.1
	rm -fr $(ETCDIR)/autobench.conf

release: deb_package clean
	@if [ -e $(REL_BDIR) ]; then echo "Please delete $(REL_BDIR)"; exit 1; fi
	@if [ -e $(REL_BUILD_DIR)/$(REL_TGZ) ]; then echo "Please delete $(REL_BUILD_DIR)/$(REL_TGZ)"; exit 1; fi
	thisdir=$$(pwd); \
	ln -s $$thisdir $(REL_BDIR);
	cd $(REL_BUILD_DIR); \
	tar --exclude \{arch\} --exclude .arch-ids --exclude .todo --exclude +* --exclude ,* -cvhzf $(REL_TGZ) $(REL_DIR);\
	gpg -b --armor --sign $(REL_TGZ)
	rm $(REL_BDIR)

deb_package: clean
	dpkg-buildpackage -rfakeroot
	mv ../$(PKG_NAME)_$(VERSION)* $(REL_BUILD_DIR)
	fakeroot debian/rules clean
