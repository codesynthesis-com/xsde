# file      : examples/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

examples := cxx

default   := $(out_base)/
test      := $(out_base)/.test
clean     := $(out_base)/.clean

.PHONY: $(default) $(test) $(clean)

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(examples)))
$(test): $(addprefix $(out_base)/,$(addsuffix /.test,$(examples)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(examples)))

$(foreach e,$(examples),$(call import,$(src_base)/$e/makefile))
