# file      : documentation/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2010 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

default  := $(out_base)/
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
install  := $(out_base)/.install
cleandoc := $(out_base)/.cleandoc

$(default): $(out_base)/cxx/

# Dist.
#
dist-common := $(out_base)/.dist-common

$(dist-common):
	$(call install-data,$(src_base)/default.css,$(dist_prefix)/documentation/default.css)
	$(call install-data,$(src_base)/xsde.xhtml,$(dist_prefix)/documentation/xsde.xhtml)
	$(call install-data,$(src_base)/xsde.1,$(dist_prefix)/documentation/xsde.1)

$(dist): $(dist-common) $(out_base)/cxx/.dist
$(dist-win): $(dist-common) $(out_base)/cxx/.dist


# Install.
#
$(install):
	$(call install-dir,$(src_base)/cxx,$(install_doc_dir)/xsde/cxx)
	$(call install-data,$(src_base)/default.css,$(install_doc_dir)/xsde/default.css)
	$(call install-data,$(src_base)/xsde.xhtml,$(install_doc_dir)/xsde/xsde.xhtml)
	$(call install-data,$(src_base)/xsde.1,$(install_man_dir)/man1/xsde.1)


# Clean.
#
$(cleandoc): $(src_base)/cxx/.cleandoc


$(call include,$(bld_root)/install.make)

$(call import,$(src_base)/cxx/makefile)
