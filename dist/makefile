dirs := libxsde examples

ifneq ($(filter $(MAKECMDGOALS),test clean),)
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
