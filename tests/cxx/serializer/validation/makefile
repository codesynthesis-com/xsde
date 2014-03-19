# file      : tests/cxx/serializer/validation/makefile
# copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

# NOTE: remember to update dist/tests/cxx/serializer/validation/{makefile,
# nmakefile} if you change anything here.
#

all_tests := built-in any attribute choice element sequence

build_tests := built-in

ifeq ($(xsde_iostream),y)
build_tests += any attribute choice element sequence
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
