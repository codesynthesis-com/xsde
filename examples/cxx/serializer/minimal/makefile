# file      : examples/cxx/serializer/minimal/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

xsd := people.xsd
cxx := driver.cxx

obj := $(addprefix $(out_base)/,$(cxx:.cxx=.o) $(xsd:.xsd=-sskel.o))
dep := $(obj:.o=.o.d)

xsde.l             := $(out_root)/libxsde/xsde/xsde.l
xsde.l.cpp-options := $(out_root)/libxsde/xsde/xsde.l.cpp-options

driver := $(out_base)/driver
clean  := $(out_base)/.clean


# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): cpp_options := -I$(out_base) -I$(src_base)
$(obj) $(dep): $(xsde.l.cpp-options)

skelf := $(xsd:.xsd=-sskel.hxx) $(xsd:.xsd=-sskel.ixx) $(xsd:.xsd=-sskel.cxx)
skel  := $(addprefix $(out_base)/,$(skelf))

$(skel): xsde := $(out_root)/xsde/xsde
$(skel): xsde_options += --type-map $(src_base)/people.map

$(skel): $(out_root)/xsde/xsde $(src_base)/people.map

$(call include-dep,$(dep))

# Convenience alias for default target.
#
.PHONY: $(out_base)/
$(out_base)/: $(driver)


# Clean.
#
.PHONY: $(clean)

$(clean): $(driver).o.clean                                \
  $(addsuffix .cxx.clean,$(obj))                           \
  $(addsuffix .cxx.clean,$(dep))                           \
  $(addprefix $(out_base)/,$(xsd:.xsd=-sskel.cxx.xsd.clean))


# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(skel): | $(out_base)/.gitignore
$(driver): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := driver $(skelf)
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif


# How to.
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
$(call include,$(scf_root)/xsde/serializer/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)
