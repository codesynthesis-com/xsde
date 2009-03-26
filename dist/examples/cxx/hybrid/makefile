root := ../../..

include $(root)/build/config.make

dirs := binary compositors custom

ifeq ($(XSDE_IOSTREAM),y)
ifeq ($(XSDE_EXCEPTIONS),y)

dirs += hello multiroot streaming

ifeq ($(XSDE_STL),y)
dirs += library wildcard filter
endif

ifeq ($(XSDE_POLYMORPHIC),y)
dirs += polymorphism polyroot
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
