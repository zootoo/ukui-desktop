m4_define([ukui_platform], [1])
m4_define([ukui_minor], [19])
m4_define([ukui_micro], [1])

m4_define(ukui_version, [ukui_platform.ukui_minor.ukui_micro]),

AC_INIT([ukui-desktop], ukui_version, [https://github.com/ukui-desktop/ukui-desktop/])
AC_CONFIG_SRCDIR([libukui-desktop])

AM_INIT_AUTOMAKE([1.9 foreign no-dist-gzip dist-xz tar-ustar check-news])
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])

AC_CONFIG_HEADERS([config.h])

AC_CONFIG_MACRO_DIR([m4])

dnl make sure we keep ACLOCAL_FLAGS around for maintainer builds to work
AC_SUBST(ACLOCAL_AMFLAGS, "\${ACLOCAL_FLAGS}")

# Before making a release, the LT_VERSION string should be modified.
# The string is of the form C:R:A.
# - If interfaces have been changed or added, but binary compatibility has
#   been preserved, change to C+1:0:A+1
# - If binary compatibility has been broken (eg removed or changed interfaces)
#   change to C+1:0:0
# - If the interface is the same as the previous version, change to C:R+1:A

LT_VERSION=18:4:1
AC_SUBST(LT_VERSION)

AM_MAINTAINER_MODE

# GLib min/max required versions
AC_DEFINE([GLIB_VERSION_MAX_ALLOWED], [GLIB_VERSION_2_36],
	[Warn on use of APIs added after GLib 2.36])
AC_DEFINE([GLIB_VERSION_MIN_REQUIRED], [GLIB_VERSION_2_36],
	[Warn on use of APIs deprecated before GLib 2.36])

IT_PROG_INTLTOOL([0.50.1])

AC_PROG_CC
AC_STDC_HEADERS
AC_PROG_LIBTOOL
PKG_PROG_PKG_CONFIG

UKUI_PLATFORM=ukui_platform
UKUI_MINOR=ukui_minor
UKUI_MICRO=ukui_micro
UKUI_DATE=`date +"%Y-%m-%d"`

UKUI_DATE=
UKUI_DATE_COMMENT_START="<!--"
UKUI_DATE_COMMENT_END="-->"

AC_SUBST(UKUI_PLATFORM)
AC_SUBST(UKUI_MINOR)
AC_SUBST(UKUI_MICRO)
AC_SUBST(UKUI_DATE)
AC_SUBST(UKUI_DATE_COMMENT_START)
AC_SUBST(UKUI_DATE_COMMENT_END)

AC_DEFINE(UKUI_MAJOR, [ukui_platform], [Define to the major version])
AC_DEFINE(UKUI_MINOR, [ukui_minor], [Define to the minor version])
AC_DEFINE(UKUI_MICRO, [ukui_micro], [Define to the micro version])

UKUI_COMMON_INIT
UKUI_DEBUG_CHECK
UKUI_COMPILE_WARNINGS([maximum])
UKUI_MAINTAINER_MODE_DEFINES

AC_ARG_ENABLE(deprecation_flags,
              [AC_HELP_STRING([--enable-deprecation-flags],
                              [use *_DISABLE_DEPRECATED flags @<:@default=no@:>@])],,
              [enable_deprecation_flags=no])

if test "x$enable_deprecation_flags" = "xyes"; then
   DISABLE_DEPRECATED_CFLAGS=$DISABLE_DEPRECATED
   AC_SUBST(DISABLE_DEPRECATED_CFLAGS)
fi

# GLib min/max required versions
AC_DEFINE([GLIB_VERSION_MAX_ALLOWED], [GLIB_VERSION_2_36],
	[Warn on use of APIs added after GLib 2.36])
AC_DEFINE([GLIB_VERSION_MIN_REQUIRED], [GLIB_VERSION_2_36],
	[Warn on use of APIs deprecated before GLib 2.36])

# Whether to build ukui-about
AC_ARG_ENABLE([ukui-about],
	[AS_HELP_STRING([--disable-ukui-about],[do not build ukui-about])],,
	[enable_ukui_about=yes])
AM_CONDITIONAL(UKUI_ABOUT_ENABLED, test "x$enable_ukui_about" = "xyes")


# Path to the pnp.ids file -- to know if we use one shipped with another
# package, or an internal file

