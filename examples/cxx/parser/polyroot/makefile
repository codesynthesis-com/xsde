# file      : examples/cxx/parser/polyroot/makefile
# copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

xsd := supermen.xsd
cxx := driver.cxx

ifeq ($(xsde_reuse_style),mixin)
cxx += supermen-pimpl-mixin.cxx
else
cxx += supermen-pimpl-tiein.cxx
endif

obj := $(addprefix $(out_base)/,$(cxx:.cxx=.o) $(xsd:.xsd=-pskel.o))
dep := $(obj:.o=.o.d)

xsde.l             := $(out_root)/libxsde/xsde/xsde.l
xsde.l.cpp-options := $(out_root)/libxsde/xsde/xsde.l.cpp-options

driver   := $(out_base)/driver
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean


# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): cpp_options := -I$(out_base) -I$(src_base)
$(obj) $(dep): $(xsde.l.cpp-options)

genf := $(xsd:.xsd=-pskel.hxx) $(xsd:.xsd=-pskel.ixx) $(xsd:.xsd=-pskel.cxx)
gen  := $(addprefix $(out_base)/,$(genf))

$(gen): xsde := $(out_root)/xsde/xsde
$(gen): xsde_options += --generate-polymorphic
$(gen): $(out_root)/xsde/xsde

$(call include-dep,$(dep),$(obj),$(gen))

# Convenience alias for default target.
#
$(out_base)/: $(driver)


# Dist.
#
dist-common := $(out_base)/.dist-common
$(dist) $(dist-win) $(dist-common): path := $(subst $(src_root)/,,$(src_base))

$(dist-common):
	$(call install-data,$(src_base)/driver.cxx,$(dist_prefix)/$(path)/driver.cxx)
	$(call install-data,$(src_base)/supermen-pimpl-mixin.hxx,$(dist_prefix)/$(path)/supermen-pimpl-mixin.hxx)
	$(call install-data,$(src_base)/supermen-pimpl-mixin.cxx,$(dist_prefix)/$(path)/supermen-pimpl-mixin.cxx)
	$(call install-data,$(src_base)/supermen-pimpl-tiein.hxx,$(dist_prefix)/$(path)/supermen-pimpl-tiein.hxx)
	$(call install-data,$(src_base)/supermen-pimpl-tiein.cxx,$(dist_prefix)/$(path)/supermen-pimpl-tiein.cxx)
	$(call install-data,$(src_base)/supermen.xsd,$(dist_prefix)/$(path)/supermen.xsd)
	$(call install-data,$(src_base)/person.xml,$(dist_prefix)/$(path)/person.xml)
	$(call install-data,$(src_base)/superman.xml,$(dist_prefix)/$(path)/superman.xml)
	$(call install-data,$(src_base)/batman.xml,$(dist_prefix)/$(path)/batman.xml)

$(dist): $(dist-common)
	$(call install-data,$(src_base)/README,$(dist_prefix)/$(path)/README)

$(dist-win): $(dist-common)
	$(call install-data,$(src_base)/README,$(dist_prefix)/$(path)/README.txt)
	$(call message,,unix2dos $(dist_prefix)/$(path)/README.txt)


# Clean.
#
$(clean): $(driver).o.clean                                \
  $(addsuffix .cxx.clean,$(obj))                           \
  $(addsuffix .cxx.clean,$(dep))                           \
  $(addprefix $(out_base)/,$(xsd:.xsd=-pskel.cxx.xsd.clean))


# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(gen): | $(out_base)/.gitignore
$(driver): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := driver $(genf)
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif


# How to.
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)
$(call include,$(bld_root)/install.make)
$(call include,$(scf_root)/xsde/parser/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)
