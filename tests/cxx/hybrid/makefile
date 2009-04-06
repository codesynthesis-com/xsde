# file      : tests/cxx/hybrid/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../build/bootstrap.make

tests := sequences

ifeq ($(xsde_polymorphism),y)
tests += polymorphism
endif

ifeq ($(xsde_iostream),y)
tests += built-in default list recursive test-template union

ifeq ($(xsde_cdr),y)
tests += binary/cdr
endif

ifeq ($(xsde_xdr),y)
tests += binary/xdr
endif

ifeq ($(xsde_parser_validation),y)
tests += choice
endif

endif

default   := $(out_base)/
test      := $(out_base)/.test
clean     := $(out_base)/.clean

.PHONY: $(default) $(test) $(clean)

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(tests)))
$(test): $(addprefix $(out_base)/,$(addsuffix /.test,$(tests)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(tests)))

$(foreach t,$(tests),$(call import,$(src_base)/$t/makefile))
