root := ../../..

include $(root)/build/config.make

dirs := compositors

ifeq ($(XSDE_IOSTREAM),y)
ifeq ($(XSDE_EXCEPTIONS),y)

dirs += hello multiroot streaming

ifeq ($(XSDE_STL),y)
dirs += library wildcard filter
endif

endif
endif

ifeq ($(XSDE_STL),n)
ifeq ($(XSDE_EXCEPTIONS),n)
dirs += minimal
endif
endif


.PHONY: all $(dirs)

all: $(dirs)

$(dirs):
	@$(MAKE) -C $@ $(MAKECMDGOALS)

makefile: ;
%.make:: ;

%:: $(dirs) ;
