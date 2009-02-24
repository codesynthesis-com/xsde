# file      : tests/cxx/serializer/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../build/bootstrap.make

tests := generated-driver generated-impl

ifeq ($(xsde_iostream),y)

tests +=        \
all             \
built-in        \
choice          \
error-handling  \
recursive       \
reset           \
restriction     \
sequence        \
test-template   \
wildcard

ifeq ($(xsde_reuse_style),tiein)
tests +=        \
complex         \
enumeration     \
list            \
union
endif

ifeq ($(xsde_polymorphic),y)
tests += polymorphism
endif

endif # xsde_iostream

ifeq ($(xsde_serializer_validation),y)
tests += validation
endif

default   := $(out_base)/
test      := $(out_base)/.test
clean     := $(out_base)/.clean

.PHONY: $(default) $(test) $(clean)

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(tests)))
$(test): $(addprefix $(out_base)/,$(addsuffix /.test,$(tests)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(tests)))

$(foreach t,$(tests),$(call import,$(src_base)/$t/makefile))
