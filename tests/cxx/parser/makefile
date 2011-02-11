# file      : tests/cxx/parser/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../build/bootstrap.make

# NOTE: remember to update dist/tests/cxx/parser/{makefile,nmakefile} if 
# you change anything here.
#

all_tests := generated-impl duplicate built-in recursive test-template \
union enumeration list polymorphism reset validation error-handling \
name-clash/inheritance

build_tests := generated-impl duplicate

ifeq ($(xsde_iostream),y)
ifeq ($(xsde_exceptions),y)
build_tests += \
built-in       \
recursive      \
test-template  \
union

ifeq ($(xsde_reuse_style),tiein)
build_tests += \
enumeration    \
list
endif

endif # exceptions

ifeq ($(xsde_polymorphic),y)
ifeq ($(xsde_reuse_style),tiein)
build_tests += polymorphism polyrecur
endif
endif

build_tests += reset

endif # iostream


ifeq ($(xsde_parser_validation),y)
build_tests += validation

ifeq ($(xsde_iostream),y)
build_tests += error-handling

ifeq ($(xsde_exceptions),y)
build_tests += name-clash/inheritance
endif
endif
endif

default   := $(out_base)/
test      := $(out_base)/.test
dist      := $(out_base)/.dist
dist-win  := $(out_base)/.dist-win
clean     := $(out_base)/.clean

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(build_tests)))
$(test): $(addprefix $(out_base)/,$(addsuffix /.test,$(build_tests)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(build_tests)))
$(dist): $(addprefix $(out_base)/,$(addsuffix /.dist,$(all_tests)))
$(dist-win): $(addprefix $(out_base)/,$(addsuffix /.dist-win,$(all_tests)))

ifneq ($(filter $(MAKECMDGOALS),dist dist-win),)
$(foreach t,$(all_tests),$(call import,$(src_base)/$t/makefile))
else
$(foreach t,$(build_tests),$(call import,$(src_base)/$t/makefile))
endif
