root := ../../../..

include $(root)/build/config.make

dirs :=

ifeq ($(XSDE_IOSTREAM),y)
ifeq ($(XSDE_EXCEPTIONS),y)

dirs += wildcard

endif
endif

.PHONY: all $(dirs)

all: $(dirs)

$(dirs):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

makefile: ;
%.make:: ;

%:: $(dirs) ;
