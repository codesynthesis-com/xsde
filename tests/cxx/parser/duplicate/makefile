# file      : tests/cxx/parser/duplicate/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

xsd := test.xsd

obj := $(addprefix $(out_base)/,$(xsd:.xsd=-pskel.o) $(xsd:.xsd=-pimpl.o) $(xsd:.xsd=-pdriver.o))
dep := $(obj:.o=.o.d)

xsde.l             := $(out_root)/libxsde/xsde/xsde.l
xsde.l.cpp-options := $(out_root)/libxsde/xsde/xsde.l.cpp-options

driver := $(out_base)/$(xsd:.xsd=-pdriver)
test   := $(out_base)/.test
clean  := $(out_base)/.clean


# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): $(xsde.l.cpp-options)

genf := $(xsd:.xsd=-pskel.hxx) $(xsd:.xsd=-pskel.ixx) $(xsd:.xsd=-pskel.cxx) \
        $(xsd:.xsd=-pimpl.hxx) $(xsd:.xsd=-pimpl.cxx) $(xsd:.xsd=-pdriver.cxx)

gen := $(addprefix $(out_base)/,$(genf))

$(gen): xsde := $(out_root)/xsde/xsde
$(gen): xsde_options += --generate-print-impl --generate-test-driver \
--force-overwrite
$(gen): $(out_root)/xsde/xsde

$(call include-dep,$(dep))

# Convenience alias for default target.
#
.PHONY: $(out_base)/
$(out_base)/: $(driver)


# Test.
#
.PHONY: $(test)

$(test): driver := $(driver)
$(test): $(driver) $(src_base)/test.xml $(src_base)/output
	$(call message,test $$1,$$1 $(src_base)/test.xml | diff -u $(src_base)/output -,$(driver))

# Clean.
#
.PHONY: $(clean)

$(clean): $(driver).o.clean                                  \
  $(addsuffix .cxx.clean,$(obj))                             \
  $(addsuffix .cxx.clean,$(dep))                             \
  $(addprefix $(out_base)/,$(xsd:.xsd=-pskel.cxx.xsd.clean)) \
  $(addprefix $(out_base)/,$(xsd:.xsd=-pimpl.cxx.xsd.clean))


# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(gen): | $(out_base)/.gitignore
$(driver): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := $(xsd:.xsd=-pdriver) $(genf)
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif


# How to.
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)

xsd_pimpl_suffix := -pimpl
$(call include,$(scf_root)/xsde/parser/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)