AC_ARG_WITH(pnp-ids-path,
              [AC_HELP_STRING([--with-pnp-ids-path],
                              [Specify the path to pnp.ids @<:@default=(internal)@:>@])],,
                              [with_pnp_ids_path="\${pnpdatadir}/pnp.ids"])

AM_CONDITIONAL(USE_INTERNAL_PNP_IDS, test "x$with_pnp_ids_path" = "x\${pnpdatadir}/pnp.ids")
PNP_IDS=$with_pnp_ids_path
AC_SUBST(PNP_IDS)
if test "x$with_pnp_ids_path" = "x\${pnpdatadir}/pnp.ids"; then
        EXTERNAL_PNP_IDS="no (internal)"
else
        EXTERNAL_PNP_IDS="$with_pnp_ids_path"
fi

GLIB_REQUIRED=2.36.0
GIO_REQUIRED=2.26.0
GTK_REQUIRED=3.14.0
XRANDR_REQUIRED=1.3

dnl ----------------------------------------------------------------------------
dnl DCONF
dnl ----------------------------------------------------------------------------

DCONF_REQUIRED=0.13.4

PKG_CHECK_MODULES(DCONF, dconf >= $DCONF_REQUIRED)

AC_SUBST(DCONF_CFLAGS)
AC_SUBST(DCONF_LIBS)

dnl ----------------------------------------------------------------------------
dnl startup notification
dnl ----------------------------------------------------------------------------
STARTUP_NOTIFICATION_VERSION=0.5
STARTUP_NOTIFICATION_PACKAGE=

AC_ARG_ENABLE(startup-notification,
	AC_HELP_STRING([--disable-startup-notification], [disable startup notification support]),, enable_startup_notification=auto)

AC_MSG_CHECKING([Startup notification library >= $STARTUP_NOTIFICATION_VERSION])

if $PKG_CONFIG --atleast-version $STARTUP_NOTIFICATION_VERSION libstartup-notification-1.0; then
	have_startup_notification=yes
else
	have_startup_notification=no
fi

AC_MSG_RESULT($have_startup_notification)

if test x$enable_startup_notification = xyes; then
	have_startup_notification=yes
	echo "startup-notification support forced on"
elif test x$enable_startup_notification = xauto; then
	true
else
	have_startup_notification=no
fi

if test x$have_startup_notification = xyes; then
	echo "Building with libstartup-notification"
	STARTUP_NOTIFICATION_PACKAGE=libstartup-notification-1.0
	AC_DEFINE(HAVE_STARTUP_NOTIFICATION,, [Building with startup notification support])
else
	echo "Building without libstartup-notification"
fi

AC_SUBST(STARTUP_NOTIFICATION_PACKAGE)

GOBJECT_INTROSPECTION_CHECK([0.9.7])

dnl we need x11 for UkuiBG

PKG_CHECK_MODULES(XLIB, x11,
    X11_PACKAGE=x11,
    [X11_PACKAGE=
     AC_PATH_XTRA
     if test "x$no_x" = xyes; then
       AC_MSG_ERROR("no (requires X development libraries)")
     else
       XLIB_LIBS="$X_PRE_LIBS $X_LIBS -lX11 $X_EXTRA_LIBS"
       XLIB_CFLAGS=$X_CFLAGS
     fi])
AC_SUBST(X11_PACKAGE)
AC_SUBST(XLIB_CFLAGS)
AC_SUBST(XLIB_LIBS)

dnl Checks for X is RANDR extension

AC_MSG_CHECKING(for xrandr)
if $PKG_CONFIG --atleast-version $XRANDR_REQUIRED xrandr; then
  AC_MSG_RESULT(yes)
  AC_DEFINE(HAVE_RANDR, 1,
            [Define if the xrandr-$XRANDR_REQUIRED library is present])
  have_randr=yes
  RANDR_PACKAGE=xrandr
else
  AC_MSG_RESULT(no)
  have_randr=no
  RANDR_PACKAGE=
fi

AC_SUBST(RANDR_PACKAGE)

dnl pkg-config dependency checks

PKG_CHECK_MODULES(UKUI_DESKTOP, gdk-pixbuf-2.0 gtk+-3.0 >= $GTK_REQUIRED glib-2.0 >= $GLIB_REQUIRED gio-2.0 >= $GIO_REQUIRED $STARTUP_NOTIFICATION_PACKAGE $RANDR_PACKAGE)

