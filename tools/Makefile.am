bin_PROGRAMS = ukui-color-select
bin_SCRIPTS =

AM_CPPFLAGS = \
	-I$(top_srcdir)

ukui_color_select_SOURCES = \
	ukui-color-select.c

ukui_color_select_CFLAGS = \
	-DLOCALE_DIR=\"$(datadir)/locale\" \
	$(UKUI_DESKTOP_CFLAGS)

ukui_color_select_LDADD = \
	$(top_builddir)/libukui-desktop/libukui-desktop-2.la \
	$(UKUI_DESKTOP_LIBS)

@INTLTOOL_DESKTOP_RULE@

desktopdir = $(datadir)/applications
desktop_in_files = ukui-color-select.desktop.in
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

CLEANFILES = *.desktop *.desktop.in
