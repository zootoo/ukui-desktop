/* ukui-rr.h
 *
 * Copyright 2007, 2008, Red Hat, Inc.
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
#ifndef UKUI_RR_H
#define UKUI_RR_H

#ifndef UKUI_DESKTOP_USE_UNSTABLE_API
#error    UkuiRR is unstable API. You must define UKUI_DESKTOP_USE_UNSTABLE_API before including ukuirr.h
#endif

#include <glib.h>
#include <gdk/gdk.h>

typedef struct UkuiRRScreenPrivate UkuiRRScreenPrivate;
typedef struct UkuiRROutput UkuiRROutput;
typedef struct UkuiRRCrtc UkuiRRCrtc;
typedef struct UkuiRRMode UkuiRRMode;

typedef struct {
    GObject parent;

    UkuiRRScreenPrivate* priv;
} UkuiRRScreen;

typedef struct {
    GObjectClass parent_class;

        void (* changed) (void);
} UkuiRRScreenClass;

typedef enum
{
    UKUI_RR_ROTATION_0 =	(1 << 0),
    UKUI_RR_ROTATION_90 =	(1 << 1),
    UKUI_RR_ROTATION_180 =	(1 << 2),
    UKUI_RR_ROTATION_270 =	(1 << 3),
    UKUI_RR_REFLECT_X =	(1 << 4),
    UKUI_RR_REFLECT_Y =	(1 << 5)
} UkuiRRRotation;

/* Error codes */

#define UKUI_RR_ERROR (ukui_rr_error_quark ())

GQuark ukui_rr_error_quark (void);

typedef enum {
    UKUI_RR_ERROR_UNKNOWN,		/* generic "fail" */
    UKUI_RR_ERROR_NO_RANDR_EXTENSION,	/* RANDR extension is not present */
    UKUI_RR_ERROR_RANDR_ERROR,		/* generic/undescribed error from the underlying XRR API */
    UKUI_RR_ERROR_BOUNDS_ERROR,	/* requested bounds of a CRTC are outside the maximum size */
    UKUI_RR_ERROR_CRTC_ASSIGNMENT,	/* could not assign CRTCs to outputs */
    UKUI_RR_ERROR_NO_MATCHING_CONFIG,	/* none of the saved configurations matched the current configuration */
} UkuiRRError;

#define UKUI_RR_CONNECTOR_TYPE_PANEL "Panel"  /* This is a laptop's built-in LCD */

#define UKUI_TYPE_RR_SCREEN                  (ukui_rr_screen_get_type())
#define UKUI_RR_SCREEN(obj)                  (G_TYPE_CHECK_INSTANCE_CAST ((obj), UKUI_TYPE_RR_SCREEN, UkuiRRScreen))
#define UKUI_IS_RR_SCREEN(obj)               (G_TYPE_CHECK_INSTANCE_TYPE ((obj), UKUI_TYPE_RR_SCREEN))
#define UKUI_RR_SCREEN_CLASS(klass)          (G_TYPE_CHECK_CLASS_CAST ((klass), UKUI_TYPE_RR_SCREEN, UkuiRRScreenClass))
#define UKUI_IS_RR_SCREEN_CLASS(klass)       (G_TYPE_CHECK_CLASS_TYPE ((klass), UKUI_TYPE_RR_SCREEN))
#define UKUI_RR_SCREEN_GET_CLASS(obj)        (G_TYPE_INSTANCE_GET_CLASS ((obj), UKUI_TYPE_RR_SCREEN, UkuiRRScreenClass))

#define UKUI_TYPE_RR_OUTPUT (ukui_rr_output_get_type())
#define UKUI_TYPE_RR_CRTC   (ukui_rr_crtc_get_type())
#define UKUI_TYPE_RR_MODE   (ukui_rr_mode_get_type())

GType ukui_rr_screen_get_type (void);
GType ukui_rr_output_get_type (void);
GType ukui_rr_crtc_get_type (void);
GType ukui_rr_mode_get_type (void);

/* UkuiRRScreen */
UkuiRRScreen * ukui_rr_screen_new                (GdkScreen             *screen,
						    GError               **error);
UkuiRROutput **ukui_rr_screen_list_outputs       (UkuiRRScreen         *screen);
UkuiRRCrtc **  ukui_rr_screen_list_crtcs         (UkuiRRScreen         *screen);
UkuiRRMode **  ukui_rr_screen_list_modes         (UkuiRRScreen         *screen);
UkuiRRMode **  ukui_rr_screen_list_clone_modes   (UkuiRRScreen	  *screen);
void            ukui_rr_screen_set_size           (UkuiRRScreen         *screen,
						    int                    width,
						    int                    height,
						    int                    mm_width,
						    int                    mm_height);
UkuiRRCrtc *   ukui_rr_screen_get_crtc_by_id     (UkuiRRScreen         *screen,
						    guint32                id);
gboolean        ukui_rr_screen_refresh            (UkuiRRScreen         *screen,
						    GError               **error);
