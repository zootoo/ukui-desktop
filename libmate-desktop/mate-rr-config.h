/* ukui-rr-config.h
 * -*- c-basic-offset: 4 -*-
 *
 * Copyright 2007, 2008, Red Hat, Inc.
 * Copyright 2010 Giovanni Campagna
 * 
 * This file is part of the Ukui Library.
 * 
 * The Ukui Library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * The Ukui Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * 
 * You should have received a copy of the GNU Library General Public
 * License along with the Ukui Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor,
 * Boston, MA  02110-1301, USA.
 * 
 * Author: Soren Sandmann <sandmann@redhat.com>
 */
#ifndef UKUI_RR_CONFIG_H
#define UKUI_RR_CONFIG_H

#ifndef UKUI_DESKTOP_USE_UNSTABLE_API
#error   ukui-rr-config.h is unstable API. You must define UKUI_DESKTOP_USE_UNSTABLE_API before including ukui-rr-config.h
#endif

#include "ukui-rr.h"
#include <glib.h>
#include <glib-object.h>

typedef struct UkuiRROutputInfoPrivate UkuiRROutputInfoPrivate;
typedef struct UkuiRRConfigPrivate UkuiRRConfigPrivate;

typedef struct
{
    GObject parent;

    /*< private >*/
    UkuiRROutputInfoPrivate *priv;
} UkuiRROutputInfo;

typedef struct
{
    GObjectClass parent_class;
} UkuiRROutputInfoClass;

#define UKUI_TYPE_RR_OUTPUT_INFO                  (ukui_rr_output_info_get_type())
#define UKUI_RR_OUTPUT_INFO(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), UKUI_TYPE_RR_OUTPUT_INFO, UkuiRROutputInfo))
#define UKUI_IS_RR_OUTPUT_INFO(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UKUI_TYPE_RR_OUTPUT_INFO))
#define UKUI_RR_OUTPUT_INFO_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), UKUI_TYPE_RR_OUTPUT_INFO, UkuiRROutputInfoClass))
#define UKUI_IS_RR_OUTPUT_INFO_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), UKUI_TYPE_RR_OUTPUT_INFO))
#define UKUI_RR_OUTPUT_INFO_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), UKUI_TYPE_RR_OUTPUT_INFO, UkuiRROutputInfoClass))

GType ukui_rr_output_info_get_type (void);

char *ukui_rr_output_info_get_name (UkuiRROutputInfo *self);

gboolean ukui_rr_output_info_is_active  (UkuiRROutputInfo *self);
void     ukui_rr_output_info_set_active (UkuiRROutputInfo *self, gboolean active);


void ukui_rr_output_info_get_geometry (UkuiRROutputInfo *self, int *x, int *y, int *width, int *height);
void ukui_rr_output_info_set_geometry (UkuiRROutputInfo *self, int  x, int  y, int  width, int  height);

int  ukui_rr_output_info_get_refresh_rate (UkuiRROutputInfo *self);
void ukui_rr_output_info_set_refresh_rate (UkuiRROutputInfo *self, int rate);

UkuiRRRotation ukui_rr_output_info_get_rotation (UkuiRROutputInfo *self);
void            ukui_rr_output_info_set_rotation (UkuiRROutputInfo *self, UkuiRRRotation rotation);

gboolean ukui_rr_output_info_is_connected     (UkuiRROutputInfo *self);
void     ukui_rr_output_info_get_vendor       (UkuiRROutputInfo *self, gchar* vendor);
guint    ukui_rr_output_info_get_product      (UkuiRROutputInfo *self);
guint    ukui_rr_output_info_get_serial       (UkuiRROutputInfo *self);
double   ukui_rr_output_info_get_aspect_ratio (UkuiRROutputInfo *self);
char    *ukui_rr_output_info_get_display_name (UkuiRROutputInfo *self);

gboolean ukui_rr_output_info_get_primary (UkuiRROutputInfo *self);
void     ukui_rr_output_info_set_primary (UkuiRROutputInfo *self, gboolean primary);

int ukui_rr_output_info_get_preferred_width  (UkuiRROutputInfo *self);
int ukui_rr_output_info_get_preferred_height (UkuiRROutputInfo *self);

typedef struct
{
    GObject parent;

    /*< private >*/
    UkuiRRConfigPrivate *priv;
} UkuiRRConfig;

typedef struct
{
    GObjectClass parent_class;
} UkuiRRConfigClass;

#define UKUI_TYPE_RR_CONFIG                  (ukui_rr_config_get_type())
#define UKUI_RR_CONFIG(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), UKUI_TYPE_RR_CONFIG, UkuiRRConfig))
#define UKUI_IS_RR_CONFIG(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UKUI_TYPE_RR_CONFIG))
#define UKUI_RR_CONFIG_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), UKUI_TYPE_RR_CONFIG, UkuiRRConfigClass))
#define UKUI_IS_RR_CONFIG_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), UKUI_TYPE_RR_CONFIG))
#define UKUI_RR_CONFIG_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), UKUI_TYPE_RR_CONFIG, UkuiRRConfigClass))

GType               ukui_rr_config_get_type     (void);

UkuiRRConfig      *ukui_rr_config_new_current  (UkuiRRScreen  *screen,
						  GError        **error);
UkuiRRConfig      *ukui_rr_config_new_stored   (UkuiRRScreen  *screen,
						  GError        **error);
gboolean                ukui_rr_config_load_current (UkuiRRConfig  *self,
						      GError        **error);
gboolean                ukui_rr_config_load_filename (UkuiRRConfig  *self,
						       const gchar    *filename,
						       GError        **error);
gboolean            ukui_rr_config_match        (UkuiRRConfig  *config1,
						  UkuiRRConfig  *config2);
gboolean            ukui_rr_config_equal	 (UkuiRRConfig  *config1,
						  UkuiRRConfig  *config2);
gboolean            ukui_rr_config_save         (UkuiRRConfig  *configuration,
						  GError        **error);
void                ukui_rr_config_sanitize     (UkuiRRConfig  *configuration);
gboolean            ukui_rr_config_ensure_primary (UkuiRRConfig  *configuration);

gboolean	    ukui_rr_config_apply_with_time (UkuiRRConfig  *configuration,
						     UkuiRRScreen  *screen,
						     guint32         timestamp,
						     GError        **error);

gboolean            ukui_rr_config_apply_from_filename_with_time (UkuiRRScreen  *screen,
								   const char     *filename,
								   guint32         timestamp,
								   GError        **error);

gboolean            ukui_rr_config_applicable   (UkuiRRConfig  *configuration,
						  UkuiRRScreen  *screen,
						  GError        **error);

gboolean            ukui_rr_config_get_clone    (UkuiRRConfig  *configuration);
void                ukui_rr_config_set_clone    (UkuiRRConfig  *configuration, gboolean clone);
UkuiRROutputInfo **ukui_rr_config_get_outputs  (UkuiRRConfig  *configuration);

char *ukui_rr_config_get_backup_filename (void);
char *ukui_rr_config_get_intended_filename (void);

#endif
