/* -*- Mode: C; c-set-style: linux indent-tabs-mode: t; c-basic-offset: 8; tab-width: 8 -*- */
/* ukui-ditem.h - UKUI Desktop File Representation

   Copyright (C) 1999, 2000 Red Hat Inc.
   Copyright (C) 2001 Sid Vicious
   All rights reserved.

   This file is part of the Ukui Library.

   The Ukui Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The Ukui Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the Ukui Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
   Boston, MA  02110-1301, USA.  */
/*
  @NOTATION@
 */

#ifndef UKUI_DITEM_H
#define UKUI_DITEM_H

#include <glib.h>
#include <glib-object.h>

#include <gdk/gdk.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum {
	UKUI_DESKTOP_ITEM_TYPE_NULL = 0 /* This means its NULL, that is, not
					   * set */,
	UKUI_DESKTOP_ITEM_TYPE_OTHER /* This means it's not one of the below
					      strings types, and you must get the
					      Type attribute. */,

	/* These are the standard compliant types: */
	UKUI_DESKTOP_ITEM_TYPE_APPLICATION,
	UKUI_DESKTOP_ITEM_TYPE_LINK,
	UKUI_DESKTOP_ITEM_TYPE_FSDEVICE,
	UKUI_DESKTOP_ITEM_TYPE_MIME_TYPE,
	UKUI_DESKTOP_ITEM_TYPE_DIRECTORY,
	UKUI_DESKTOP_ITEM_TYPE_SERVICE,
	UKUI_DESKTOP_ITEM_TYPE_SERVICE_TYPE
} UkuiDesktopItemType;

typedef enum {
        UKUI_DESKTOP_ITEM_UNCHANGED = 0,
        UKUI_DESKTOP_ITEM_CHANGED = 1,
        UKUI_DESKTOP_ITEM_DISAPPEARED = 2
} UkuiDesktopItemStatus;

#define UKUI_TYPE_DESKTOP_ITEM         (ukui_desktop_item_get_type ())
GType ukui_desktop_item_get_type       (void);

typedef struct _UkuiDesktopItem UkuiDesktopItem;

/* standard */
#define UKUI_DESKTOP_ITEM_ENCODING	"Encoding" /* string */
#define UKUI_DESKTOP_ITEM_VERSION	"Version"  /* numeric */
#define UKUI_DESKTOP_ITEM_NAME		"Name" /* localestring */
#define UKUI_DESKTOP_ITEM_GENERIC_NAME	"GenericName" /* localestring */
#define UKUI_DESKTOP_ITEM_TYPE		"Type" /* string */
#define UKUI_DESKTOP_ITEM_FILE_PATTERN "FilePattern" /* regexp(s) */
#define UKUI_DESKTOP_ITEM_TRY_EXEC	"TryExec" /* string */
#define UKUI_DESKTOP_ITEM_NO_DISPLAY	"NoDisplay" /* boolean */
#define UKUI_DESKTOP_ITEM_COMMENT	"Comment" /* localestring */
#define UKUI_DESKTOP_ITEM_EXEC		"Exec" /* string */
#define UKUI_DESKTOP_ITEM_ACTIONS	"Actions" /* strings */
#define UKUI_DESKTOP_ITEM_ICON		"Icon" /* string */
#define UKUI_DESKTOP_ITEM_MINI_ICON	"MiniIcon" /* string */
#define UKUI_DESKTOP_ITEM_HIDDEN	"Hidden" /* boolean */
#define UKUI_DESKTOP_ITEM_PATH		"Path" /* string */
#define UKUI_DESKTOP_ITEM_TERMINAL	"Terminal" /* boolean */
#define UKUI_DESKTOP_ITEM_TERMINAL_OPTIONS "TerminalOptions" /* string */
#define UKUI_DESKTOP_ITEM_SWALLOW_TITLE "SwallowTitle" /* string */
#define UKUI_DESKTOP_ITEM_SWALLOW_EXEC	"SwallowExec" /* string */
#define UKUI_DESKTOP_ITEM_MIME_TYPE	"MimeType" /* regexp(s) */
#define UKUI_DESKTOP_ITEM_PATTERNS	"Patterns" /* regexp(s) */
#define UKUI_DESKTOP_ITEM_DEFAULT_APP	"DefaultApp" /* string */
#define UKUI_DESKTOP_ITEM_DEV		"Dev" /* string */
#define UKUI_DESKTOP_ITEM_FS_TYPE	"FSType" /* string */
#define UKUI_DESKTOP_ITEM_MOUNT_POINT	"MountPoint" /* string */
#define UKUI_DESKTOP_ITEM_READ_ONLY	"ReadOnly" /* boolean */
#define UKUI_DESKTOP_ITEM_UNMOUNT_ICON "UnmountIcon" /* string */
#define UKUI_DESKTOP_ITEM_SORT_ORDER	"SortOrder" /* strings */
#define UKUI_DESKTOP_ITEM_URL		"URL" /* string */
#define UKUI_DESKTOP_ITEM_DOC_PATH	"X-UKUI-DocPath" /* string */

