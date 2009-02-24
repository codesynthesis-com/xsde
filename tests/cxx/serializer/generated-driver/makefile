# file      : tests/cxx/serializer/generated-driver/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

xsd := test.xsd

obj := $(addprefix $(out_base)/,$(xsd:.xsd=-sskel.o) $(xsd:.xsd=-simpl.o) $(xsd:.xsd=-sdriver.o))
dep := $(obj:.o=.o.d)

xsde.l             := $(out_root)/libxsde/xsde/xsde.l
xsde.l.cpp-options := $(out_root)/libxsde/xsde/xsde.l.cpp-options

driver := $(out_base)/$(xsd:.xsd=-sdriver)
test   := $(out_base)/.test
clean  := $(out_base)/.clean


# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): $(xsde.l.cpp-options)

gen :=  $(out_base)/$(xsd:.xsd=-sskel.hxx)  \
        $(out_base)/$(xsd:.xsd=-sskel.ixx)  \
        $(out_base)/$(xsd:.xsd=-sskel.cxx)  \
        $(out_base)/$(xsd:.xsd=-sdriver.cxx)

$(gen): xsde := $(out_root)/xsde/xsde
$(gen): xsde_options += --generate-test-driver --force-overwrite
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
$(test): $(driver) $(src_base)/output
	$(call message,test $$1,$$1 | diff -u $(src_base)/output -,$(driver))

# Clean.
#
.PHONY: $(clean)

$(clean): $(driver).o.clean                                  \
  $(addsuffix .cxx.clean,$(obj))                             \
  $(addsuffix .cxx.clean,$(dep))                             \
  $(addprefix $(out_base)/,$(xsd:.xsd=-sskel.cxx.xsd.clean))


# How to.
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)

xsd_simpl_suffix := -simpl
$(call include,$(scf_root)/xsde/serializer/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)
