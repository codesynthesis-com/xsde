# file      : examples/cxx/parser/multiroot/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../build/bootstrap.make

xsd := protocol.xsd
cxx := driver.cxx

ifeq ($(xsde_reuse_style),mixin)
cxx += protocol-pimpl-mixin.cxx
else
cxx += protocol-pimpl-tiein.cxx
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

skelf := $(xsd:.xsd=-pskel.hxx) $(xsd:.xsd=-pskel.ixx) $(xsd:.xsd=-pskel.cxx)
skel  := $(addprefix $(out_base)/,$(skelf))

$(skel): xsde := $(out_root)/xsde/xsde
$(skel): xsde_options += --type-map $(src_base)/protocol.map
$(skel): $(out_root)/xsde/xsde $(src_base)/protocol.map

$(call include-dep,$(dep))

# Convenience alias for default target.
#
$(out_base)/: $(driver)


# Dist.
#
dist-common := $(out_base)/.dist-common
$(dist) $(dist-win) $(dist-common): path := $(subst $(src_root)/,,$(src_base))

$(dist-common):
	$(call install-data,$(src_base)/driver.cxx,$(dist_prefix)/$(path)/driver.cxx)
	$(call install-data,$(src_base)/protocol.hxx,$(dist_prefix)/$(path)/protocol.hxx)
	$(call install-data,$(src_base)/protocol-pimpl-mixin.hxx,$(dist_prefix)/$(path)/protocol-pimpl-mixin.hxx)
	$(call install-data,$(src_base)/protocol-pimpl-mixin.cxx,$(dist_prefix)/$(path)/protocol-pimpl-mixin.cxx)
	$(call install-data,$(src_base)/protocol-pimpl-tiein.hxx,$(dist_prefix)/$(path)/protocol-pimpl-tiein.hxx)
	$(call install-data,$(src_base)/protocol-pimpl-tiein.cxx,$(dist_prefix)/$(path)/protocol-pimpl-tiein.cxx)
	$(call install-data,$(src_base)/protocol.map,$(dist_prefix)/$(path)/protocol.map)
	$(call install-data,$(src_base)/protocol.xsd,$(dist_prefix)/$(path)/protocol.xsd)
	$(call install-data,$(src_base)/balance.xml,$(dist_prefix)/$(path)/balance.xml)
	$(call install-data,$(src_base)/deposit.xml,$(dist_prefix)/$(path)/deposit.xml)
	$(call install-data,$(src_base)/withdraw.xml,$(dist_prefix)/$(path)/withdraw.xml)

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
$(call include,$(bld_root)/install.make)
$(call include,$(scf_root)/xsde/parser/xsd-cxx.make)


# Dependencies.
#
$(call import,$(src_root)/xsde/makefile)
$(call import,$(src_root)/libxsde/xsde/makefile)