/* The vfolder proposal */
#define UKUI_DESKTOP_ITEM_CATEGORIES	"Categories" /* string */
#define UKUI_DESKTOP_ITEM_ONLY_SHOW_IN	"OnlyShowIn" /* string */

typedef enum {
	/* Use the TryExec field to determine if this should be loaded */
        UKUI_DESKTOP_ITEM_LOAD_ONLY_IF_EXISTS = 1<<0,
        UKUI_DESKTOP_ITEM_LOAD_NO_TRANSLATIONS = 1<<1
} UkuiDesktopItemLoadFlags;

typedef enum {
	/* Never launch more instances even if the app can only
	 * handle one file and we have passed many */
        UKUI_DESKTOP_ITEM_LAUNCH_ONLY_ONE = 1<<0,
	/* Use current directory instead of home directory */
        UKUI_DESKTOP_ITEM_LAUNCH_USE_CURRENT_DIR = 1<<1,
	/* Append the list of URIs to the command if no Exec
	 * parameter is specified, instead of launching the
	 * app without parameters. */
	UKUI_DESKTOP_ITEM_LAUNCH_APPEND_URIS = 1<<2,
	/* Same as above but instead append local paths */
	UKUI_DESKTOP_ITEM_LAUNCH_APPEND_PATHS = 1<<3,
	/* Don't automatically reap child process.  */
	UKUI_DESKTOP_ITEM_LAUNCH_DO_NOT_REAP_CHILD = 1<<4
} UkuiDesktopItemLaunchFlags;

typedef enum {
	/* Don't check the kde directories */
        UKUI_DESKTOP_ITEM_ICON_NO_KDE = 1<<0
} UkuiDesktopItemIconFlags;

typedef enum {
	UKUI_DESKTOP_ITEM_ERROR_NO_FILENAME /* No filename set or given on save */,
	UKUI_DESKTOP_ITEM_ERROR_UNKNOWN_ENCODING /* Unknown encoding of the file */,
	UKUI_DESKTOP_ITEM_ERROR_CANNOT_OPEN /* Cannot open file */,
	UKUI_DESKTOP_ITEM_ERROR_NO_EXEC_STRING /* Cannot launch due to no execute string */,
	UKUI_DESKTOP_ITEM_ERROR_BAD_EXEC_STRING /* Cannot launch due to bad execute string */,
	UKUI_DESKTOP_ITEM_ERROR_NO_URL /* No URL on a url entry*/,
	UKUI_DESKTOP_ITEM_ERROR_NOT_LAUNCHABLE /* Not a launchable type of item */,
	UKUI_DESKTOP_ITEM_ERROR_INVALID_TYPE /* Not of type application/x-ukui-app-info */
} UkuiDesktopItemError;

/* Note that functions can also return the G_FILE_ERROR_* errors */

#define UKUI_DESKTOP_ITEM_ERROR ukui_desktop_item_error_quark ()
GQuark ukui_desktop_item_error_quark (void);

/* Returned item from new*() and copy() methods have a refcount of 1 */
UkuiDesktopItem *      ukui_desktop_item_new               (void);
UkuiDesktopItem *      ukui_desktop_item_new_from_file     (const char                 *file,
							      UkuiDesktopItemLoadFlags   flags,
							      GError                    **error);
UkuiDesktopItem *      ukui_desktop_item_new_from_uri      (const char                 *uri,
							      UkuiDesktopItemLoadFlags   flags,
							      GError                    **error);
UkuiDesktopItem *      ukui_desktop_item_new_from_string   (const char                 *uri,
							      const char                 *string,
							      gssize                      length,
							      UkuiDesktopItemLoadFlags   flags,
							      GError                    **error);
UkuiDesktopItem *      ukui_desktop_item_new_from_basename (const char                 *basename,
							      UkuiDesktopItemLoadFlags   flags,
							      GError                    **error);
UkuiDesktopItem *      ukui_desktop_item_copy              (const UkuiDesktopItem     *item);

/* if under is NULL save in original location */
gboolean                ukui_desktop_item_save              (UkuiDesktopItem           *item,
							      const char                 *under,
							      gboolean			  force,
							      GError                    **error);
UkuiDesktopItem *      ukui_desktop_item_ref               (UkuiDesktopItem           *item);
void                    ukui_desktop_item_unref             (UkuiDesktopItem           *item);
int			ukui_desktop_item_launch	     (const UkuiDesktopItem     *item,
							      GList                      *file_list,
							      UkuiDesktopItemLaunchFlags flags,
							      GError                    **error);
int			ukui_desktop_item_launch_with_env   (const UkuiDesktopItem     *item,
							      GList                      *file_list,
							      UkuiDesktopItemLaunchFlags flags,
							      char                      **envp,
							      GError                    **error);

