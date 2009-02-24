# file      : tests/cxx/parser/validation/built-in/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../../build/bootstrap.make

tests := binary boolean byte date-time float int integer qname short \
string uri

ifeq ($(xsde_iostream),y)
ifeq ($(xsde_exceptions),y)
ifeq ($(xsde_reuse_style),tiein)
tests += any-type
endif
endif
endif

ifeq ($(xsde_longlong),y)
tests += long-long
else
tests += long
endif

default   := $(out_base)/
test      := $(out_base)/.test
clean     := $(out_base)/.clean

.PHONY: $(default) $(test) $(clean)

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(tests)))
$(test): $(addprefix $(out_base)/,$(addsuffix /.test,$(tests)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(tests)))

$(foreach t,$(tests),$(call import,$(src_base)/$t/makefile))
