# file      : examples/cxx/makefile
# author    : Boris Kolpackov <boris@codesynthesis.com>
# copyright : Copyright (c) 2006-2010 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../../build/bootstrap.make

mappings := hybrid parser serializer

default  := $(out_base)/
test     := $(out_base)/.test
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(mappings)))
$(test): $(addprefix $(out_base)/,$(addsuffix /.test,$(mappings)))
$(dist): $(addprefix $(out_base)/,$(addsuffix /.dist,$(mappings)))
$(dist-win): $(addprefix $(out_base)/,$(addsuffix /.dist-win,$(mappings)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(mappings)))

$(foreach m,$(mappings),$(call import,$(src_base)/$m/makefile))