PKG_CHECK_MODULES(UKUI_ABOUT, gtk+-3.0 >= $GTK_REQUIRED)
AC_SUBST(UKUI_ABOUT_CFLAGS)
AC_SUBST(UKUI_ABOUT_LIBS)

dnl Language Support

GETTEXT_PACKAGE=ukui-desktop
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE",
		   [The gettext translation domain])
AC_SUBST(GETTEXT_PACKAGE)

AM_GLIB_GNU_GETTEXT

GLIB_GSETTINGS

dnl Do not use AC_PROG_AWK since we need the full pathname.
AC_PATH_PROGS(AWK, mawk gawk nawk awk, )
AC_PATH_PROGS(PERL, perl5 perl)

dnl define a MAINT-like variable REBUILD which is set if Perl
dnl and awk are found, so autogenerated sources can be rebuilt
AC_ARG_ENABLE(rebuilds, [  --disable-rebuilds      disable all source autogeneration rules],,enable_rebuilds=yes)
REBUILD=\#
if test "x$enable_rebuilds" = "xyes" && \
     test -n "$PERL" && \
     $PERL -e 'exit !($] >= 5.002)' > /dev/null 2>&1 && \
     test -n "$AWK" ; then
  REBUILD=
fi
AC_SUBST(REBUILD)

AC_SEARCH_LIBS([sqrt], [m])

# check for gtk-doc
GTK_DOC_CHECK([1.4])

AC_CONFIG_FILES([
Makefile
ukui-about/Makefile
ukui-about/ukui-about.desktop.in
libukui-desktop/Makefile
libukui-desktop/ukui-desktop-2.0.pc
libukui-desktop/ukui-desktop-2.0-uninstalled.pc
docs/Makefile
docs/reference/Makefile
docs/reference/ukui-desktop/Makefile
ukui-version.xml.in
po/Makefile.in
schemas/Makefile
schemas/org.ukui.accessibility-keyboard.gschema.xml
schemas/org.ukui.accessibility-startup.gschema.xml
schemas/org.ukui.applications-at-mobility.gschema.xml
schemas/org.ukui.applications-at-visual.gschema.xml
schemas/org.ukui.applications-browser.gschema.xml
schemas/org.ukui.applications-office.gschema.xml
schemas/org.ukui.applications-terminal.gschema.xml
schemas/org.ukui.background.gschema.xml
schemas/org.ukui.debug.gschema.xml
schemas/org.ukui.file-views.gschema.xml
schemas/org.ukui.interface.gschema.xml
schemas/org.ukui.lockdown.gschema.xml
schemas/org.ukui.peripherals-keyboard.gschema.xml
schemas/org.ukui.peripherals-mouse.gschema.xml
schemas/org.ukui.sound.gschema.xml
schemas/org.ukui.thumbnail-cache.gschema.xml
schemas/org.ukui.thumbnailers.gschema.xml
schemas/org.ukui.typing-break.gschema.xml
man/Makefile
tools/ukui-color-select.desktop.in
tools/Makefile
icons/Makefile
])

AC_OUTPUT

dnl ---------------------------------------------------------------------------
dnl - Show summary
dnl ---------------------------------------------------------------------------

echo "
ukui-desktop $VERSION
`echo ukui-desktop $VERSION | sed "s/./=/g"`

    prefix:                       ${prefix}
    exec_prefix:                  ${exec_prefix}
    libdir:                       ${libdir}
    bindir:                       ${bindir}
    sbindir:                      ${sbindir}
    sysconfdir:                   ${sysconfdir}
    localstatedir:                ${localstatedir}
    datadir:                      ${datadir}
    source code location:         ${srcdir}
    compiler:                     ${CC}
    cflags:                       ${CFLAGS}
    Maintainer mode:              ${USE_MAINTAINER_MODE}
    Use *_DISABLE_DEPRECATED:     ${enable_deprecation_flags}

    Build ukui-about:             ${enable_ukui_about}
    Use external pnp.ids:         ${EXTERNAL_PNP_IDS}
    Startup notification support: ${have_startup_notification}
    XRandr support:               ${have_randr}
    Build introspection support:  ${found_introspection}
    Build gtk-doc documentation:  ${enable_gtk_doc}
"
