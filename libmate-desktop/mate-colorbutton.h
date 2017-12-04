/*
 * GTK - The GIMP Toolkit
 * Copyright (C) 1998, 1999 Red Hat, Inc.
 * All rights reserved.
 *
 * This Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with the Ukui Library; see the file COPYING.LIB. If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

/* Color picker button for GNOME
 *
 * Author: Federico Mena <federico@nuclecu.unam.mx>
 *
 * Modified by the GTK+ Team and others 2003.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/.
 */

#ifndef __UKUI_COLOR_BUTTON_H__
#define __UKUI_COLOR_BUTTON_H__

#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS


/* The UkuiColorButton widget is a simple color picker in a button.
 * The button displays a sample of the currently selected color.  When
 * the user clicks on the button, a color selection dialog pops up.
 * The color picker emits the "color_set" signal when the color is set.
 */

#define UKUI_TYPE_COLOR_BUTTON             (ukui_color_button_get_type ())
#define UKUI_COLOR_BUTTON(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), UKUI_TYPE_COLOR_BUTTON, UkuiColorButton))
#define UKUI_COLOR_BUTTON_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), UKUI_TYPE_COLOR_BUTTON, UkuiColorButtonClass))
#define UKUI_IS_COLOR_BUTTON(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UKUI_TYPE_COLOR_BUTTON))
#define UKUI_IS_COLOR_BUTTON_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), UKUI_TYPE_COLOR_BUTTON))
#define UKUI_COLOR_BUTTON_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), UKUI_TYPE_COLOR_BUTTON, UkuiColorButtonClass))

typedef struct _UkuiColorButton          UkuiColorButton;
typedef struct _UkuiColorButtonClass     UkuiColorButtonClass;
typedef struct _UkuiColorButtonPrivate   UkuiColorButtonPrivate;

struct _UkuiColorButton {
  GtkButton button;

  /*< private >*/

  UkuiColorButtonPrivate *priv;
};

struct _UkuiColorButtonClass {
  GtkButtonClass parent_class;

  void (* color_set) (UkuiColorButton *cp);

  /* Padding for future expansion */
  void (*_gtk_reserved1) (void);
  void (*_gtk_reserved2) (void);
  void (*_gtk_reserved3) (void);
  void (*_gtk_reserved4) (void);
};


GType      ukui_color_button_get_type       (void) G_GNUC_CONST;
GtkWidget *ukui_color_button_new            (void);
GtkWidget *ukui_color_button_new_with_color (const GdkColor *color);
void       ukui_color_button_set_color      (UkuiColorButton *color_button,
					    const GdkColor *color);
void       ukui_color_button_set_rgba       (UkuiColorButton *color_button,
					     const GdkRGBA   *color);
void       ukui_color_button_set_alpha      (UkuiColorButton *color_button,
					    guint16         alpha);
void       ukui_color_button_get_color      (UkuiColorButton *color_button,
					    GdkColor       *color);
void       ukui_color_button_get_rgba       (UkuiColorButton *color_button,
					     GdkRGBA         *color);
guint16    ukui_color_button_get_alpha      (UkuiColorButton *color_button);
void       ukui_color_button_set_use_alpha  (UkuiColorButton *color_button,
					    gboolean        use_alpha);
gboolean   ukui_color_button_get_use_alpha  (UkuiColorButton *color_button);
void       ukui_color_button_set_title      (UkuiColorButton *color_button,
					    const gchar    *title);
const gchar *ukui_color_button_get_title (UkuiColorButton *color_button);


G_END_DECLS

#endif  /* __UKUI_COLOR_BUTTON_H__ */
