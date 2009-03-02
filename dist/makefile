dirs := libxsde examples

.PHONY: all $(dirs)
all: $(dirs)

examples: libxsde

$(dirs):
	$(MAKE) -C $@ $(MAKECMDGOALS)

makefile: ;
% :: $(dirs) ;
