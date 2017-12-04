/* ukui-rr-output-info.c
 * -*- c-basic-offset: 4 -*-
 *
 * Copyright 2010 Giovanni Campagna
 *
 * This file is part of the Ukui Desktop Library.
 *
 * The Ukui Desktop Library is free software; you can redistribute it and/or
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
 * License along with the Ukui Desktop Library; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#define UKUI_DESKTOP_USE_UNSTABLE_API

#include <config.h>

#include "ukui-rr-config.h"

#include "edid.h"
#include "ukui-rr-private.h"

G_DEFINE_TYPE (UkuiRROutputInfo, ukui_rr_output_info, G_TYPE_OBJECT)

static void
ukui_rr_output_info_init (UkuiRROutputInfo *self)
{
    self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, UKUI_TYPE_RR_OUTPUT_INFO, UkuiRROutputInfoPrivate);

    self->priv->name = NULL;
    self->priv->on = FALSE;
    self->priv->display_name = NULL;
}

static void
ukui_rr_output_info_finalize (GObject *gobject)
{
    UkuiRROutputInfo *self = UKUI_RR_OUTPUT_INFO (gobject);

    g_free (self->priv->name);
    g_free (self->priv->display_name);

    G_OBJECT_CLASS (ukui_rr_output_info_parent_class)->finalize (gobject);
}

static void
ukui_rr_output_info_class_init (UkuiRROutputInfoClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);

    g_type_class_add_private (klass, sizeof (UkuiRROutputInfoPrivate));

    gobject_class->finalize = ukui_rr_output_info_finalize;
}

/**
 * ukui_rr_output_info_get_name:
 *
 * Returns: (transfer none): the output name
 */
char *ukui_rr_output_info_get_name (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), NULL);

    return self->priv->name;
}

/**
 * ukui_rr_output_info_is_active:
 *
 * Returns: whether there is a CRTC assigned to this output (i.e. a signal is being sent to it)
 */
gboolean ukui_rr_output_info_is_active (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), FALSE);

    return self->priv->on;
}

void ukui_rr_output_info_set_active (UkuiRROutputInfo *self, gboolean active)
{
    g_return_if_fail (UKUI_IS_RR_OUTPUT_INFO (self));

    self->priv->on = active;
}

/**
 * ukui_rr_output_info_get_geometry:
 * @self: a #UkuiRROutputInfo
 * @x: (out) (allow-none):
 * @y: (out) (allow-none):
 * @width: (out) (allow-none):
 * @height: (out) (allow-none):
 */
void ukui_rr_output_info_get_geometry (UkuiRROutputInfo *self, int *x, int *y, int *width, int *height)
{
    g_return_if_fail (UKUI_IS_RR_OUTPUT_INFO (self));

    if (x)
	*x = self->priv->x;
    if (y)
	*y = self->priv->y;
    if (width)
	*width = self->priv->width;
    if (height)
	*height = self->priv->height;
}

/**
 * ukui_rr_output_info_set_geometry:
 * @self: a #UkuiRROutputInfo
 * @x: x offset for monitor
 * @y: y offset for monitor
 * @width: monitor width
 * @height: monitor height
 *
 * Set the geometry for the monitor connected to the specified output.
 */
void ukui_rr_output_info_set_geometry (UkuiRROutputInfo *self, int  x, int  y, int  width, int  height)
{
    g_return_if_fail (UKUI_IS_RR_OUTPUT_INFO (self));

    self->priv->x = x;
    self->priv->y = y;
    self->priv->width = width;
    self->priv->height = height;
}

int ukui_rr_output_info_get_refresh_rate (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), 0);

    return self->priv->rate;
}

void ukui_rr_output_info_set_refresh_rate (UkuiRROutputInfo *self, int rate)
{
    g_return_if_fail (UKUI_IS_RR_OUTPUT_INFO (self));

    self->priv->rate = rate;
}

UkuiRRRotation ukui_rr_output_info_get_rotation (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), UKUI_RR_ROTATION_0);

    return self->priv->rotation;
}

void ukui_rr_output_info_set_rotation (UkuiRROutputInfo *self, UkuiRRRotation rotation)
{
    g_return_if_fail (UKUI_IS_RR_OUTPUT_INFO (self));

    self->priv->rotation = rotation;
}

/**
 * ukui_rr_output_info_is_connected:
 *
 * Returns: whether the output is physically connected to a monitor
 */
gboolean ukui_rr_output_info_is_connected (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), FALSE);

    return self->priv->connected;
}

/**
 * ukui_rr_output_info_get_vendor:
 * @self: a #UkuiRROutputInfo
 * @vendor: (out caller-allocates) (array fixed-size=4):
 */
void ukui_rr_output_info_get_vendor (UkuiRROutputInfo *self, gchar* vendor)
{
    g_return_if_fail (UKUI_IS_RR_OUTPUT_INFO (self));
    g_return_if_fail (vendor != NULL);

    vendor[0] = self->priv->vendor[0];
    vendor[1] = self->priv->vendor[1];
    vendor[2] = self->priv->vendor[2];
    vendor[3] = self->priv->vendor[3];
}

guint ukui_rr_output_info_get_product (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), 0);

    return self->priv->product;
}

guint ukui_rr_output_info_get_serial (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), 0);

    return self->priv->serial;
}

double ukui_rr_output_info_get_aspect_ratio (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), 0);

    return self->priv->aspect;
}

/**
 * ukui_rr_output_info_get_display_name:
 *
 * Returns: (transfer none): the display name of this output
 */
char *ukui_rr_output_info_get_display_name (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), NULL);

    return self->priv->display_name;
}

gboolean ukui_rr_output_info_get_primary (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), FALSE);

    return self->priv->primary;
}

void ukui_rr_output_info_set_primary (UkuiRROutputInfo *self, gboolean primary)
{
    g_return_if_fail (UKUI_IS_RR_OUTPUT_INFO (self));

    self->priv->primary = primary;
}

int ukui_rr_output_info_get_preferred_width (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), 0);

    return self->priv->pref_width;
}

int ukui_rr_output_info_get_preferred_height (UkuiRROutputInfo *self)
{
    g_return_val_if_fail (UKUI_IS_RR_OUTPUT_INFO (self), 0);

    return self->priv->pref_height;
}
