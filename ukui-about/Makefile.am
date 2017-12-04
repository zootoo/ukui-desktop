bin_PROGRAMS = ukui-about
ukui_about_SOURCES = ukui-about.c ukui-about.h
ukui_about_CFLAGS = $(UKUI_ABOUT_CFLAGS)
ukui_about_LDADD = $(UKUI_ABOUT_LIBS)

AM_CPPFLAGS = \
	-I$(top_srcdir) \
	$(UKUI_ABOUT_CFLAGS) \
	-DLOCALE_DIR=\"$(datadir)/locale\" \
	-DPIXMAPS_DIR=\"$(datadir)/pixmaps/\"

@INTLTOOL_DESKTOP_RULE@

desktopdir = $(datadir)/applications
desktop_in_files = ukui-about.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

CLEANFILES = ukui-about.desktop ukui-about

#-include $(top_srcdir)/git.mk

