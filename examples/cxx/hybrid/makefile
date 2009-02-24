# file      : examples/cxx/hybrid/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../build/bootstrap.make

examples := compositors

ifeq ($(xsde_iostream),y)
ifeq ($(xsde_exceptions),y)

examples += hello multiroot streaming

ifeq ($(xsde_stl),y)
examples += library wildcard filter
endif

endif
endif

ifeq ($(xsde_stl),n)
ifeq ($(xsde_exceptions),n)
examples += minimal
endif
endif

default   := $(out_base)/
clean     := $(out_base)/.clean

.PHONY: $(default) $(clean)

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(examples)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(examples)))

$(foreach e,$(examples),$(call import,$(src_base)/$e/makefile))
