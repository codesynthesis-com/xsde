# file      : build/c/rules.make
# license   : GNU GPL v2 + exceptions; see accompanying LICENSE file

include $(root)/build/config.make

# Rules.
#
%.o: %.c
	$(CC) $(CPPFLAGS) $(EXTRA_CPPFLAGS) $(CFLAGS) -c $< -o $@
