# file      : build/bootstrap.make
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2005-2011 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

project_name := xsd/e

# First try to include the bundled bootstrap.make if it exist. If that
# fails, let make search for the external bootstrap.make.
#
build := build-0.3

-include $(dir $(lastword $(MAKEFILE_LIST)))../../$(build)/bootstrap.make

ifeq ($(patsubst %build/bootstrap.make,,$(lastword $(MAKEFILE_LIST))),)
include $(build)/bootstrap.make
endif

# Configuration
#
$(call include,$(scf_root)/configuration.make)


# Aliases
#
.PHONY: $(out_base)/               \
        $(out_base)/.test          \
        $(out_base)/.install       \
        $(out_base)/.dist          \
        $(out_base)/.dist-win      \
        $(out_base)/.dist-common   \
        $(out_base)/.clean

ifdef %interactive%

.PHONY: test install dist dist-win clean

test: $(out_base)/.test
install: $(out_base)/.install
dist: $(out_base)/.dist
dist-win: $(out_base)/.dist-win
clean: $(out_base)/.clean

ifneq ($(filter $(.DEFAULT_GOAL),test install dist dist-win clean),)
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

ifneq ($(filter $(MAKECMDGOALS),clean disfigure),)
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
