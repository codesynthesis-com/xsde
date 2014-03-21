# file      : build/xsde/serializer/xsd-cxx.make
# copyright : Copyright (c) 2005-2014 Code Synthesis Tools CC
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

#@@ Need to use extensions from cxx config.
#

$(call include,$(scf_root)/configuration.make)

ifeq ($(xsd_sskel_suffix),)
xsd_sskel_suffix := -sskel
endif

xsd_serializer_pattern :=            \
$(out_base)/%$(xsd_sskel_suffix).cxx \
$(out_base)/%$(xsd_sskel_suffix).hxx \
$(out_base)/%$(xsd_sskel_suffix).ixx

ifneq ($(xsd_simpl_suffix),)
xsd_serializer_pattern +=            \
$(out_base)/%$(xsd_simpl_suffix).cxx \
$(out_base)/%$(xsd_simpl_suffix).hxx
endif

xsd_serializer_pattern += $(out_base)/%-sdriver.cxx

$(xsd_serializer_pattern): xsde := xsde
$(xsd_serializer_pattern): xsde_command := cxx-serializer

ops := --generate-inline --skel-file-suffix $(xsd_sskel_suffix)

ifneq ($(xsd_pimpl_suffix),)
ops += --impl-file-suffix $(xsd_simpl_suffix)
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

ifeq ($(xsde_serializer_validation),n)
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

$(xsd_serializer_pattern): xsde_options := $(ops)


.PRECIOUS: $(xsd_serializer_pattern)

ifeq ($(out_base),$(src_base))

$(xsd_serializer_pattern): $(src_base)/%.xsd
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

else

$(xsd_serializer_pattern): $(src_base)/%.xsd | $$(dir $$@).
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

$(xsd_serializer_pattern): $(out_base)/%.xsd | $$(dir $$@).
	$(call message,xsde $<,$(xsde) $(xsde_command) $(xsde_options) --output-dir $(dir $@) $<)

endif


.PHONY: $(out_base)/%$(xsd_sskel_suffix).cxx.xsd.clean
$(out_base)/%$(xsd_sskel_suffix).cxx.xsd.clean:
	$(call message,rm $(@:.cxx.xsd.clean=.cxx),rm -f $(@:.cxx.xsd.clean=.cxx))
	$(call message,rm $(@:.cxx.xsd.clean=.hxx),rm -f $(@:.cxx.xsd.clean=.hxx))
	$(call message,rm $(@:.cxx.xsd.clean=.ixx),rm -f $(@:.cxx.xsd.clean=.ixx))

ifneq ($(xsd_simpl_suffix),)
.PHONY: $(out_base)/%$(xsd_simpl_suffix).cxx.xsd.clean
$(out_base)/%$(xsd_simpl_suffix).cxx.xsd.clean:
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=.cxx))
	$(call message,rm $$1,rm -f $$1,$(@:.cxx.xsd.clean=.hxx))
endif

.PHONY: $(out_base)/%-sdriver.cxx.xsd.clean
$(out_base)/%-sdriver.cxx.xsd.clean:
	$(call message,rm $$1,rm -f $$1,$(out_base)/$*-sdriver.cxx)

# Reset the config variables so they won't take effect in other places.
#
xsd_sskel_suffix :=
xsd_simpl_suffix :=
