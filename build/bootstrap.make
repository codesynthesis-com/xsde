# file      : build/bootstrap.make
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2009 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

project_name := XSD/e

include build-0.3/bootstrap.make


# Configuration
#
$(call include,$(scf_root)/configuration.make)


# Aliases
#
ifdef %interactive%

.PHONY: test $(out_base)/.test \
        install $(out_base)/.install \
        dist $(out_base)/.dist \
	dist-win $(out_base)/.dist-win \
        clean $(out_base)/.clean \
        cleandoc $(out_base)/.cleandoc

test: $(out_base)/.test
install: $(out_base)/.install
dist: $(out_base)/.dist
dist-win: $(out_base)/.dist-win
clean: $(out_base)/.clean
cleandoc: $(out_base)/.cleandoc

ifneq ($(filter $(.DEFAULT_GOAL),test install dist dist-win clean cleandoc),)
.DEFAULT_GOAL :=
endif

endif


# Make sure the distribution prefix is set if the goal is dist or dist-win.
#
ifneq ($(filter $(MAKECMDGOALS),dist dist-win),)
ifeq ($(dist_prefix),)
$(error dist_prefix is not set)
endif
endif


# Don't include dependency info for certain targets.
#
define include-dep
$(call -include,$1)
endef

ifneq ($(filter $(MAKECMDGOALS),clean cleandoc disfigure),)
include-dep =
endif


# For dist, don't include dependecies in libxsde, examples, and tests.
#
ifneq ($(filter $(MAKECMDGOALS),dist dist-win),)

ifneq ($(subst $(src_root)/libxsde/,,$(src_base)),$(src_base))
include-dep =
endif

ifneq ($(subst $(src_root)/tests/,,$(src_base)),$(src_base))
include-dep =
endif

ifneq ($(subst $(src_root)/examples/,,$(src_base)),$(src_base))
include-dep =
endif

endif
