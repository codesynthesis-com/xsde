# file      : documentation/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

install   := $(out_base)/.install

.PHONY: $(install)

$(install):
	$(call install-dir,$(src_base)/cxx,$(install_doc_dir)/xsde/cxx)
	$(call install-data,$(src_base)/default.css,$(install_doc_dir)/xsde/default.css)
	$(call install-data,$(src_base)/xsde.xhtml,$(install_doc_dir)/xsde/xsde.xhtml)
	$(call install-data,$(src_base)/xsde.1,$(install_man_dir)/man1/xsde.1)

$(call include,$(bld_root)/install.make)
