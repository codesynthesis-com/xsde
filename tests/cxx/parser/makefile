# file      : tests/cxx/parser/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../build/bootstrap.make

tests := generated-impl duplicate

ifeq ($(xsde_iostream),y)
ifeq ($(xsde_exceptions),y)
tests +=      \
built-in      \
recursive     \
test-template \
union

ifeq ($(xsde_reuse_style),tiein)
tests +=      \
enumeration   \
list
endif

endif # exceptions

ifeq ($(xsde_polymorphic),y)
tests += polymorphism
endif

tests += reset

endif # iostream


ifeq ($(xsde_parser_validation),y)
tests += validation

ifeq ($(xsde_iostream),y)
tests += error-handling

ifeq ($(xsde_exceptions),y)
tests += name-clash/inheritance
endif
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