int                     ukui_desktop_item_launch_on_screen  (const UkuiDesktopItem       *item,
							      GList                        *file_list,
							      UkuiDesktopItemLaunchFlags   flags,
							      GdkScreen                    *screen,
							      int                           workspace,
							      GError                      **error);

/* A list of files or urls dropped onto an icon */
int                     ukui_desktop_item_drop_uri_list     (const UkuiDesktopItem     *item,
							      const char                 *uri_list,
							      UkuiDesktopItemLaunchFlags flags,
							      GError                    **error);

int                     ukui_desktop_item_drop_uri_list_with_env    (const UkuiDesktopItem     *item,
								      const char                 *uri_list,
								      UkuiDesktopItemLaunchFlags flags,
								      char                      **envp,
								      GError                    **error);

gboolean                ukui_desktop_item_exists            (const UkuiDesktopItem     *item);

UkuiDesktopItemType	ukui_desktop_item_get_entry_type    (const UkuiDesktopItem	 *item);
/* You could also just use the set_string on the TYPE argument */
void			ukui_desktop_item_set_entry_type    (UkuiDesktopItem		 *item,
							      UkuiDesktopItemType	  type);

/* Get current location on disk */
const char *            ukui_desktop_item_get_location      (const UkuiDesktopItem     *item);
void                    ukui_desktop_item_set_location      (UkuiDesktopItem           *item,
							      const char                 *location);
void                    ukui_desktop_item_set_location_file (UkuiDesktopItem           *item,
							      const char                 *file);
UkuiDesktopItemStatus  ukui_desktop_item_get_file_status   (const UkuiDesktopItem     *item);

/*
 * Get the icon, this is not as simple as getting the Icon attr as it actually tries to find
 * it and returns %NULL if it can't
 */
char *                  ukui_desktop_item_get_icon          (const UkuiDesktopItem     *item,
							      GtkIconTheme               *icon_theme);

char *                  ukui_desktop_item_find_icon         (GtkIconTheme               *icon_theme,
							      const char                 *icon,
							      /* size is only a suggestion */
							      int                         desired_size,
							      int                         flags);


/*
 * Reading/Writing different sections, NULL is the standard section
 */
gboolean                ukui_desktop_item_attr_exists       (const UkuiDesktopItem     *item,
							      const char                 *attr);

/*
 * String type
 */
const char *            ukui_desktop_item_get_string        (const UkuiDesktopItem     *item,
							      const char		 *attr);

void                    ukui_desktop_item_set_string        (UkuiDesktopItem           *item,
							      const char		 *attr,
							      const char                 *value);

const char *            ukui_desktop_item_get_attr_locale   (const UkuiDesktopItem     *item,
							      const char		 *attr);

/*
 * LocaleString type
 */
const char *            ukui_desktop_item_get_localestring  (const UkuiDesktopItem     *item,
							      const char		 *attr);
const char *            ukui_desktop_item_get_localestring_lang (const UkuiDesktopItem *item,
								  const char		 *attr,
								  const char             *language);
/* use g_list_free only */
GList *                 ukui_desktop_item_get_languages     (const UkuiDesktopItem     *item,
							      const char		 *attr);

void                    ukui_desktop_item_set_localestring  (UkuiDesktopItem           *item,
							      const char		 *attr,
							      const char                 *value);
void                    ukui_desktop_item_set_localestring_lang  (UkuiDesktopItem      *item,
								   const char		 *attr,
								   const char		 *language,
								   const char            *value);
void                    ukui_desktop_item_clear_localestring(UkuiDesktopItem           *item,
							      const char		 *attr);

/*
 * Strings, Regexps types
 */

/* use ukui_desktop_item_free_string_list */
char **                 ukui_desktop_item_get_strings       (const UkuiDesktopItem     *item,
							      const char		 *attr);

void			ukui_desktop_item_set_strings       (UkuiDesktopItem           *item,
							      const char                 *attr,
							      char                      **strings);

/*
 * Boolean type
 */
gboolean                ukui_desktop_item_get_boolean       (const UkuiDesktopItem     *item,
							      const char		 *attr);

void                    ukui_desktop_item_set_boolean       (UkuiDesktopItem           *item,
							      const char		 *attr,
							      gboolean                    value);

/*
 * Xserver time of user action that caused the application launch to start.
 */
void                    ukui_desktop_item_set_launch_time   (UkuiDesktopItem           *item,
							      guint32                     timestamp);

/*
 * Clearing attributes
 */
#define                 ukui_desktop_item_clear_attr(item,attr) \
				ukui_desktop_item_set_string(item,attr,NULL)
void			ukui_desktop_item_clear_section     (UkuiDesktopItem           *item,
							      const char                 *section);

G_END_DECLS

#endif /* UKUI_DITEM_H */
