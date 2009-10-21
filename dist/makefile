dirs := libxsde examples

ifneq ($(filter $(MAKECMDGOALS),gen test clean cleanobj),)
dirs += tests
endif

.PHONY: all $(dirs)
all: $(dirs)

tests: libxsde
examples: libxsde

$(dirs):
	$(MAKE) -C $@ $(MAKECMDGOALS)

makefile: ;
% :: $(dirs) ;