UkuiRROutput * ukui_rr_screen_get_output_by_id   (UkuiRRScreen         *screen,
						    guint32                id);
UkuiRROutput * ukui_rr_screen_get_output_by_name (UkuiRRScreen         *screen,
						    const char            *name);
void            ukui_rr_screen_get_ranges         (UkuiRRScreen         *screen,
						    int                   *min_width,
						    int                   *max_width,
						    int                   *min_height,
						    int                   *max_height);
void            ukui_rr_screen_get_timestamps     (UkuiRRScreen         *screen,
						    guint32               *change_timestamp_ret,
						    guint32               *config_timestamp_ret);

void            ukui_rr_screen_set_primary_output (UkuiRRScreen         *screen,
                                                    UkuiRROutput         *output);

/* UkuiRROutput */
guint32         ukui_rr_output_get_id             (UkuiRROutput         *output);
const char *    ukui_rr_output_get_name           (UkuiRROutput         *output);
gboolean        ukui_rr_output_is_connected       (UkuiRROutput         *output);
int             ukui_rr_output_get_size_inches    (UkuiRROutput         *output);
int             ukui_rr_output_get_width_mm       (UkuiRROutput         *outout);
int             ukui_rr_output_get_height_mm      (UkuiRROutput         *output);
const guint8 *  ukui_rr_output_get_edid_data      (UkuiRROutput         *output);
UkuiRRCrtc **  ukui_rr_output_get_possible_crtcs (UkuiRROutput         *output);
UkuiRRMode *   ukui_rr_output_get_current_mode   (UkuiRROutput         *output);
UkuiRRCrtc *   ukui_rr_output_get_crtc           (UkuiRROutput         *output);
const char *    ukui_rr_output_get_connector_type (UkuiRROutput         *output);
gboolean        ukui_rr_output_is_laptop          (UkuiRROutput         *output);
void            ukui_rr_output_get_position       (UkuiRROutput         *output,
						    int                   *x,
						    int                   *y);
gboolean        ukui_rr_output_can_clone          (UkuiRROutput         *output,
						    UkuiRROutput         *clone);
UkuiRRMode **  ukui_rr_output_list_modes         (UkuiRROutput         *output);
UkuiRRMode *   ukui_rr_output_get_preferred_mode (UkuiRROutput         *output);
gboolean        ukui_rr_output_supports_mode      (UkuiRROutput         *output,
						    UkuiRRMode           *mode);
gboolean        ukui_rr_output_get_is_primary     (UkuiRROutput         *output);

/* UkuiRRMode */
guint32         ukui_rr_mode_get_id               (UkuiRRMode           *mode);
guint           ukui_rr_mode_get_width            (UkuiRRMode           *mode);
guint           ukui_rr_mode_get_height           (UkuiRRMode           *mode);
int             ukui_rr_mode_get_freq             (UkuiRRMode           *mode);

/* UkuiRRCrtc */
guint32         ukui_rr_crtc_get_id               (UkuiRRCrtc           *crtc);

#ifndef UKUI_DISABLE_DEPRECATED
gboolean        ukui_rr_crtc_set_config           (UkuiRRCrtc           *crtc,
						    int                    x,
						    int                    y,
						    UkuiRRMode           *mode,
						    UkuiRRRotation        rotation,
						    UkuiRROutput        **outputs,
						    int                    n_outputs,
						    GError               **error);
#endif

gboolean        ukui_rr_crtc_set_config_with_time (UkuiRRCrtc           *crtc,
						    guint32                timestamp,
						    int                    x,
						    int                    y,
						    UkuiRRMode           *mode,
						    UkuiRRRotation        rotation,
						    UkuiRROutput        **outputs,
						    int                    n_outputs,
						    GError               **error);
gboolean        ukui_rr_crtc_can_drive_output     (UkuiRRCrtc           *crtc,
						    UkuiRROutput         *output);
UkuiRRMode *   ukui_rr_crtc_get_current_mode     (UkuiRRCrtc           *crtc);
void            ukui_rr_crtc_get_position         (UkuiRRCrtc           *crtc,
						    int                   *x,
						    int                   *y);
UkuiRRRotation ukui_rr_crtc_get_current_rotation (UkuiRRCrtc           *crtc);
UkuiRRRotation ukui_rr_crtc_get_rotations        (UkuiRRCrtc           *crtc);
gboolean        ukui_rr_crtc_supports_rotation    (UkuiRRCrtc           *crtc,
						    UkuiRRRotation        rotation);

gboolean        ukui_rr_crtc_get_gamma            (UkuiRRCrtc           *crtc,
						    int                   *size,
						    unsigned short       **red,
						    unsigned short       **green,
						    unsigned short       **blue);
void            ukui_rr_crtc_set_gamma            (UkuiRRCrtc           *crtc,
						    int                    size,
						    unsigned short        *red,
						    unsigned short        *green,
						    unsigned short        *blue);
#endif /* UKUI_RR_H */
