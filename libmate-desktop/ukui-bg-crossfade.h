/* ukui-bg-crossfade.h - fade window background between two pixmaps

   Copyright 2008, Red Hat, Inc.

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
   write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
   Floor, Boston, MA 02110-1301 US.

   Author: Ray Strode <rstrode@redhat.com>
*/

#ifndef __UKUI_BG_CROSSFADE_H__
#define __UKUI_BG_CROSSFADE_H__

#ifndef UKUI_DESKTOP_USE_UNSTABLE_API
#error    UkuiBGCrossfade is unstable API. You must define UKUI_DESKTOP_USE_UNSTABLE_API before including ukui-bg-crossfade.h
#endif

#include <glib.h>
#include <gdk/gdk.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UKUI_TYPE_BG_CROSSFADE            (ukui_bg_crossfade_get_type ())
#define UKUI_BG_CROSSFADE(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UKUI_TYPE_BG_CROSSFADE, UkuiBGCrossfade))
#define UKUI_BG_CROSSFADE_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UKUI_TYPE_BG_CROSSFADE, UkuiBGCrossfadeClass))
#define UKUI_IS_BG_CROSSFADE(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UKUI_TYPE_BG_CROSSFADE))
#define UKUI_IS_BG_CROSSFADE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UKUI_TYPE_BG_CROSSFADE))
#define UKUI_BG_CROSSFADE_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UKUI_TYPE_BG_CROSSFADE, UkuiBGCrossfadeClass))

typedef struct _UkuiBGCrossfadePrivate UkuiBGCrossfadePrivate;
typedef struct _UkuiBGCrossfade UkuiBGCrossfade;
typedef struct _UkuiBGCrossfadeClass UkuiBGCrossfadeClass;

struct _UkuiBGCrossfade
{
	GObject parent_object;

	UkuiBGCrossfadePrivate *priv;
};

struct _UkuiBGCrossfadeClass
{
	GObjectClass parent_class;

	void (* finished) (UkuiBGCrossfade *fade, GdkWindow *window);
};

GType             ukui_bg_crossfade_get_type              (void);
UkuiBGCrossfade *ukui_bg_crossfade_new (int width, int height);

gboolean          ukui_bg_crossfade_set_start_surface (UkuiBGCrossfade *fade,
						       cairo_surface_t *surface);
gboolean          ukui_bg_crossfade_set_end_surface (UkuiBGCrossfade *fade,
						     cairo_surface_t *surface);

void              ukui_bg_crossfade_start (UkuiBGCrossfade *fade,
                                           GdkWindow        *window);
void              ukui_bg_crossfade_start_widget (UkuiBGCrossfade *fade,
                                                  GtkWidget       *widget);
gboolean          ukui_bg_crossfade_is_started (UkuiBGCrossfade *fade);
void              ukui_bg_crossfade_stop (UkuiBGCrossfade *fade);

G_END_DECLS

#endif
