# file      : tests/cxx/parser/validation/built-in/int/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2011 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../../../../../build/bootstrap.make

cxx := driver.cxx

obj := $(addprefix $(out_base)/,$(cxx:.cxx=.o))
dep := $(obj:.o=.o.d)

xsde.l             := $(out_root)/libxsde/xsde/xsde.l
xsde.l.cpp-options := $(out_root)/libxsde/xsde/xsde.l.cpp-options

driver   := $(out_base)/driver
test     := $(out_base)/.test
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean


# Build.
#
$(driver): $(obj) $(xsde.l)

$(obj) $(dep): $(xsde.l.cpp-options)

$(call include-dep,$(dep))

# Convenience alias for default target.
#
$(out_base)/: $(driver)


# Test.
#
$(test): driver := $(driver)
$(test): $(driver)
	$(call message,test $$1,$$1,$(driver))


# Dist.
#
$(dist) $(dist-win): opt := -src $(src_base) -cxx "$(cxx)" -out $(dist_prefix)

$(dist):
	$(call message,install $(src_base),$(scf_root)/dist $(opt))

$(dist-win):
	$(call message,install $(src_base),$(scf_root)/dist -win $(opt))


# Clean.
#
$(clean): $(driver).o.clean                                \
  $(addsuffix .cxx.clean,$(obj))                           \
  $(addsuffix .cxx.clean,$(dep))


# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(driver): | $(out_base)/.gitignore

$(out_base)/.gitignore: files := driver
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif


# How to.
#
$(call include,$(bld_root)/cxx/o-e.make)
$(call include,$(bld_root)/cxx/cxx-o.make)
$(call include,$(bld_root)/cxx/cxx-d.make)


# Dependencies.
#
$(call import,$(src_root)/libxsde/xsde/makefile)
