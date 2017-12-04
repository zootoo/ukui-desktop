/* ukui-rr-labeler.h - Utility to label monitors to identify them
 * while they are being configured.
 *
 * Copyright 2008, Novell, Inc.
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
 * Author: Federico Mena-Quintero <federico@novell.com>
 */

#ifndef UKUI_RR_LABELER_H
#define UKUI_RR_LABELER_H

#ifndef UKUI_DESKTOP_USE_UNSTABLE_API
#error    UkuiRR is unstable API. You must define UKUI_DESKTOP_USE_UNSTABLE_API before including ukuirr.h
#endif

#include "ukui-rr-config.h"

#define UKUI_TYPE_RR_LABELER            (ukui_rr_labeler_get_type ())
#define UKUI_RR_LABELER(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), UKUI_TYPE_RR_LABELER, UkuiRRLabeler))
#define UKUI_RR_LABELER_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass),  UKUI_TYPE_RR_LABELER, UkuiRRLabelerClass))
#define UKUI_IS_RR_LABELER(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UKUI_TYPE_RR_LABELER))
#define UKUI_IS_RR_LABELER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass),  UKUI_TYPE_RR_LABELER))
#define UKUI_RR_LABELER_GET_CLASS(obj)  (G_TYPE_INSTANCE_GET_CLASS ((obj),  UKUI_TYPE_RR_LABELER, UkuiRRLabelerClass))

typedef struct _UkuiRRLabeler UkuiRRLabeler;
typedef struct _UkuiRRLabelerClass UkuiRRLabelerClass;
typedef struct _UkuiRRLabelerPrivate UkuiRRLabelerPrivate;

struct _UkuiRRLabeler {
	GObject parent;

	/*< private >*/
	UkuiRRLabelerPrivate *priv;
};

struct _UkuiRRLabelerClass {
	GObjectClass parent_class;
};

GType ukui_rr_labeler_get_type (void);

UkuiRRLabeler *ukui_rr_labeler_new (UkuiRRConfig *config);

void ukui_rr_labeler_hide (UkuiRRLabeler *labeler);

void ukui_rr_labeler_get_rgba_for_output (UkuiRRLabeler *labeler, UkuiRROutputInfo *output, GdkRGBA *color_out);

#endif
