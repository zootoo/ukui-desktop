/* GTK - The GIMP Toolkit
 * Copyright (C) 2000 Red Hat, Inc.
 * Copyright (C) 1995-1997 Peter Mattis, Spencer Kimball and Josh MacDonald
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/*
 * Modified by the GTK+ Team and others 1997-2000.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __UKUI_COLOR_SELECTION_H__
#define __UKUI_COLOR_SELECTION_H__

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define UKUI_TYPE_COLOR_SELECTION			(ukui_color_selection_get_type ())
#define UKUI_COLOR_SELECTION(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), UKUI_TYPE_COLOR_SELECTION, UkuiColorSelection))
#define UKUI_COLOR_SELECTION_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), UKUI_TYPE_COLOR_SELECTION, UkuiColorSelectionClass))
#define UKUI_IS_COLOR_SELECTION(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), UKUI_TYPE_COLOR_SELECTION))
#define UKUI_IS_COLOR_SELECTION_CLASS(klass)		(G_TYPE_CHECK_CLASS_TYPE ((klass), UKUI_TYPE_COLOR_SELECTION))
#define UKUI_COLOR_SELECTION_GET_CLASS(obj)              (G_TYPE_INSTANCE_GET_CLASS ((obj), UKUI_TYPE_COLOR_SELECTION, UkuiColorSelectionClass))


typedef struct _UkuiColorSelection       UkuiColorSelection;
typedef struct _UkuiColorSelectionClass  UkuiColorSelectionClass;
typedef struct _ColorSelectionPrivate    ColorSelectionPrivate;


typedef void (* UkuiColorSelectionChangePaletteFunc) (const GdkColor    *colors,
                                                     gint               n_colors);
typedef void (* UkuiColorSelectionChangePaletteWithScreenFunc) (GdkScreen         *screen,
							       const GdkColor    *colors,
							       gint               n_colors);

struct _UkuiColorSelection
{
  GtkBox parent_instance;

  /* < private_data > */
  ColorSelectionPrivate *private_data;
};

struct _UkuiColorSelectionClass
{
  GtkBoxClass parent_class;

  void (*color_changed)	(UkuiColorSelection *color_selection);

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


/* ColorSelection */

GType      ukui_color_selection_get_type                (void) G_GNUC_CONST;
GtkWidget *ukui_color_selection_new                     (void);
gboolean   ukui_color_selection_get_has_opacity_control (UkuiColorSelection *colorsel);
void       ukui_color_selection_set_has_opacity_control (UkuiColorSelection *colorsel,
							gboolean           has_opacity);
gboolean   ukui_color_selection_get_has_palette         (UkuiColorSelection *colorsel);
void       ukui_color_selection_set_has_palette         (UkuiColorSelection *colorsel,
							gboolean           has_palette);


void     ukui_color_selection_set_current_color   (UkuiColorSelection *colorsel,
						  const GdkColor    *color);
void     ukui_color_selection_set_current_alpha   (UkuiColorSelection *colorsel,
						  guint16            alpha);
void     ukui_color_selection_get_current_color   (UkuiColorSelection *colorsel,
						  GdkColor          *color);
guint16  ukui_color_selection_get_current_alpha   (UkuiColorSelection *colorsel);
void     ukui_color_selection_set_previous_color  (UkuiColorSelection *colorsel,
						  const GdkColor    *color);
void     ukui_color_selection_set_previous_alpha  (UkuiColorSelection *colorsel,
						  guint16            alpha);
void     ukui_color_selection_get_previous_color  (UkuiColorSelection *colorsel,
						  GdkColor          *color);
guint16  ukui_color_selection_get_previous_alpha  (UkuiColorSelection *colorsel);

gboolean ukui_color_selection_is_adjusting        (UkuiColorSelection *colorsel);

gboolean ukui_color_selection_palette_from_string (const gchar       *str,
                                                  GdkColor         **colors,
                                                  gint              *n_colors);
gchar*   ukui_color_selection_palette_to_string   (const GdkColor    *colors,
                                                  gint               n_colors);

#ifndef GTK_DISABLE_DEPRECATED
#ifndef GDK_MULTIHEAD_SAFE
UkuiColorSelectionChangePaletteFunc           ukui_color_selection_set_change_palette_hook             (UkuiColorSelectionChangePaletteFunc           func);
#endif
#endif

UkuiColorSelectionChangePaletteWithScreenFunc ukui_color_selection_set_change_palette_with_screen_hook (UkuiColorSelectionChangePaletteWithScreenFunc func);

#ifndef GTK_DISABLE_DEPRECATED
/* Deprecated calls: */
void ukui_color_selection_set_color         (UkuiColorSelection *colorsel,
					    gdouble           *color);
void ukui_color_selection_get_color         (UkuiColorSelection *colorsel,
					    gdouble           *color);
#endif /* GTK_DISABLE_DEPRECATED */

G_END_DECLS

#endif /* __UKUI_COLOR_SELECTION_H__ */
