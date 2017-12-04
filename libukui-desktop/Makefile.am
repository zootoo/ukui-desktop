libukui_desktopdir = $(includedir)/ukui-desktop-2.0/libukui-desktop
libukui_desktop_HEADERS = \
	ukui-desktop.h \
	ukui-desktop-utils.h \
	ukui-desktop-item.h \
	ukui-dconf.h \
	ukui-gsettings.h \
	ukui-bg.h			\
	ukui-bg-crossfade.h		\
	ukui-desktop-thumbnail.h	\
	ukui-rr.h			\
	ukui-rr-config.h		\
	ukui-rr-labeler.h \
	ukui-colorbutton.h \
	ukui-colorsel.h \
	ukui-colorseldialog.h

lib_LTLIBRARIES = libukui-desktop-2.la

AM_CPPFLAGS =							\
	$(UKUI_DESKTOP_CFLAGS)					\
	$(XLIB_CFLAGS)						\
	$(DCONF_CFLAGS)						\
	-DG_LOG_DOMAIN=\"UkuiDesktop\"				\
	-DUKUILOCALEDIR=\""$(prefix)/$(DATADIRNAME)/locale\""	\
	-DPNP_IDS=\""$(PNP_IDS)"\"				\
	$(DISABLE_DEPRECATED_CFLAGS)

AM_CFLAGS = $(WARN_CFLAGS)

noinst_PROGRAMS = test-ditem test

CLEANFILES =

introspection_sources = \
	ukui-desktop-utils.c	\
	ukui-desktop-thumbnail.c \
	ukui-thumbnail-pixbuf-utils.c \
	ukui-dconf.c \
	ukui-gsettings.c \
	ukui-bg.c		\
	ukui-bg-crossfade.c	\
	display-name.c		\
	ukui-rr.c		\
	ukui-rr-config.c	\
	ukui-rr-output-info.c	\
	ukui-rr-labeler.c	\
	ukui-colorbutton.c \
	ukui-colorsel.c \
	ukui-colorseldialog.c \
	edid-parse.c

libukui_desktop_2_la_SOURCES =		\
	$(introspection_sources)	\
	ukui-desktop-item.c		\
	ukui-rr-private.h		\
	edid.h				\
	private.h

libukui_desktop_2_la_LIBADD =	\
	$(XLIB_LIBS)		\
	$(UKUI_DESKTOP_LIBS)	\
	$(DCONF_LIBS)

libukui_desktop_2_la_LDFLAGS = \
	-version-info $(LT_VERSION) \
	-export-symbols-regex "^ukui_.*" \
	-no-undefined

test_SOURCES = \
	test.c

test_LDADD = \
	libukui-desktop-2.la		\
	$(UKUI_DESKTOP_LIBS)

test_ditem_SOURCES = \
	test-ditem.c

test_ditem_LDADD = \
	libukui-desktop-2.la		\
	$(XLIB_LIBS)			\
	$(UKUI_DESKTOP_LIBS)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ukui-desktop-2.0.pc

if USE_INTERNAL_PNP_IDS
pnpdatadir = $(datadir)/libukui-desktop
pnpdata_DATA = pnp.ids
endif
pnpdata_DATA_dist = pnp.ids

check:
	test -s $(top_srcdir)/libukui-desktop/pnp.ids

-include $(INTROSPECTION_MAKEFILE)
INTROSPECTION_GIRS =
INTROSPECTION_SCANNER_ARGS = --add-include-path=$(srcdir) --warn-all
INTROSPECTION_COMPILER_ARGS = --includedir=$(srcdir)

if HAVE_INTROSPECTION

UkuiDesktop-2.0.gir: libukui-desktop-2.la
UkuiDesktop_2_0_gir_INCLUDES = GObject-2.0 Gtk-3.0
UkuiDesktop_2_0_gir_PACKAGES = gdk-pixbuf-2.0 glib-2.0 gobject-2.0 gio-2.0 gtk+-3.0
UkuiDesktop_2_0_gir_CFLAGS = -DUKUI_DESKTOP_USE_UNSTABLE_API -I$(top_srcdir)
UkuiDesktop_2_0_gir_LIBS = libukui-desktop-2.la
UkuiDesktop_2_0_gir_FILES = $(introspection_sources) $(libukui_desktop_HEADERS)
UkuiDesktop_2_0_gir_SCANNERFLAGS = --identifier-prefix=Ukui --symbol-prefix=ukui_
INTROSPECTION_SCANNER_ARGS += $(patsubst %,--c-include='libukui-desktop/%',$(libukui_desktop_HEADERS))
INTROSPECTION_GIRS += UkuiDesktop-2.0.gir

girdir = $(datadir)/gir-1.0
gir_DATA = $(INTROSPECTION_GIRS)

typelibdir = $(libdir)/girepository-1.0
typelib_DATA = $(INTROSPECTION_GIRS:.gir=.typelib)

CLEANFILES += $(gir_DATA) $(typelib_DATA)
endif

EXTRA_DIST = \
	ukui-desktop-2.0.pc.in                \
	ukui-desktop-2.0-uninstalled.pc.in    \
	$(pnpdata_DATA_dist)

MAINTAINERCLEANFILES = \
	pnp.ids

-include $(top_srcdir)/git.mk
