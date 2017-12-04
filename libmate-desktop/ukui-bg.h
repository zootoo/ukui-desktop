/* ukui-bg.h -

   Copyright (C) 2007 Red Hat, Inc.
   Copyright (C) 2012 Jasmine Hassan <jasmine.aura@gmail.com>

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
   Boston, MA  02110-1301, USA.

   Authors: Soren Sandmann <sandmann@redhat.com>
	    Jasmine Hassan <jasmine.aura@gmail.com>
*/

#ifndef __UKUI_BG_H__
#define __UKUI_BG_H__

#ifndef UKUI_DESKTOP_USE_UNSTABLE_API
#error    UkuiBG is unstable API. You must define UKUI_DESKTOP_USE_UNSTABLE_API before including ukui-bg.h
#endif

#include <glib.h>
#include <gdk/gdk.h>
#include <gio/gio.h>
#include "ukui-desktop-thumbnail.h"
#include "ukui-bg-crossfade.h"

G_BEGIN_DECLS

#define UKUI_TYPE_BG            (ukui_bg_get_type ())
#define UKUI_BG(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UKUI_TYPE_BG, UkuiBG))
#define UKUI_BG_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UKUI_TYPE_BG, UkuiBGClass))
#define UKUI_IS_BG(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UKUI_TYPE_BG))
#define UKUI_IS_BG_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UKUI_TYPE_BG))
#define UKUI_BG_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UKUI_TYPE_BG, UkuiBGClass))

#define UKUI_BG_SCHEMA "org.ukui.background"

/* whether to draw the desktop bg */
#define UKUI_BG_KEY_DRAW_BACKGROUND	"draw-background"

/* whether Caja or ukui-settings-daemon draw the desktop */
#define UKUI_BG_KEY_SHOW_DESKTOP	"show-desktop-icons"

/* whether to fade when changing background (By Caja/m-s-d) */
#define UKUI_BG_KEY_BACKGROUND_FADE	"background-fade"

#define UKUI_BG_KEY_PRIMARY_COLOR	"primary-color"
#define UKUI_BG_KEY_SECONDARY_COLOR	"secondary-color"
#define UKUI_BG_KEY_COLOR_TYPE		"color-shading-type"
#define UKUI_BG_KEY_PICTURE_PLACEMENT	"picture-options"
#define UKUI_BG_KEY_PICTURE_OPACITY	"picture-opacity"
#define UKUI_BG_KEY_PICTURE_FILENAME	"picture-filename"

typedef struct _UkuiBG UkuiBG;
typedef struct _UkuiBGClass UkuiBGClass;

typedef enum {
	UKUI_BG_COLOR_SOLID,
	UKUI_BG_COLOR_H_GRADIENT,
	UKUI_BG_COLOR_V_GRADIENT
} UkuiBGColorType;

typedef enum {
	UKUI_BG_PLACEMENT_TILED,
	UKUI_BG_PLACEMENT_ZOOMED,
	UKUI_BG_PLACEMENT_CENTERED,
	UKUI_BG_PLACEMENT_SCALED,
	UKUI_BG_PLACEMENT_FILL_SCREEN,
	UKUI_BG_PLACEMENT_SPANNED
} UkuiBGPlacement;

GType            ukui_bg_get_type              (void);
UkuiBG *         ukui_bg_new                   (void);
void             ukui_bg_load_from_preferences (UkuiBG               *bg);
void             ukui_bg_load_from_system_preferences  (UkuiBG       *bg);
void             ukui_bg_load_from_system_gsettings    (UkuiBG       *bg,
							GSettings    *settings,
							gboolean      reset_apply);
void             ukui_bg_load_from_gsettings   (UkuiBG               *bg,
						GSettings            *settings);
void             ukui_bg_save_to_preferences   (UkuiBG               *bg);
void             ukui_bg_save_to_gsettings     (UkuiBG               *bg,
						GSettings            *settings);

/* Setters */
void             ukui_bg_set_filename          (UkuiBG               *bg,
						 const char            *filename);
void             ukui_bg_set_placement         (UkuiBG               *bg,
						 UkuiBGPlacement       placement);
void             ukui_bg_set_color             (UkuiBG               *bg,
						 UkuiBGColorType       type,
						 GdkRGBA              *primary,
						 GdkRGBA              *secondary);
void		 ukui_bg_set_draw_background   (UkuiBG		     *bg,
						gboolean	      draw_background);
/* Getters */
gboolean	 ukui_bg_get_draw_background   (UkuiBG		     *bg);
UkuiBGPlacement  ukui_bg_get_placement         (UkuiBG               *bg);
void		 ukui_bg_get_color             (UkuiBG               *bg,
						 UkuiBGColorType      *type,
						 GdkRGBA              *primary,
						 GdkRGBA              *secondary);
const gchar *    ukui_bg_get_filename          (UkuiBG               *bg);

/* Drawing and thumbnailing */
void             ukui_bg_draw                  (UkuiBG               *bg,
						 GdkPixbuf             *dest,
						 GdkScreen	       *screen,
                                                 gboolean               is_root);

cairo_surface_t *ukui_bg_create_surface        (UkuiBG               *bg,
						GdkWindow            *window,
						int                   width,
						int                   height,
						gboolean              root);

gboolean         ukui_bg_get_image_size        (UkuiBG               *bg,
						 UkuiDesktopThumbnailFactory *factory,
                                                 int                    best_width,
                                                 int                    best_height,
						 int                   *width,
						 int                   *height);
GdkPixbuf *      ukui_bg_create_thumbnail      (UkuiBG               *bg,
						 UkuiDesktopThumbnailFactory *factory,
						 GdkScreen             *screen,
						 int                    dest_width,
						 int                    dest_height);
gboolean         ukui_bg_is_dark               (UkuiBG               *bg,
                                                 int                    dest_width,
						 int                    dest_height);
gboolean         ukui_bg_has_multiple_sizes    (UkuiBG               *bg);
gboolean         ukui_bg_changes_with_time     (UkuiBG               *bg);
GdkPixbuf *      ukui_bg_create_frame_thumbnail (UkuiBG              *bg,
						 UkuiDesktopThumbnailFactory *factory,
						 GdkScreen             *screen,
						 int                    dest_width,
						 int                    dest_height,
						 int                    frame_num);

/* Set a surface as root - not a UkuiBG method. At some point
 * if we decide to stabilize the API then we may want to make
 * these object methods, drop ukui_bg_create_surface, etc.
 */
void             ukui_bg_set_surface_as_root   (GdkScreen            *screen,
						cairo_surface_t    *surface);
UkuiBGCrossfade *ukui_bg_set_surface_as_root_with_crossfade (GdkScreen       *screen,
							     cairo_surface_t *surface);
cairo_surface_t *ukui_bg_get_surface_from_root (GdkScreen *screen);

G_END_DECLS

#endif
