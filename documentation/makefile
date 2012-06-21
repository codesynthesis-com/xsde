# file      : documentation/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

default  := $(out_base)/
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
install  := $(out_base)/.install
cleandoc := $(out_base)/.cleandoc

# Import.
#
$(call import,\
  $(scf_root)/import/cli/stub.make,\
  cli: cli,cli-rules: cli_rules)

# Build.
#
$(default):            \
$(out_base)/cxx/       \
$(out_base)/xsde.xhtml \
$(out_base)/xsde.1

# Man/html pages.
#
$(out_base)/xsde.xhtml $(out_base)/xsde.1: cli := $(cli)
$(out_base)/xsde.xhtml $(out_base)/xsde.1: cli_options += -I $(src_root)/xsde

$(out_base)/xsde.xhtml $(out_base)/xsde.1:  \
$(src_root)/xsde/options.cli                \
$(src_root)/xsde/cxx/options.cli            \
$(src_root)/xsde/cxx/hybrid/options.cli     \
$(src_root)/xsde/cxx/parser/options.cli     \
$(src_root)/xsde/cxx/serializer/options.cli

# Assemble the options from different files in a specific order.
#

# XHTML
#
$(out_base)/xsde.xhtml: $(src_base)/xsde-prologue.xhtml          \
                        $(src_base)/xsde-epilogue.xhtml          \
                        $(src_base)/xsde-hybrid-header.xhtml     \
                        $(src_base)/xsde-parser-header.xhtml     \
			$(src_base)/xsde-serializer-header.xhtml \
                        | $(out_base)/.
# Common options.
#
	$(call message,cli-html $$1,$(cli) $(cli_options) --generate-html \
--stdout --suppress-undocumented --exclude-base --class CXX::options \
--class options --html-prologue-file $(src_base)/xsde-prologue.xhtml \
$$1 >$@, $(src_root)/xsde/cxx/options.cli)

# C++/Hybrid options.
#
	$(call message,cli-html $$1,$(cli) $(cli_options) --generate-html \
--stdout --suppress-undocumented --exclude-base \
--html-prologue-file $(src_base)/xsde-hybrid-header.xhtml \
$$1 >>$@, $(src_root)/xsde/cxx/hybrid/options.cli)

# C++/Parser options.
#
	$(call message,cli-html $$1,$(cli) $(cli_options) --generate-html \
--stdout --suppress-undocumented --exclude-base \
--html-prologue-file $(src_base)/xsde-parser-header.xhtml \
$$1 >>$@, $(src_root)/xsde/cxx/parser/options.cli)

# C++/Serializer options.
#
	$(call message,cli-html $$1,$(cli) $(cli_options) --generate-html \
--stdout --suppress-undocumented --exclude-base \
--html-prologue-file $(src_base)/xsde-serializer-header.xhtml \
--html-epilogue-file $(src_base)/xsde-epilogue.xhtml \
$$1 >>$@, $(src_root)/xsde/cxx/serializer/options.cli)


# MAN
#
$(out_base)/xsde.1: $(src_base)/xsde-prologue.1              \
                        $(src_base)/xsde-epilogue.1          \
                        $(src_base)/xsde-hybrid-header.1     \
                        $(src_base)/xsde-parser-header.1     \
			$(src_base)/xsde-serializer-header.1 \
                        | $(out_base)/.
# Common options.
#
	$(call message,cli-man $$1,$(cli) $(cli_options) --generate-man \
--stdout --suppress-undocumented --exclude-base --class CXX::options \
--class options --man-prologue-file $(src_base)/xsde-prologue.1 \
$$1 >$@, $(src_root)/xsde/cxx/options.cli)

# C++/Hybrid options.
#
	$(call message,cli-man $$1,$(cli) $(cli_options) --generate-man \
--stdout --suppress-undocumented --exclude-base \
--man-prologue-file $(src_base)/xsde-hybrid-header.1 \
$$1 >>$@, $(src_root)/xsde/cxx/hybrid/options.cli)

# C++/Parser options.
#
	$(call message,cli-man $$1,$(cli) $(cli_options) --generate-man \
--stdout --suppress-undocumented --exclude-base \
--man-prologue-file $(src_base)/xsde-parser-header.1 \
$$1 >>$@, $(src_root)/xsde/cxx/parser/options.cli)

# C++/Serializer options.
#
	$(call message,cli-man $$1,$(cli) $(cli_options) --generate-man \
--stdout --suppress-undocumented --exclude-base \
--man-prologue-file $(src_base)/xsde-serializer-header.1 \
--man-epilogue-file $(src_base)/xsde-epilogue.1 \
$$1 >>$@, $(src_root)/xsde/cxx/serializer/options.cli)


# Dist.
#
dist-common := $(out_base)/.dist-common

$(dist-common): $(out_base)/xsde.xhtml \
                $(out_base)/xsde.1
	$(call install-data,$(src_base)/default.css,$(dist_prefix)/documentation/default.css)
	$(call install-data,$(out_base)/xsde.xhtml,$(dist_prefix)/documentation/xsde.xhtml)
	$(call install-data,$(out_base)/xsde.1,$(dist_prefix)/documentation/xsde.1)

$(dist): $(dist-common) $(out_base)/cxx/.dist
$(dist-win): $(dist-common) $(out_base)/cxx/.dist

# Install.
#
$(install): $(out_base)/xsde.xhtml \
            $(out_base)/xsde.1	
	$(call install-dir,$(src_base)/cxx,$(install_doc_dir)/xsde/cxx)
	$(call install-data,$(src_base)/default.css,$(install_doc_dir)/xsde/default.css)
	$(call install-data,$(out_base)/xsde.xhtml,$(install_doc_dir)/xsde/xsde.xhtml)
	$(call install-data,$(out_base)/xsde.1,$(install_man_dir)/man1/xsde.1)

# Clean.
#
$(cleandoc): $(src_base)/cxx/.cleandoc
	$(call message,rm $$1,rm -f $$1,$(out_base)/xsde.1)
	$(call message,rm $$1,rm -f $$1,$(out_base)/xsde.xhtml)

# Generated .gitignore.
#
ifeq ($(out_base),$(src_base))
$(out_base)/xsde.xhtml $(out_base)/xsde.1: | $(out_base)/.gitignore

$(out_base)/.gitignore: files := xsde.1 xsde.xhtml
$(clean): $(out_base)/.gitignore.clean

$(call include,$(bld_root)/git/gitignore.make)
endif

$(call include,$(bld_root)/install.make)
$(call import,$(src_base)/cxx/makefile)
