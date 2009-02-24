# file      : build/import/libboost/date-time/rules.make
# author    : Boris Kolpackov <boris@kolpackov.net>
# copyright : Copyright (c) 2005-2009 Boris Kolpackov
# license   : GNU GPL v2; see accompanying LICENSE file

$(dcf_root)/import/libboost/%: root := $(libboost_root)

$(dcf_root)/import/libboost/date-time/date-time.l: \
  | $(dcf_root)/import/libboost/date-time/.

ifeq ($(libboost_type),archive)

$(dcf_root)/import/libboost/date-time/date-time.l: \
  $(libboost_root)/stage/lib/libboost_date_time-gcc$(libboost_toolset)-$(libboost_version).a
	@echo $< >$@

else

$(dcf_root)/import/libboost/date-time/date-time.l: \
  $(libboost_root)/stage/lib/libboost_date_time-gcc$(libboost_toolset)-$(libboost_version).so
	@echo $< >$@
	@echo rpath:$(root)/stage/lib >>$@

endif


$(dcf_root)/import/libboost/date-time/date-time.l.cpp-options: \
  | $(dcf_root)/import/libboost/date-time/.
	@echo include: -I$(root) >$@

ifndef %foreign%

disfigure::
	$(call message,rm $(dcf_root)/import/libboost/date-time/date-time.l,\
rm -f $(dcf_root)/import/libboost/date-time/date-time.l)
	$(call message,,\
rm -f $(dcf_root)/import/libboost/date-time/date-time.l.cpp-options)

endif
