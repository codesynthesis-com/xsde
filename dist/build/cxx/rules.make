# file      : build/cxx/rules.make
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(root)/build/config.make

# Rules.
#
%.o: %.cxx
	$(CXX) $(CPPFLAGS) $(EXTRA_CPPFLAGS) $(CXXFLAGS) -c $< -o $@

%: %.o
	$(LD) $(LDFLAGS) -o $@ $^ $(LIBS)
ifneq ($(POSTLD),)
	$(POSTLD) $@ $^ $(LIBS)
endif
