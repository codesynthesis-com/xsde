ifneq ($(MAKECMDGOALS),install)
dirs := libxsde examples
else
dirs := libxsde
endif

.PHONY: all $(dirs)
all: $(dirs)

tests: libxsde
examples: libxsde

$(dirs):
	$(MAKE) -C $@ $(MAKECMDGOALS)

makefile: ;
% :: $(dirs) ;

# Install.
#
install_prefix := /usr/local
install_bin := $(install_prefix)/bin
install_man := $(install_prefix)/man

# Note that the xsde binary may come from a separate package (.deb/.rpm).
#
.PHONY: install
install: $(dirs)
	if test -f bin/xsde; then
	  ./install-sh -d -m 755 $(install_bin)
	  ./install-sh -m 755 bin/xsde $(install_bin)/xsde
	  ./install-sh -d -m 755 $(install_man)/man1
	  ./install-sh -m 644 doc/xsde.1 $(install_man)/man1/xsde.1
	fi
