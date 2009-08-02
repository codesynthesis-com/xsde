# file      : makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))build/bootstrap.make

default  := $(out_base)/
test     := $(out_base)/.test
install  := $(out_base)/.install
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean
cleandoc := $(out_base)/.cleandoc


.PHONY: $(default) $(test) $(install) $(dist) $(dist-win) $(clean) $(cleandoc)

# Build.
#
$(default): $(out_base)/xsde/         \
            $(out_base)/libxsde/      \
            $(out_base)/tests/        \
            $(out_base)/examples/     \
            $(src_base)/documentation/

# Test.
#
$(test): $(out_base)/tests/.test

# Install.
#
$(install): $(out_base)/xsde/.install           \
            $(out_base)/documentation/.install
	$(call install-data,$(src_base)/GPLv2,$(install_doc_dir)/xsde/GPLv2)
	$(call install-data,$(src_base)/LICENSE,$(install_doc_dir)/xsde/LICENSE)
	$(call install-data,$(src_base)/NEWS,$(install_doc_dir)/xsde/NEWS)
	$(call install-data,$(src_base)/README,$(install_doc_dir)/xsde/README)

# Dist.
#
dist-common := $(out_base)/.dist-common

.PHONY: $(dist-common)

$(dist-common):
	$(call install-dir,$(src_base)/dist/build,$(dist_prefix)/build)
	$(call install-dir,$(src_base)/dist/config,$(dist_prefix)/config)
	$(call install-dir,$(src_base)/dist/etc,$(dist_prefix)/etc)
	$(call install-dir,$(src_base)/dist/examples,$(dist_prefix)/examples)
	$(call install-dir,$(src_base)/dist/libxsde,$(dist_prefix)/libxsde)
	$(call install-data,$(src_base)/dist/makefile,$(dist_prefix)/makefile)
	$(call install-data,$(src_base)/dist/nmakefile,$(dist_prefix)/nmakefile)


$(dist): $(dist-common)                  \
         $(out_base)/xsde/.dist          \
         $(out_base)/libxsde/.dist       \
         $(out_base)/examples/.dist      \
         $(out_base)/documentation/.dist
	$(call install-dir,$(src_base)/dist/,$(dist_prefix))
	$(call install-data,$(src_base)/GPLv2,$(dist_prefix)/GPLv2)
	$(call install-data,$(src_base)/LICENSE,$(dist_prefix)/LICENSE)
	$(call install-data,$(src_base)/NEWS,$(dist_prefix)/NEWS)
	$(call install-data,$(src_base)/version,$(dist_prefix)/version)
	$(call install-data,$(src_base)/dist/INSTALL,$(dist_prefix)/INSTALL)
	$(call install-data,$(src_base)/dist/README,$(dist_prefix)/README)

$(dist-win): $(dist-common)                      \
             $(out_base)/xsde/.dist-win          \
             $(out_base)/libxsde/.dist-win       \
             $(out_base)/examples/.dist-win      \
             $(out_base)/documentation/.dist-win
	$(call install-data,$(src_base)/GPLv2,$(dist_prefix)/GPLv2.txt)
	$(call message,,unix2dos $(dist_prefix)/GPLv2.txt)
	$(call install-data,$(src_base)/LICENSE,$(dist_prefix)/LICENSE.txt)
	$(call message,,unix2dos $(dist_prefix)/LICENSE.txt)
	$(call install-data,$(src_base)/NEWS,$(dist_prefix)/NEWS.txt)
	$(call message,,unix2dos $(dist_prefix)/NEWS.txt)
	$(call install-data,$(src_base)/version,$(dist_prefix)/version.txt)
	$(call message,,unix2dos $(dist_prefix)/version.txt)
	$(call install-data,$(src_base)/dist/INSTALL,$(dist_prefix)/INSTALL.txt)
	$(call message,,unix2dos $(dist_prefix)/INSTALL.txt)
	$(call install-data,$(src_base)/dist/README,$(dist_prefix)/README.txt)
	$(call message,,unix2dos $(dist_prefix)/README.txt)

# Clean.
#
$(clean): $(out_base)/xsde/.clean      \
          $(out_base)/libxsde/.clean   \
          $(out_base)/tests/.clean     \
          $(out_base)/examples/.clean

$(cleandoc): $(src_base)/documentation/.cleandoc


$(call include,$(bld_root)/install.make)

$(call import,$(src_base)/xsde/makefile)
$(call import,$(src_base)/libxsde/makefile)
$(call import,$(src_base)/tests/makefile)
$(call import,$(src_base)/examples/makefile)
$(call import,$(src_base)/documentation/makefile)
