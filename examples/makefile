# file      : examples/makefile
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(dir $(lastword $(MAKEFILE_LIST)))../build/bootstrap.make

examples := cxx

default  := $(out_base)/
test     := $(out_base)/.test
dist     := $(out_base)/.dist
dist-win := $(out_base)/.dist-win
clean    := $(out_base)/.clean

$(default): $(addprefix $(out_base)/,$(addsuffix /,$(examples)))
$(test): $(addprefix $(out_base)/,$(addsuffix /.test,$(examples)))
$(dist): $(addprefix $(out_base)/,$(addsuffix /.dist,$(examples)))
$(dist-win): $(addprefix $(out_base)/,$(addsuffix /.dist-win,$(examples)))
$(clean): $(addprefix $(out_base)/,$(addsuffix /.clean,$(examples)))

$(foreach e,$(examples),$(call import,$(src_base)/$e/makefile))
