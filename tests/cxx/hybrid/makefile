# file      : tests/cxx/hybrid/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2010 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../build/bootstrap.make

# NOTE: remember to update dist/tests/cxx/hybrid/{makefile,nmakefile} if
# you change anything here.
#

all_tests := sequences polymorphism iterator built-in default enumeration \
iso8859-1 list recursive test-template union binary/cdr binary/xdr choice \
clone pattern facets

build_tests := sequences

ifeq ($(xsde_polymorphic),y)
build_tests += polymorphism
endif

ifeq ($(xsde_stl),y)
ifeq ($(xsde_stl_iterator),y)
build_tests += iterator
endif
endif

ifeq ($(xsde_iostream),y)
build_tests += built-in default enumeration list test-template union facets \
pattern

ifeq ($(xsde_encoding),iso8859-1)
build_tests += iso8859-1
endif

ifeq ($(xsde_parser_validation),y)
build_tests += choice recursive clone

ifeq ($(xsde_cdr),y)
build_tests += binary/cdr
endif

ifeq ($(xsde_xdr),y)
build_tests += binary/xdr
endif
endif

endif # xsde_iostream

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
