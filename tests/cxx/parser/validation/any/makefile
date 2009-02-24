# file      : tests/cxx/parser/validation/any/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../../build/bootstrap.make

xsd := test.xsd
cxx := driver.cxx

tests := 000

obj := $(addprefix $(out_base)/,$(cxx:.cxx=.o) $(xsd:.xsd=-pskel.o))
dep := $(obj:.o=.o.d)

xsde.l             := $(out_root)/libxsde/xsde/xsde.l
xsde.l.cpp-options := $(out_root)/libxsde/xsde/xsde.l.cpp-options

driver := $(out_base)/driver
test   := $(out_base)/.test
clean  := $(out_base)/.clean


# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): $(xsde.l.cpp-options)

skel := $(out_base)/$(xsd:.xsd=-pskel.hxx) \
        $(out_base)/$(xsd:.xsd=-pskel.ixx) \
        $(out_base)/$(xsd:.xsd=-pskel.cxx)

$(skel): xsde := $(out_root)/xsde/xsde
$(skel): $(out_root)/xsde/xsde

$(call include-dep,$(dep))


# Convenience alias for default target.
#
.PHONY: $(out_base)/
$(out_base)/: $(driver)


# Test.
#
test_targets := $(addprefix $(out_base)/.test-,$(tests))

.PHONY: $(test)
$(test): $(test_targets)


$(test_targets): driver := $(driver)

.PHONY: $(out_base)/.test-%
$(out_base)/.test-%: $(driver) $(src_base)/test.xsd $(src_base)/test-%.xml $(src_base)/test-%.std
	$(call message,test $(out_base)/$*,$(driver) $(src_base)/test-$*.xml | diff -u $(src_base)/test-$*.std -)


# Clean.
#
.PHONY: $(clean)

$(clean): $(driver).o.clean                                \
  $(addsuffix .cxx.clean,$(obj))                           \
  $(addsuffix .cxx.clean,$(dep))                           \
  $(addprefix $(out_base)/,$(xsd:.xsd=-pskel.cxx.xsd.clean))


# How to.
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
$(call include,$(scf_root)/xsde/parser/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)