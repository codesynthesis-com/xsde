# file      : examples/cxx/parser/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../build/bootstrap.make

examples :=

ifneq ($(xsde_reuse_style),none)
examples += generated
endif

ifeq ($(xsde_stl),y)
ifeq ($(xsde_iostream),y)
ifeq ($(xsde_exceptions),y)

examples += hello

ifneq ($(xsde_reuse_style),none)
examples += library multiroot

ifeq ($(xsde_polymorphic),y)
examples += polymorphism polyroot
endif

endif

ifeq ($(xsde_reuse_style),tiein)
examples += mixed wildcard
endif

endif
endif
endif

ifeq ($(xsde_stl),n)
ifeq ($(xsde_exceptions),n)
ifneq ($(xsde_reuse_style),none)
examples += minimal
endif
endif
endif

default   := $(out_base)/
clean     := $(out_base)/.clean

.PHONY: $(default) $(clean)

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(examples)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(examples)))

$(foreach e,$(examples),$(call import,$(src_base)/$e/makefile))
