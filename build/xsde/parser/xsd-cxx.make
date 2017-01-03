# file      : build/xsde/parser/xsd-cxx.make
# copyright : Copyright (c) 2005-2017 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#@@ Need to use extensions from cxx config.
#

$(call include,$(scf_root)/configuration.make)

ifeq ($(xsd_pskel_suffix),)
xsd_pskel_suffix := -pskel
endif

xsd_parser_pattern :=                \
$(out_base)/%$(xsd_pskel_suffix).cxx \
$(out_base)/%$(xsd_pskel_suffix).hxx \
$(out_base)/%$(xsd_pskel_suffix).ixx

ifneq ($(xsd_pimpl_suffix),)
xsd_parser_pattern +=                \
$(out_base)/%$(xsd_pimpl_suffix).cxx \
$(out_base)/%$(xsd_pimpl_suffix).hxx
endif

xsd_parser_pattern += $(out_base)/%-pdriver.cxx

$(xsd_parser_pattern): xsde := xsde
$(xsd_parser_pattern): xsde_command := cxx-parser

ops := --generate-inline --skel-file-suffix $(xsd_pskel_suffix)

ifneq ($(xsd_pimpl_suffix),)
ops += --impl-file-suffix $(xsd_pimpl_suffix)
endif

ops += --char-encoding $(xsde_encoding)

ifeq ($(xsde_stl),n)
ops += --no-stl
endif

ifeq ($(xsde_iostream),n)
ops += --no-iostream
endif

ifeq ($(xsde_exceptions),n)
ops += --no-exceptions
endif

ifeq ($(xsde_longlong),n)
ops += --no-long-long
endif

ifeq ($(xsde_parser_validation),n)
ops += --suppress-validation
endif

ifeq ($(xsde_polymorphic),y)
ops += --runtime-polymorphic
endif

ifeq ($(xsde_reuse_style),mixin)
ops += --reuse-style-mixin
endif

ifeq ($(xsde_reuse_style),none)
ops += $(xsde_options) --reuse-style-none
endif

ifeq ($(xsde_custom_allocator),y)
ops += $(xsde_options) --custom-allocator
endif

$(xsd_parser_pattern): xsde_options := $(ops)


.PRECIOUS: $(xsd_parser_pattern)

ifeq ($(out_base),$(src_base))

$(xsd_parser_pattern): $(src_base)/%.xsd
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

else

$(xsd_parser_pattern): $(src_base)/%.xsd | $$(dir $$@).
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

$(xsd_parser_pattern): $(out_base)/%.xsd | $$(dir $$@).
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

endif


.PHONY: $(out_base)/%$(xsd_pskel_suffix).cxx.xsd.clean
$(out_base)/%$(xsd_pskel_suffix).cxx.xsd.clean:
	$(call message,rm $(@:.cxx.xsd.clean=.cxx),rm -f $(@:.cxx.xsd.clean=.cxx))
	$(call message,rm $(@:.cxx.xsd.clean=.hxx),rm -f $(@:.cxx.xsd.clean=.hxx))
	$(call message,rm $(@:.cxx.xsd.clean=.ixx),rm -f $(@:.cxx.xsd.clean=.ixx))

ifneq ($(xsd_pimpl_suffix),)
.PHONY: $(out_base)/%$(xsd_pimpl_suffix).cxx.xsd.clean
$(out_base)/%$(xsd_pimpl_suffix).cxx.xsd.clean:
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=.cxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=.hxx))
endif

.PHONY: $(out_base)/%-pdriver.cxx.xsd.clean
$(out_base)/%-pdriver.cxx.xsd.clean:
	$(call message,rm $$1,rm -f $$1,$(out_base)/$*-pdriver.cxx)

# Reset the config variables so they won't take effect in other places.
#
xsd_pskel_suffix :=
xsd_pimpl_suffix :=
