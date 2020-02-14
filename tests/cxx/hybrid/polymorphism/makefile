# file      : tests/cxx/hybrid/polymorphism/makefile
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

# NOTE: remember to update dist/tests/cxx/hybrid/polymorphis/{makefile,
# nmakefile} if you change anything here.
#
all_tests := any-type enumeration multischema

build_tests :=

ifeq ($(xsde_iostream),y)
build_tests += any-type enumeration multischema
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
