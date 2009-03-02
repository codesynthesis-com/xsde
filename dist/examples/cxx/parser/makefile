root := ../../..

include $(root)/build/config.make

dirs :=

ifneq ($(XSDE_REUSE_STYLE),none)
dirs += generated
endif

ifeq ($(XSDE_STL),y)
ifeq ($(XSDE_IOSTREAM),y)
ifeq ($(XSDE_EXCEPTIONS),y)

dirs += hello

ifneq ($(XSDE_REUSE_STYLE),none)
dirs += library multiroot

ifeq ($(XSDE_POLYMORPHIC),y)
dirs += polymorphism polyroot
endif

endif

ifeq ($(XSDE_REUSE_STYLE),tiein)
dirs += mixed wildcard
endif

endif
endif
endif

ifeq ($(XSDE_STL),n)
ifeq ($(XSDE_EXCEPTIONS),n)
ifneq ($(XSDE_REUSE_STYLE),none)
dirs += minimal
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
