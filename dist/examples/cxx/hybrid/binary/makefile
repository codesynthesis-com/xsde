root := ../../../..

include $(root)/build/config.make

dirs :=

ifeq ($(XSDE_IOSTREAM),y)
ifeq ($(XSDE_EXCEPTIONS),y)

dirs += custom

ifeq ($(XSDE_CDR),y)
dirs += cdr
endif

ifeq ($(XSDE_XDR),y)
dirs += xdr
endif

endif
endif

.PHONY: all $(dirs)

all: $(dirs)

$(dirs):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

makefile: ;
%.make:: ;

%:: $(dirs) ;
