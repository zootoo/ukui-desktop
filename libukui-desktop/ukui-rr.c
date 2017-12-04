/* ukui-rr.c
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
 * Boston, MA 02110-1301, USA.
 * 
 * Author: Soren Sandmann <sandmann@redhat.com>
 */

#define UKUI_DESKTOP_USE_UNSTABLE_API

#include <config.h>
#include <glib/gi18n-lib.h>
#include <string.h>
#include <X11/Xlib.h>

#ifdef HAVE_RANDR
#include <X11/extensions/Xrandr.h>
#endif

#include <gtk/gtk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>

#undef UKUI_DISABLE_DEPRECATED
#include "ukui-rr.h"
#include "ukui-rr-config.h"

#include "private.h"
#include "ukui-rr-private.h"

#define DISPLAY(o) ((o)->info->screen->priv->xdisplay)

#ifndef HAVE_RANDR
/* This is to avoid a ton of ifdefs wherever we use a type from libXrandr */
typedef int RROutput;
typedef int RRCrtc;
typedef int RRMode;
typedef int Rotation;
#define RR_Rotate_0		1
#define RR_Rotate_90		2
#define RR_Rotate_180		4
#define RR_Rotate_270		8
#define RR_Reflect_X		16
#define RR_Reflect_Y		32
#endif

enum {
    SCREEN_PROP_0,
    SCREEN_PROP_GDK_SCREEN,
    SCREEN_PROP_LAST,
};

enum {
    SCREEN_CHANGED,
    SCREEN_SIGNAL_LAST,
};

gint screen_signals[SCREEN_SIGNAL_LAST];

struct UkuiRROutput
{
    ScreenInfo *	info;
    RROutput		id;
    
    char *		name;
    UkuiRRCrtc *	current_crtc;
    gboolean		connected;
    gulong		width_mm;
    gulong		height_mm;
    UkuiRRCrtc **	possible_crtcs;
    UkuiRROutput **	clones;
    UkuiRRMode **	modes;
    int			n_preferred;
    guint8 *		edid_data;
    int         edid_size;
    char *              connector_type;
};

struct UkuiRROutputWrap
{
    RROutput		id;
};

struct UkuiRRCrtc
{
    ScreenInfo *	info;
    RRCrtc		id;
    
    UkuiRRMode *	current_mode;
    UkuiRROutput **	current_outputs;
    UkuiRROutput **	possible_outputs;
    int			x;
    int			y;
    
    UkuiRRRotation	current_rotation;
    UkuiRRRotation	rotations;
    int			gamma_size;
};

struct UkuiRRMode
{
    ScreenInfo *	info;
    RRMode		id;
    char *		name;
    int			width;
    int			height;
    int			freq;		/* in mHz */
};

/* UkuiRRCrtc */
static UkuiRRCrtc *  crtc_new          (ScreenInfo         *info,
					 RRCrtc              id);
static UkuiRRCrtc *  crtc_copy         (const UkuiRRCrtc  *from);
static void           crtc_free         (UkuiRRCrtc        *crtc);

#ifdef HAVE_RANDR
static gboolean       crtc_initialize   (UkuiRRCrtc        *crtc,
					 XRRScreenResources *res,
					 GError            **error);
#endif

/* UkuiRROutput */
static UkuiRROutput *output_new        (ScreenInfo         *info,
					 RROutput            id);

#ifdef HAVE_RANDR
static gboolean       output_initialize (UkuiRROutput      *output,
					 XRRScreenResources *res,
					 GError            **error);
#endif

static UkuiRROutput *output_copy       (const UkuiRROutput *from);
static void           output_free       (UkuiRROutput      *output);

/* UkuiRRMode */
static UkuiRRMode *  mode_new          (ScreenInfo         *info,
					 RRMode              id);

#ifdef HAVE_RANDR
static void           mode_initialize   (UkuiRRMode        *mode,
					 XRRModeInfo        *info);
#endif

static UkuiRRMode *  mode_copy         (const UkuiRRMode  *from);
static void           mode_free         (UkuiRRMode        *mode);


static void ukui_rr_screen_finalize (GObject*);
static void ukui_rr_screen_set_property (GObject*, guint, const GValue*, GParamSpec*);
static void ukui_rr_screen_get_property (GObject*, guint, GValue*, GParamSpec*);
static gboolean ukui_rr_screen_initable_init (GInitable*, GCancellable*, GError**);
static void ukui_rr_screen_initable_iface_init (GInitableIface *iface);
G_DEFINE_TYPE_WITH_CODE (UkuiRRScreen, ukui_rr_screen, G_TYPE_OBJECT,
    G_IMPLEMENT_INTERFACE (G_TYPE_INITABLE, ukui_rr_screen_initable_iface_init))

G_DEFINE_BOXED_TYPE (UkuiRRCrtc, ukui_rr_crtc, crtc_copy, crtc_free)
G_DEFINE_BOXED_TYPE (UkuiRROutput, ukui_rr_output, output_copy, output_free)
G_DEFINE_BOXED_TYPE (UkuiRRMode, ukui_rr_mode, mode_copy, mode_free)

/* Errors */

/**
 * ukui_rr_error_quark:
 *
 * Returns the #GQuark that will be used for #GError values returned by the
 * UkuiRR API.
 *
 * Return value: a #GQuark used to identify errors coming from the UkuiRR API.
 */
GQuark
ukui_rr_error_quark (void)
{
    return g_quark_from_static_string ("ukui-rr-error-quark");
}

/* Screen */
static UkuiRROutput *
ukui_rr_output_by_id (ScreenInfo *info, RROutput id)
{
    UkuiRROutput **output;
    
    g_assert (info != NULL);
    
    for (output = info->outputs; *output; ++output)
    {
	if ((*output)->id == id)
	    return *output;
    }
    
    return NULL;
}

static UkuiRRCrtc *
crtc_by_id (ScreenInfo *info, RRCrtc id)
{
    UkuiRRCrtc **crtc;
    
    if (!info)
        return NULL;
    
    for (crtc = info->crtcs; *crtc; ++crtc)
    {
	if ((*crtc)->id == id)
	    return *crtc;
    }
    
    return NULL;
}

static UkuiRRMode *
mode_by_id (ScreenInfo *info, RRMode id)
{
    UkuiRRMode **mode;
    
    g_assert (info != NULL);
    
    for (mode = info->modes; *mode; ++mode)
    {
	if ((*mode)->id == id)
	    return *mode;
    }
    
    return NULL;
}

static void
screen_info_free (ScreenInfo *info)
{
    UkuiRROutput **output;
    UkuiRRCrtc **crtc;
    UkuiRRMode **mode;
    
    g_assert (info != NULL);

#ifdef HAVE_RANDR
    if (info->resources)
    {
	XRRFreeScreenResources (info->resources);
	
	info->resources = NULL;
    }
#endif
    
    if (info->outputs)
    {
	for (output = info->outputs; *output; ++output)
	    output_free (*output);
	g_free (info->outputs);
    }
    
    if (info->crtcs)
    {
	for (crtc = info->crtcs; *crtc; ++crtc)
	    crtc_free (*crtc);
	g_free (info->crtcs);
    }
    
    if (info->modes)
    {
	for (mode = info->modes; *mode; ++mode)
	    mode_free (*mode);
	g_free (info->modes);
    }

    if (info->clone_modes)
    {
	/* The modes themselves were freed above */
	g_free (info->clone_modes);
    }
    
    g_free (info);
}

static gboolean
has_similar_mode (UkuiRROutput *output, UkuiRRMode *mode)
{
    int i;
    UkuiRRMode **modes = ukui_rr_output_list_modes (output);
    int width = ukui_rr_mode_get_width (mode);
    int height = ukui_rr_mode_get_height (mode);

    for (i = 0; modes[i] != NULL; ++i)
    {
	UkuiRRMode *m = modes[i];

	if (ukui_rr_mode_get_width (m) == width	&&
	    ukui_rr_mode_get_height (m) == height)
	{
	    return TRUE;
	}
    }

    return FALSE;
}

static void
gather_clone_modes (ScreenInfo *info)
{
    int i;
    GPtrArray *result = g_ptr_array_new ();

    for (i = 0; info->outputs[i] != NULL; ++i)
    {
	int j;
	UkuiRROutput *output1, *output2;

	output1 = info->outputs[i];
	
	if (!output1->connected)
	    continue;
	
	for (j = 0; output1->modes[j] != NULL; ++j)
	{
	    UkuiRRMode *mode = output1->modes[j];
	    gboolean valid;
	    int k;

	    valid = TRUE;
	    for (k = 0; info->outputs[k] != NULL; ++k)
	    {
		output2 = info->outputs[k];
		
		if (!output2->connected)
		    continue;
		
		if (!has_similar_mode (output2, mode))
		{
		    valid = FALSE;
		    break;
		}
	    }

	    if (valid)
		g_ptr_array_add (result, mode);
	}
    }

    g_ptr_array_add (result, NULL);
    
    info->clone_modes = (UkuiRRMode **)g_ptr_array_free (result, FALSE);
}

#ifdef HAVE_RANDR
static gboolean
fill_screen_info_from_resources (ScreenInfo *info,
				 XRRScreenResources *resources,
				 GError **error)
{
    int i;
    GPtrArray *a;
    UkuiRRCrtc **crtc;
    UkuiRROutput **output;

    info->resources = resources;

    /* We create all the structures before initializing them, so
     * that they can refer to each other.
     */
    a = g_ptr_array_new ();
    for (i = 0; i < resources->ncrtc; ++i)
    {
	UkuiRRCrtc *crtc = crtc_new (info, resources->crtcs[i]);

	g_ptr_array_add (a, crtc);
    }
    g_ptr_array_add (a, NULL);
    info->crtcs = (UkuiRRCrtc **)g_ptr_array_free (a, FALSE);

    a = g_ptr_array_new ();
    for (i = 0; i < resources->noutput; ++i)
    {
	UkuiRROutput *output = output_new (info, resources->outputs[i]);

	g_ptr_array_add (a, output);
    }
    g_ptr_array_add (a, NULL);
    info->outputs = (UkuiRROutput **)g_ptr_array_free (a, FALSE);

    a = g_ptr_array_new ();
    for (i = 0;  i < resources->nmode; ++i)
    {
	UkuiRRMode *mode = mode_new (info, resources->modes[i].id);

	g_ptr_array_add (a, mode);
    }
    g_ptr_array_add (a, NULL);
    info->modes = (UkuiRRMode **)g_ptr_array_free (a, FALSE);

    /* Initialize */
    for (crtc = info->crtcs; *crtc; ++crtc)
    {
	if (!crtc_initialize (*crtc, resources, error))
	    return FALSE;
    }

    for (output = info->outputs; *output; ++output)
    {
	if (!output_initialize (*output, resources, error))
	    return FALSE;
    }

    for (i = 0; i < resources->nmode; ++i)
    {
	UkuiRRMode *mode = mode_by_id (info, resources->modes[i].id);

	mode_initialize (mode, &(resources->modes[i]));
    }

    gather_clone_modes (info);

    return TRUE;
}
#endif /* HAVE_RANDR */

static gboolean
fill_out_screen_info (Display *xdisplay,
		      Window xroot,
		      ScreenInfo *info,
		      gboolean needs_reprobe,
		      GError **error)
{
#ifdef HAVE_RANDR
    XRRScreenResources *resources;
    
    g_assert (xdisplay != NULL);
    g_assert (info != NULL);

    /* First update the screen resources */

    if (needs_reprobe)
        resources = XRRGetScreenResources (xdisplay, xroot);
    else
        resources = XRRGetScreenResourcesCurrent (xdisplay, xroot);

    if (resources)
    {
	if (!fill_screen_info_from_resources (info, resources, error))
	    return FALSE;
    }
    else
    {
	g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_RANDR_ERROR,
		     /* Translators: a CRTC is a CRT Controller (this is X terminology). */
		     _("could not get the screen resources (CRTCs, outputs, modes)"));
	return FALSE;
    }

    /* Then update the screen size range.  We do this after XRRGetScreenResources() so that
     * the X server will already have an updated view of the outputs.
     */

    if (needs_reprobe) {
	gboolean success;

        gdk_error_trap_push ();
	success = XRRGetScreenSizeRange (xdisplay, xroot,
					 &(info->min_width),
					 &(info->min_height),
					 &(info->max_width),
					 &(info->max_height));
	gdk_flush ();
	if (gdk_error_trap_pop ()) {
	    g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_UNKNOWN,
			 _("unhandled X error while getting the range of screen sizes"));
	    return FALSE;
	}

	if (!success) {
	    g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_RANDR_ERROR,
			 _("could not get the range of screen sizes"));
            return FALSE;
        }
    }
    else
    {
        ukui_rr_screen_get_ranges (info->screen, 
					 &(info->min_width),
					 &(info->max_width),
					 &(info->min_height),
					 &(info->max_height));
    }

    info->primary = None;
    gdk_error_trap_push ();
    info->primary = XRRGetOutputPrimary (xdisplay, xroot);
    gdk_error_trap_pop_ignored ();

    return TRUE;
#else
    return FALSE;
#endif /* HAVE_RANDR */
}

static ScreenInfo *
screen_info_new (UkuiRRScreen *screen, gboolean needs_reprobe, GError **error)
{
    ScreenInfo *info = g_new0 (ScreenInfo, 1);
    UkuiRRScreenPrivate *priv;

    g_assert (screen != NULL);

    priv = screen->priv;

    info->outputs = NULL;
    info->crtcs = NULL;
    info->modes = NULL;
    info->screen = screen;
    
    if (fill_out_screen_info (priv->xdisplay, priv->xroot, info, needs_reprobe, error))
    {
	return info;
    }
    else
    {
	screen_info_free (info);
	return NULL;
    }
}

static gboolean
screen_update (UkuiRRScreen *screen, gboolean force_callback, gboolean needs_reprobe, GError **error)
{
    ScreenInfo *info;
    gboolean changed = FALSE;
    
    g_assert (screen != NULL);

    info = screen_info_new (screen, needs_reprobe, error);
    if (!info)
	    return FALSE;

#ifdef HAVE_RANDR
    if (info->resources->configTimestamp != screen->priv->info->resources->configTimestamp)
	    changed = TRUE;
#endif

    screen_info_free (screen->priv->info);
	
    screen->priv->info = info;

    if (changed || force_callback)
	g_signal_emit (G_OBJECT (screen), screen_signals[SCREEN_CHANGED], 0);
    
    return changed;
}

static GdkFilterReturn
screen_on_event (GdkXEvent *xevent,
		 GdkEvent *event,
		 gpointer data)
{
#ifdef HAVE_RANDR
    UkuiRRScreen *screen = data;
    UkuiRRScreenPrivate *priv = screen->priv;
    XEvent *e = xevent;
    int event_num;

    if (!e)
	return GDK_FILTER_CONTINUE;

    event_num = e->type - priv->randr_event_base;

    if (event_num == RRScreenChangeNotify) {
	/* We don't reprobe the hardware; we just fetch the X server's latest
	 * state.  The server already knows the new state of the outputs; that's
	 * why it sent us an event!
	 */
        screen_update (screen, TRUE, FALSE, NULL); /* NULL-GError */
#if 0
	/* Enable this code to get a dialog showing the RANDR timestamps, for debugging purposes */
	{
	    GtkWidget *dialog;
	    XRRScreenChangeNotifyEvent *rr_event;
	    static int dialog_num;

	    rr_event = (XRRScreenChangeNotifyEvent *) e;

	    dialog = gtk_message_dialog_new (NULL,
					     0,
					     GTK_MESSAGE_INFO,
					     GTK_BUTTONS_CLOSE,
					     "RRScreenChangeNotify timestamps (%d):\n"
					     "event change: %u\n"
					     "event config: %u\n"
					     "event serial: %lu\n"
					     "----------------------"
					     "screen change: %u\n"
					     "screen config: %u\n",
					     dialog_num++,
					     (guint32) rr_event->timestamp,
					     (guint32) rr_event->config_timestamp,
					     rr_event->serial,
					     (guint32) priv->info->resources->timestamp,
					     (guint32) priv->info->resources->configTimestamp);
	    g_signal_connect (dialog, "response",
			      G_CALLBACK (gtk_widget_destroy), NULL);
	    gtk_widget_show (dialog);
	}
#endif
    }
#if 0
    /* WHY THIS CODE IS DISABLED:
     *
     * Note that in ukui_rr_screen_new(), we only select for
     * RRScreenChangeNotifyMask.  We used to select for other values in
     * RR*NotifyMask, but we weren't really doing anything useful with those
     * events.  We only care about "the screens changed in some way or another"
     * for now.
     *
     * If we ever run into a situtation that could benefit from processing more
     * detailed events, we can enable this code again.
     *
     * Note that the X server sends RRScreenChangeNotify in conjunction with the
     * more detailed events from RANDR 1.2 - see xserver/randr/randr.c:TellChanged().
     */
    else if (event_num == RRNotify)
    {
	/* Other RandR events */

	XRRNotifyEvent *event = (XRRNotifyEvent *)e;

	/* Here we can distinguish between RRNotify events supported
	 * since RandR 1.2 such as RRNotify_OutputProperty.  For now, we
	 * don't have anything special to do for particular subevent types, so
	 * we leave this as an empty switch().
	 */
	switch (event->subtype)
	{
	default:
	    break;
	}

	/* No need to reprobe hardware here */
	screen_update (screen, TRUE, FALSE, NULL); /* NULL-GError */
    }
#endif

#endif /* HAVE_RANDR */

    /* Pass the event on to GTK+ */
    return GDK_FILTER_CONTINUE;
}

static gboolean
ukui_rr_screen_initable_init (GInitable *initable, GCancellable *canc, GError **error)

{
    UkuiRRScreen *self = UKUI_RR_SCREEN (initable);
    UkuiRRScreenPrivate *priv = self->priv;
    Display *dpy = GDK_SCREEN_XDISPLAY (self->priv->gdk_screen);
    int event_base;
    int ignore;

    priv->connector_type_atom = XInternAtom (dpy, "ConnectorType", FALSE);

#ifdef HAVE_RANDR
    if (XRRQueryExtension (dpy, &event_base, &ignore))
    {
        priv->randr_event_base = event_base;

        XRRQueryVersion (dpy, &priv->rr_major_version, &priv->rr_minor_version);
        if (priv->rr_major_version < 1 || (priv->rr_major_version == 1 && priv->rr_minor_version < 3)) {
            g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_NO_RANDR_EXTENSION,
                "RANDR extension is too old (must be at least 1.3)");
            return FALSE;
        }

        priv->info = screen_info_new (self, TRUE, error);

        if (!priv->info) {
	    return FALSE;
	}

        XRRSelectInput (priv->xdisplay,
            priv->xroot,
            RRScreenChangeNotifyMask);
        gdk_x11_register_standard_event_type (gdk_screen_get_display (priv->gdk_screen),
                          event_base,
                          RRNotify + 1);
        gdk_window_add_filter (priv->gdk_root, screen_on_event, self);

        return TRUE;
    }
    else
    {
#endif /* HAVE_RANDR */
    g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_NO_RANDR_EXTENSION,
        _("RANDR extension is not present"));

    return FALSE;

#ifdef HAVE_RANDR
   }
#endif
}

void
ukui_rr_screen_initable_iface_init (GInitableIface *iface)
{
    iface->init = ukui_rr_screen_initable_init;
}

void
    ukui_rr_screen_finalize (GObject *gobject)
{
    UkuiRRScreen *screen = UKUI_RR_SCREEN (gobject);

    gdk_window_remove_filter (screen->priv->gdk_root, screen_on_event, screen);

    if (screen->priv->info)
      screen_info_free (screen->priv->info);

    G_OBJECT_CLASS (ukui_rr_screen_parent_class)->finalize (gobject);
}

void
ukui_rr_screen_set_property (GObject *gobject, guint property_id, const GValue *value, GParamSpec *property)
{
    UkuiRRScreen *self = UKUI_RR_SCREEN (gobject);
    UkuiRRScreenPrivate *priv = self->priv;

    switch (property_id)
    {
    case SCREEN_PROP_GDK_SCREEN:
        priv->gdk_screen = g_value_get_object (value);
        priv->gdk_root = gdk_screen_get_root_window (priv->gdk_screen);
        priv->xroot = GDK_WINDOW_XID (priv->gdk_root);
        priv->xdisplay = GDK_SCREEN_XDISPLAY (priv->gdk_screen);
        priv->xscreen = gdk_x11_screen_get_xscreen (priv->gdk_screen);
        return;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, property);
        return;
    }
}

void
ukui_rr_screen_get_property (GObject *gobject, guint property_id, GValue *value, GParamSpec *property)
{
    UkuiRRScreen *self = UKUI_RR_SCREEN (gobject);
    UkuiRRScreenPrivate *priv = self->priv;

    switch (property_id)
    {
    case SCREEN_PROP_GDK_SCREEN:
        g_value_set_object (value, priv->gdk_screen);
        return;
     default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID (gobject, property_id, property);
        return;
    }
}

void
ukui_rr_screen_class_init (UkuiRRScreenClass *klass)
{
    GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
    g_type_class_add_private (klass, sizeof (UkuiRRScreenPrivate));

    gobject_class->set_property = ukui_rr_screen_set_property;
    gobject_class->get_property = ukui_rr_screen_get_property;
    gobject_class->finalize = ukui_rr_screen_finalize;

    g_object_class_install_property(
        gobject_class,
        SCREEN_PROP_GDK_SCREEN,
        g_param_spec_object (
            "gdk-screen",
            "GDK Screen",
            "The GDK Screen represented by this UkuiRRScreen",
            GDK_TYPE_SCREEN,
	    G_PARAM_READWRITE | G_PARAM_CONSTRUCT_ONLY | G_PARAM_STATIC_STRINGS)
        );

    screen_signals[SCREEN_CHANGED] = g_signal_new("changed",
        G_TYPE_FROM_CLASS (gobject_class),
        G_SIGNAL_RUN_FIRST | G_SIGNAL_NO_RECURSE | G_SIGNAL_NO_HOOKS,
        G_STRUCT_OFFSET (UkuiRRScreenClass, changed),
        NULL,
        NULL,
        g_cclosure_marshal_VOID__VOID,
        G_TYPE_NONE,
        0);
}

void
ukui_rr_screen_init (UkuiRRScreen *self)
{
    UkuiRRScreenPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE (self, UKUI_TYPE_RR_SCREEN, UkuiRRScreenPrivate);
    self->priv = priv;

    priv->gdk_screen = NULL;
    priv->gdk_root = NULL;
    priv->xdisplay = NULL;
    priv->xroot = None;
    priv->xscreen = NULL;
    priv->info = NULL;
    priv->rr_major_version = 0;
    priv->rr_minor_version = 0;
}

/**
 * ukui_rr_screen_new:
 * @screen: the #GdkScreen on which to operate
 * @error: will be set if XRandR is not supported
 *
 * Creates a new #UkuiRRScreen instance
 *
 * Returns: a new #UkuiRRScreen instance or NULL if screen could not be created,
 * for instance if the driver does not support Xrandr 1.2
 */
UkuiRRScreen *
ukui_rr_screen_new (GdkScreen *screen,
                    GError **error)
{
    _ukui_desktop_init_i18n ();
    return g_initable_new (UKUI_TYPE_RR_SCREEN, NULL, error, "gdk-screen", screen, NULL);
}

void
ukui_rr_screen_set_size (UkuiRRScreen *screen,
			  int	      width,
			  int       height,
			  int       mm_width,
			  int       mm_height)
{
    g_return_if_fail (UKUI_IS_RR_SCREEN (screen));

#ifdef HAVE_RANDR
    gdk_error_trap_push ();
    XRRSetScreenSize (screen->priv->xdisplay, screen->priv->xroot,
		      width, height, mm_width, mm_height);
    gdk_error_trap_pop_ignored ();
#endif
}

/**
 * ukui_rr_screen_get_ranges:
 * @screen: a #UkuiRRScreen
 * @min_width: (out): the minimum width
 * @max_width: (out): the maximum width
 * @min_height: (out): the minimum height
 * @max_height: (out): the maximum height
 *
 * Get the ranges of the screen
 */
void
ukui_rr_screen_get_ranges (UkuiRRScreen *screen,
			    int	          *min_width,
			    int	          *max_width,
			    int           *min_height,
			    int	          *max_height)
{
    UkuiRRScreenPrivate *priv;

    g_return_if_fail (UKUI_IS_RR_SCREEN (screen));

    priv = screen->priv;
    
    if (min_width)
	*min_width = priv->info->min_width;
    
    if (max_width)
	*max_width = priv->info->max_width;
    
    if (min_height)
	*min_height = priv->info->min_height;
    
    if (max_height)
	*max_height = priv->info->max_height;
}

/**
 * ukui_rr_screen_get_timestamps:
 * @screen: a #UkuiRRScreen
 * @change_timestamp_ret: (out): Location in which to store the timestamp at which the RANDR configuration was last changed
 * @config_timestamp_ret: (out): Location in which to store the timestamp at which the RANDR configuration was last obtained
 *
 * Queries the two timestamps that the X RANDR extension maintains.  The X
 * server will prevent change requests for stale configurations, those whose
 * timestamp is not equal to that of the latest request for configuration.  The
 * X server will also prevent change requests that have an older timestamp to
 * the latest change request.
 */
void
ukui_rr_screen_get_timestamps (UkuiRRScreen *screen,
				guint32       *change_timestamp_ret,
				guint32       *config_timestamp_ret)
{
    UkuiRRScreenPrivate *priv;

    g_return_if_fail (UKUI_IS_RR_SCREEN (screen));

    priv = screen->priv;

#ifdef HAVE_RANDR
    if (change_timestamp_ret)
	*change_timestamp_ret = priv->info->resources->timestamp;

    if (config_timestamp_ret)
	*config_timestamp_ret = priv->info->resources->configTimestamp;
#endif
}

static gboolean
force_timestamp_update (UkuiRRScreen *screen)
{
#ifdef HAVE_RANDR
    UkuiRRScreenPrivate *priv = screen->priv;
    UkuiRRCrtc *crtc;
    XRRCrtcInfo *current_info;
    Status status;
    gboolean timestamp_updated;

    timestamp_updated = FALSE;

    crtc = priv->info->crtcs[0];

    if (crtc == NULL)
	goto out;

    current_info = XRRGetCrtcInfo (priv->xdisplay,
				   priv->info->resources,
				   crtc->id);

    if (current_info == NULL)
	goto out;

    gdk_error_trap_push ();
    status = XRRSetCrtcConfig (priv->xdisplay,
			       priv->info->resources,
			       crtc->id,
			       current_info->timestamp,
			       current_info->x,
			       current_info->y,
			       current_info->mode,
			       current_info->rotation,
			       current_info->outputs,
			       current_info->noutput);

    XRRFreeCrtcInfo (current_info);

    gdk_flush ();
    if (gdk_error_trap_pop ())
	goto out;

    if (status == RRSetConfigSuccess)
	timestamp_updated = TRUE;
out:
    return timestamp_updated;
#else
    return FALSE;
#endif
}

/**
 * ukui_rr_screen_refresh:
 * @screen: a #UkuiRRScreen
 * @error: location to store error, or %NULL
 *
 * Refreshes the screen configuration, and calls the screen's callback if it
 * exists and if the screen's configuration changed.
 *
 * Return value: TRUE if the screen's configuration changed; otherwise, the
 * function returns FALSE and a NULL error if the configuration didn't change,
 * or FALSE and a non-NULL error if there was an error while refreshing the
 * configuration.
 */
gboolean
ukui_rr_screen_refresh (UkuiRRScreen *screen,
			 GError       **error)
{
    gboolean refreshed;

    g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

    gdk_x11_display_grab (gdk_screen_get_display (screen->priv->gdk_screen));

    refreshed = screen_update (screen, FALSE, TRUE, error);
    force_timestamp_update (screen); /* this is to keep other clients from thinking that the X server re-detected things by itself - bgo#621046 */

    gdk_x11_display_ungrab (gdk_screen_get_display (screen->priv->gdk_screen));

    return refreshed;
}

/**
 * ukui_rr_screen_list_modes:
 *
 * List available XRandR modes
 *
 * Returns: (array zero-terminated=1) (transfer none):
 */
UkuiRRMode **
ukui_rr_screen_list_modes (UkuiRRScreen *screen)
{
    g_return_val_if_fail (UKUI_IS_RR_SCREEN (screen), NULL);
    g_return_val_if_fail (screen->priv->info != NULL, NULL);
    
    return screen->priv->info->modes;
}

/**
 * ukui_rr_screen_list_clone_modes:
 *
 * List available XRandR clone modes
 *
 * Returns: (array zero-terminated=1) (transfer none):
 */
UkuiRRMode **
ukui_rr_screen_list_clone_modes   (UkuiRRScreen *screen)
{
    g_return_val_if_fail (UKUI_IS_RR_SCREEN (screen), NULL);
    g_return_val_if_fail (screen->priv->info != NULL, NULL);

    return screen->priv->info->clone_modes;
}

/**
 * ukui_rr_screen_list_crtcs:
 *
 * List all CRTCs
 *
 * Returns: (array zero-terminated=1) (transfer none):
 */
UkuiRRCrtc **
ukui_rr_screen_list_crtcs (UkuiRRScreen *screen)
{
    g_return_val_if_fail (UKUI_IS_RR_SCREEN (screen), NULL);
    g_return_val_if_fail (screen->priv->info != NULL, NULL);
    
    return screen->priv->info->crtcs;
}

/**
 * ukui_rr_screen_list_outputs:
 *
 * List all outputs
 *
 * Returns: (array zero-terminated=1) (transfer none):
 */
UkuiRROutput **
ukui_rr_screen_list_outputs (UkuiRRScreen *screen)
{
    g_return_val_if_fail (UKUI_IS_RR_SCREEN (screen), NULL);
    g_return_val_if_fail (screen->priv->info != NULL, NULL);
    
    return screen->priv->info->outputs;
}

/**
 * ukui_rr_screen_get_crtc_by_id:
 *
 * Returns: (transfer none): the CRTC identified by @id
 */
UkuiRRCrtc *
ukui_rr_screen_get_crtc_by_id (UkuiRRScreen *screen,
				guint32        id)
{
    UkuiRRCrtc **crtcs;
    int i;
    
    g_return_val_if_fail (UKUI_IS_RR_SCREEN (screen), NULL);
    g_return_val_if_fail (screen->priv->info != NULL, NULL);

    crtcs = screen->priv->info->crtcs;
    
    for (i = 0; crtcs[i] != NULL; ++i)
    {
	if (crtcs[i]->id == id)
	    return crtcs[i];
    }
    
    return NULL;
}

/**
 * ukui_rr_screen_get_output_by_id:
 *
 * Returns: (transfer none): the output identified by @id
 */
UkuiRROutput *
ukui_rr_screen_get_output_by_id (UkuiRRScreen *screen,
				  guint32        id)
{
    UkuiRROutput **outputs;
    int i;
    
    g_return_val_if_fail (UKUI_IS_RR_SCREEN (screen), NULL);
    g_return_val_if_fail (screen->priv->info != NULL, NULL);

    outputs = screen->priv->info->outputs;

    for (i = 0; outputs[i] != NULL; ++i)
    {
	if (outputs[i]->id == id)
	    return outputs[i];
    }
    
    return NULL;
}

/* UkuiRROutput */
static UkuiRROutput *
output_new (ScreenInfo *info, RROutput id)
{
    UkuiRROutput *output = g_slice_new0 (UkuiRROutput);
    
    output->id = id;
    output->info = info;
    
    return output;
}

static guint8 *
get_property (Display *dpy,
	      RROutput output,
	      Atom atom,
	      int *len)
{
#ifdef HAVE_RANDR
    unsigned char *prop;
    int actual_format;
    unsigned long nitems, bytes_after;
    Atom actual_type;
    guint8 *result;
    
    XRRGetOutputProperty (dpy, output, atom,
			  0, 100, False, False,
			  AnyPropertyType,
			  &actual_type, &actual_format,
			  &nitems, &bytes_after, &prop);
    
    if (actual_type == XA_INTEGER && actual_format == 8)
    {
	result = g_memdup (prop, nitems);
	if (len)
	    *len = nitems;
    }
    else
    {
	result = NULL;
    }
    
    XFree (prop);
    
    return result;
#else
    return NULL;
#endif /* HAVE_RANDR */
}

static guint8 *
read_edid_data (UkuiRROutput *output, int *len)
{
    Atom edid_atom;
    guint8 *result;

    edid_atom = XInternAtom (DISPLAY (output), "EDID", FALSE);
    result = get_property (DISPLAY (output),
			   output->id, edid_atom, len);

    if (!result)
    {
	edid_atom = XInternAtom (DISPLAY (output), "EDID_DATA", FALSE);
	result = get_property (DISPLAY (output),
			       output->id, edid_atom, len);
    }

    if (result)
    {
	if (*len % 128 == 0)
	    return result;
	else
	    g_free (result);
    }
    
    return NULL;
}

static char *
get_connector_type_string (UkuiRROutput *output)
{
#ifdef HAVE_RANDR
    char *result;
    unsigned char *prop;
    int actual_format;
    unsigned long nitems, bytes_after;
    Atom actual_type;
    Atom connector_type;
    char *connector_type_str;

    result = NULL;

    if (XRRGetOutputProperty (DISPLAY (output), output->id, output->info->screen->priv->connector_type_atom,
			      0, 100, False, False,
			      AnyPropertyType,
			      &actual_type, &actual_format,
			      &nitems, &bytes_after, &prop) != Success)
	return NULL;

    if (!(actual_type == XA_ATOM && actual_format == 32 && nitems == 1))
	goto out;

    connector_type = *((Atom *) prop);

    connector_type_str = XGetAtomName (DISPLAY (output), connector_type);
    if (connector_type_str) {
	result = g_strdup (connector_type_str); /* so the caller can g_free() it */
	XFree (connector_type_str);
    }

out:

    XFree (prop);

    return result;
#else
    return NULL;
#endif
}

#ifdef HAVE_RANDR
static gboolean
output_initialize (UkuiRROutput *output, XRRScreenResources *res, GError **error)
{
    XRROutputInfo *info = XRRGetOutputInfo (
	DISPLAY (output), res, output->id);
    GPtrArray *a;
    int i;
    
#if 0
    g_print ("Output %lx Timestamp: %u\n", output->id, (guint32)info->timestamp);
#endif
    
    if (!info || !output->info)
    {
	/* FIXME: see the comment in crtc_initialize() */
	/* Translators: here, an "output" is a video output */
	g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_RANDR_ERROR,
		     _("could not get information about output %d"),
		     (int) output->id);
	return FALSE;
    }
    
    output->name = g_strdup (info->name); /* FIXME: what is nameLen used for? */
    output->current_crtc = crtc_by_id (output->info, info->crtc);
    output->width_mm = info->mm_width;
    output->height_mm = info->mm_height;
    output->connected = (info->connection == RR_Connected);
    output->connector_type = get_connector_type_string (output);

    /* Possible crtcs */
    a = g_ptr_array_new ();
    
    for (i = 0; i < info->ncrtc; ++i)
    {
	UkuiRRCrtc *crtc = crtc_by_id (output->info, info->crtcs[i]);
	
	if (crtc)
	    g_ptr_array_add (a, crtc);
    }
    g_ptr_array_add (a, NULL);
    output->possible_crtcs = (UkuiRRCrtc **)g_ptr_array_free (a, FALSE);
    
    /* Clones */
    a = g_ptr_array_new ();
    for (i = 0; i < info->nclone; ++i)
    {
	UkuiRROutput *ukui_rr_output = ukui_rr_output_by_id (output->info, info->clones[i]);
	
	if (ukui_rr_output)
	    g_ptr_array_add (a, ukui_rr_output);
    }
    g_ptr_array_add (a, NULL);
    output->clones = (UkuiRROutput **)g_ptr_array_free (a, FALSE);
    
    /* Modes */
    a = g_ptr_array_new ();
    for (i = 0; i < info->nmode; ++i)
    {
	UkuiRRMode *mode = mode_by_id (output->info, info->modes[i]);
	
	if (mode)
	    g_ptr_array_add (a, mode);
    }
    g_ptr_array_add (a, NULL);
    output->modes = (UkuiRRMode **)g_ptr_array_free (a, FALSE);
    
    output->n_preferred = info->npreferred;
    
    /* Edid data */
    output->edid_data = read_edid_data (output, &output->edid_size);
    
    XRRFreeOutputInfo (info);

    return TRUE;
}
#endif /* HAVE_RANDR */

static UkuiRROutput*
output_copy (const UkuiRROutput *from)
{
    GPtrArray *array;
    UkuiRRCrtc **p_crtc;
    UkuiRROutput **p_output;
    UkuiRRMode **p_mode;
    UkuiRROutput *output = g_slice_new0 (UkuiRROutput);

    output->id = from->id;
    output->info = from->info;
    output->name = g_strdup (from->name);
    output->current_crtc = from->current_crtc;
    output->width_mm = from->width_mm;
    output->height_mm = from->height_mm;
    output->connected = from->connected;
    output->n_preferred = from->n_preferred;
    output->connector_type = g_strdup (from->connector_type);

    array = g_ptr_array_new ();
    for (p_crtc = from->possible_crtcs; *p_crtc != NULL; p_crtc++)
    {
        g_ptr_array_add (array, *p_crtc);
    }
    output->possible_crtcs = (UkuiRRCrtc**) g_ptr_array_free (array, FALSE);

    array = g_ptr_array_new ();
    for (p_output = from->clones; *p_output != NULL; p_output++)
    {
        g_ptr_array_add (array, *p_output);
    }
    output->clones = (UkuiRROutput**) g_ptr_array_free (array, FALSE);

    array = g_ptr_array_new ();
    for (p_mode = from->modes; *p_mode != NULL; p_mode++)
    {
        g_ptr_array_add (array, *p_mode);
    }
    output->modes = (UkuiRRMode**) g_ptr_array_free (array, FALSE);

    output->edid_size = from->edid_size;
    output->edid_data = g_memdup (from->edid_data, from->edid_size);

    return output;
}

static void
output_free (UkuiRROutput *output)
{
    g_free (output->clones);
    g_free (output->modes);
    g_free (output->possible_crtcs);
    g_free (output->edid_data);
    g_free (output->name);
    g_free (output->connector_type);
    g_slice_free (UkuiRROutput, output);
}

guint32
ukui_rr_output_get_id (UkuiRROutput *output)
{
    g_assert(output != NULL);
    
    return output->id;
}

const guint8 *
ukui_rr_output_get_edid_data (UkuiRROutput *output)
{
    g_return_val_if_fail (output != NULL, NULL);
    
    return output->edid_data;
}

/**
 * ukui_rr_screen_get_output_by_name:
 *
 * Returns: (transfer none): the output identified by @name
 */
UkuiRROutput *
ukui_rr_screen_get_output_by_name (UkuiRRScreen *screen,
				    const char    *name)
{
    int i;
    
    g_return_val_if_fail (UKUI_IS_RR_SCREEN (screen), NULL);
    g_return_val_if_fail (screen->priv->info != NULL, NULL);
    
    for (i = 0; screen->priv->info->outputs[i] != NULL; ++i)
    {
	UkuiRROutput *output = screen->priv->info->outputs[i];
	
	if (strcmp (output->name, name) == 0)
	    return output;
    }
    
    return NULL;
}

/**
 * ukui_rr_output_get_crtc:
 * @output: a #UkuiRROutput
 * Returns: (transfer none):
 */
UkuiRRCrtc *
ukui_rr_output_get_crtc (UkuiRROutput *output)
{
    g_return_val_if_fail (output != NULL, NULL);
    
    return output->current_crtc;
}

/**
 * ukui_rr_output_get_possible_crtcs:
 * @output: a #UkuiRROutput
 * Returns: (array zero-terminated=1) (transfer none):
 */
UkuiRRCrtc **
ukui_rr_output_get_possible_crtcs (UkuiRROutput *output)
{
    g_return_val_if_fail (output != NULL, NULL);

    return output->possible_crtcs;
}

/* Returns NULL if the ConnectorType property is not available */
const char *
ukui_rr_output_get_connector_type (UkuiRROutput *output)
{
    g_return_val_if_fail (output != NULL, NULL);

    return output->connector_type;
}

gboolean
_ukui_rr_output_name_is_laptop (const char *name)
{
    if (!name)
        return FALSE;

    if (strstr (name, "lvds") || /* Most drivers use an "LVDS" prefix... */
        strstr (name, "LVDS") ||
        strstr (name, "Lvds") ||
        strstr (name, "LCD")  || /* ... but fglrx uses "LCD" in some versions.  Shoot me now, kthxbye. */
        strstr (name, "eDP"))    /* eDP is for internal laptop panel connections */
        return TRUE;

    return FALSE;
}

gboolean
ukui_rr_output_is_laptop (UkuiRROutput *output)
{
    g_return_val_if_fail (output != NULL, FALSE);

    if (!output->connected)
        return FALSE;

    if (g_strcmp0 (output->connector_type, UKUI_RR_CONNECTOR_TYPE_PANEL) == 0)
        return TRUE;

    /* Fallback (see https://bugs.freedesktop.org/show_bug.cgi?id=26736) */
    return _ukui_rr_output_name_is_laptop (output->name);
}

/**
 * ukui_rr_output_get_current_mode:
 * @output: a #UkuiRROutput
 * Returns: (transfer none): the current mode of this output
 */
UkuiRRMode *
ukui_rr_output_get_current_mode (UkuiRROutput *output)
{
    UkuiRRCrtc *crtc;
    
    g_return_val_if_fail (output != NULL, NULL);
    
    if ((crtc = ukui_rr_output_get_crtc (output)))
	return ukui_rr_crtc_get_current_mode (crtc);
    
    return NULL;
}

/**
 * ukui_rr_output_get_position:
 * @output: a #UkuiRROutput
 * @x: (out) (allow-none):
 * @y: (out) (allow-none):
 */
void
ukui_rr_output_get_position (UkuiRROutput   *output,
			      int             *x,
			      int             *y)
{
    UkuiRRCrtc *crtc;
    
    g_return_if_fail (output != NULL);
    
    if ((crtc = ukui_rr_output_get_crtc (output)))
	ukui_rr_crtc_get_position (crtc, x, y);
}

const char *
ukui_rr_output_get_name (UkuiRROutput *output)
{
    g_assert (output != NULL);
    return output->name;
}

int
ukui_rr_output_get_width_mm (UkuiRROutput *output)
{
    g_assert (output != NULL);
    return output->width_mm;
}

int
ukui_rr_output_get_height_mm (UkuiRROutput *output)
{
    g_assert (output != NULL);
    return output->height_mm;
}

/**
 * ukui_rr_output_get_preferred_mode:
 * @output: a #UkuiRROutput
 * Returns: (transfer none):
 */
UkuiRRMode *
ukui_rr_output_get_preferred_mode (UkuiRROutput *output)
{
    g_return_val_if_fail (output != NULL, NULL);
    if (output->n_preferred)
	return output->modes[0];
    
    return NULL;
}

/**
 * ukui_rr_output_list_modes:
 * @output: a #UkuiRROutput
 * Returns: (array zero-terminated=1) (transfer none):
 */

UkuiRRMode **
ukui_rr_output_list_modes (UkuiRROutput *output)
{
    g_return_val_if_fail (output != NULL, NULL);
    return output->modes;
}

gboolean
ukui_rr_output_is_connected (UkuiRROutput *output)
{
    g_return_val_if_fail (output != NULL, FALSE);
    return output->connected;
}

gboolean
ukui_rr_output_supports_mode (UkuiRROutput *output,
			       UkuiRRMode   *mode)
{
    int i;
    
    g_return_val_if_fail (output != NULL, FALSE);
    g_return_val_if_fail (mode != NULL, FALSE);
    
    for (i = 0; output->modes[i] != NULL; ++i)
    {
	if (output->modes[i] == mode)
	    return TRUE;
    }
    
    return FALSE;
}

gboolean
ukui_rr_output_can_clone (UkuiRROutput *output,
			   UkuiRROutput *clone)
{
    int i;
    
    g_return_val_if_fail (output != NULL, FALSE);
    g_return_val_if_fail (clone != NULL, FALSE);
    
    for (i = 0; output->clones[i] != NULL; ++i)
    {
	if (output->clones[i] == clone)
	    return TRUE;
    }
    
    return FALSE;
}

gboolean
ukui_rr_output_get_is_primary (UkuiRROutput *output)
{
#ifdef HAVE_RANDR
    return output->info->primary == output->id;
#else
    return FALSE;
#endif
}

void
ukui_rr_screen_set_primary_output (UkuiRRScreen *screen,
                                    UkuiRROutput *output)
{
#ifdef HAVE_RANDR
    UkuiRRScreenPrivate *priv;

    g_return_if_fail (UKUI_IS_RR_SCREEN (screen));

    priv = screen->priv;

    RROutput id;

    if (output)
        id = output->id;
    else
        id = None;

    XRRSetOutputPrimary (priv->xdisplay, priv->xroot, id);
#endif
}

/* UkuiRRCrtc */
typedef struct
{
    Rotation xrot;
    UkuiRRRotation rot;
} RotationMap;

static const RotationMap rotation_map[] =
{
    { RR_Rotate_0, UKUI_RR_ROTATION_0 },
    { RR_Rotate_90, UKUI_RR_ROTATION_90 },
    { RR_Rotate_180, UKUI_RR_ROTATION_180 },
    { RR_Rotate_270, UKUI_RR_ROTATION_270 },
    { RR_Reflect_X, UKUI_RR_REFLECT_X },
    { RR_Reflect_Y, UKUI_RR_REFLECT_Y },
};

static UkuiRRRotation
ukui_rr_rotation_from_xrotation (Rotation r)
{
    int i;
    UkuiRRRotation result = 0;
    
    for (i = 0; i < G_N_ELEMENTS (rotation_map); ++i)
    {
	if (r & rotation_map[i].xrot)
	    result |= rotation_map[i].rot;
    }
    
    return result;
}

static Rotation
xrotation_from_rotation (UkuiRRRotation r)
{
    int i;
    Rotation result = 0;
    
    for (i = 0; i < G_N_ELEMENTS (rotation_map); ++i)
    {
	if (r & rotation_map[i].rot)
	    result |= rotation_map[i].xrot;
    }
    
    return result;
}

#ifndef UKUI_DISABLE_DEPRECATED_SOURCE
gboolean
ukui_rr_crtc_set_config (UkuiRRCrtc      *crtc,
			  int               x,
			  int               y,
			  UkuiRRMode      *mode,
			  UkuiRRRotation   rotation,
			  UkuiRROutput   **outputs,
			  int               n_outputs,
			  GError          **error)
{
    return ukui_rr_crtc_set_config_with_time (crtc, GDK_CURRENT_TIME, x, y, mode, rotation, outputs, n_outputs, error);
}
#endif

gboolean
ukui_rr_crtc_set_config_with_time (UkuiRRCrtc      *crtc,
				    guint32           timestamp,
				    int               x,
				    int               y,
				    UkuiRRMode      *mode,
				    UkuiRRRotation   rotation,
				    UkuiRROutput   **outputs,
				    int               n_outputs,
				    GError          **error)
{
#ifdef HAVE_RANDR
    ScreenInfo *info;
    GArray *output_ids;
    Status status;
    gboolean result;
    int i;
    
    g_return_val_if_fail (crtc != NULL, FALSE);
    g_return_val_if_fail (mode != NULL || outputs == NULL || n_outputs == 0, FALSE);
    g_return_val_if_fail (error == NULL || *error == NULL, FALSE);
    
    info = crtc->info;
    
    if (mode)
    {
	if (x + mode->width > info->max_width
	    || y + mode->height > info->max_height)
	{
	    g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_BOUNDS_ERROR,
			 /* Translators: the "position", "size", and "maximum"
			  * words here are not keywords; please translate them
			  * as usual.  A CRTC is a CRT Controller (this is X terminology) */
			 _("requested position/size for CRTC %d is outside the allowed limit: "
			   "position=(%d, %d), size=(%d, %d), maximum=(%d, %d)"),
			 (int) crtc->id,
			 x, y,
			 mode->width, mode->height,
			 info->max_width, info->max_height);
	    return FALSE;
	}
    }
    
    output_ids = g_array_new (FALSE, FALSE, sizeof (RROutput));
    
    if (outputs)
    {
	for (i = 0; i < n_outputs; ++i)
	    g_array_append_val (output_ids, outputs[i]->id);
    }

    gdk_error_trap_push ();
    status = XRRSetCrtcConfig (DISPLAY (crtc), info->resources, crtc->id,
			       timestamp, 
			       x, y,
			       mode ? mode->id : None,
			       xrotation_from_rotation (rotation),
			       (RROutput *)output_ids->data,
			       output_ids->len);
    
    g_array_free (output_ids, TRUE);

    if (gdk_error_trap_pop () || status != RRSetConfigSuccess) {
        /* Translators: CRTC is a CRT Controller (this is X terminology).
         * It is *very* unlikely that you'll ever get this error, so it is
         * only listed for completeness. */
        g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_RANDR_ERROR,
                     _("could not set the configuration for CRTC %d"),
                     (int) crtc->id);
        return FALSE;
    } else {
        result = TRUE;
    }
    
    return result;
#else
    return FALSE;
#endif /* HAVE_RANDR */
}

/**
 * ukui_rr_crtc_get_current_mode:
 * @crtc: a #UkuiRRCrtc
 * Returns: (transfer none): the current mode of this crtc
 */
UkuiRRMode *
ukui_rr_crtc_get_current_mode (UkuiRRCrtc *crtc)
{
    g_return_val_if_fail (crtc != NULL, NULL);
    
    return crtc->current_mode;
}

guint32
ukui_rr_crtc_get_id (UkuiRRCrtc *crtc)
{
    g_return_val_if_fail (crtc != NULL, 0);
    
    return crtc->id;
}

gboolean
ukui_rr_crtc_can_drive_output (UkuiRRCrtc   *crtc,
				UkuiRROutput *output)
{
    int i;
    
    g_return_val_if_fail (crtc != NULL, FALSE);
    g_return_val_if_fail (output != NULL, FALSE);
    
    for (i = 0; crtc->possible_outputs[i] != NULL; ++i)
    {
	if (crtc->possible_outputs[i] == output)
	    return TRUE;
    }
    
    return FALSE;
}

/* FIXME: merge with get_mode()? */

/**
 * ukui_rr_crtc_get_position:
 * @crtc: a #UkuiRRCrtc
 * @x: (out) (allow-none):
 * @y: (out) (allow-none):
 */
void
ukui_rr_crtc_get_position (UkuiRRCrtc *crtc,
			    int         *x,
			    int         *y)
{
    g_return_if_fail (crtc != NULL);
    
    if (x)
	*x = crtc->x;
    
    if (y)
	*y = crtc->y;
}

/* FIXME: merge with get_mode()? */
UkuiRRRotation
ukui_rr_crtc_get_current_rotation (UkuiRRCrtc *crtc)
{
    g_assert(crtc != NULL);
    return crtc->current_rotation;
}

UkuiRRRotation
ukui_rr_crtc_get_rotations (UkuiRRCrtc *crtc)
{
    g_assert(crtc != NULL);
    return crtc->rotations;
}

gboolean
ukui_rr_crtc_supports_rotation (UkuiRRCrtc *   crtc,
				 UkuiRRRotation rotation)
{
    g_return_val_if_fail (crtc != NULL, FALSE);
    return (crtc->rotations & rotation);
}

static UkuiRRCrtc *
crtc_new (ScreenInfo *info, RROutput id)
{
    UkuiRRCrtc *crtc = g_slice_new0 (UkuiRRCrtc);
    
    crtc->id = id;
    crtc->info = info;
    
    return crtc;
}

static UkuiRRCrtc *
crtc_copy (const UkuiRRCrtc *from)
{
    UkuiRROutput **p_output;
    GPtrArray *array;
    UkuiRRCrtc *to = g_slice_new0 (UkuiRRCrtc);

    to->info = from->info;
    to->id = from->id;
    to->current_mode = from->current_mode;
    to->x = from->x;
    to->y = from->y;
    to->current_rotation = from->current_rotation;
    to->rotations = from->rotations;
    to->gamma_size = from->gamma_size;

    array = g_ptr_array_new ();
    for (p_output = from->current_outputs; *p_output != NULL; p_output++)
    {
        g_ptr_array_add (array, *p_output);
    }
    to->current_outputs = (UkuiRROutput**) g_ptr_array_free (array, FALSE);

    array = g_ptr_array_new ();
    for (p_output = from->possible_outputs; *p_output != NULL; p_output++)
    {
        g_ptr_array_add (array, *p_output);
    }
    to->possible_outputs = (UkuiRROutput**) g_ptr_array_free (array, FALSE);

    return to;
}

#ifdef HAVE_RANDR
static gboolean
crtc_initialize (UkuiRRCrtc        *crtc,
		 XRRScreenResources *res,
		 GError            **error)
{
    XRRCrtcInfo *info = XRRGetCrtcInfo (DISPLAY (crtc), res, crtc->id);
    GPtrArray *a;
    int i;
    
#if 0
    g_print ("CRTC %lx Timestamp: %u\n", crtc->id, (guint32)info->timestamp);
#endif
    
    if (!info)
    {
	/* FIXME: We need to reaquire the screen resources */
	/* FIXME: can we actually catch BadRRCrtc, and does it make sense to emit that? */

	/* Translators: CRTC is a CRT Controller (this is X terminology).
	 * It is *very* unlikely that you'll ever get this error, so it is
	 * only listed for completeness. */
	g_set_error (error, UKUI_RR_ERROR, UKUI_RR_ERROR_RANDR_ERROR,
		     _("could not get information about CRTC %d"),
		     (int) crtc->id);
	return FALSE;
    }
    
    /* UkuiRRMode */
    crtc->current_mode = mode_by_id (crtc->info, info->mode);
    
    crtc->x = info->x;
    crtc->y = info->y;
    
    /* Current outputs */
    a = g_ptr_array_new ();
    for (i = 0; i < info->noutput; ++i)
    {
	UkuiRROutput *output = ukui_rr_output_by_id (crtc->info, info->outputs[i]);
	
	if (output)
	    g_ptr_array_add (a, output);
    }
    g_ptr_array_add (a, NULL);
    crtc->current_outputs = (UkuiRROutput **)g_ptr_array_free (a, FALSE);
    
    /* Possible outputs */
    a = g_ptr_array_new ();
    for (i = 0; i < info->npossible; ++i)
    {
	UkuiRROutput *output = ukui_rr_output_by_id (crtc->info, info->possible[i]);
	
	if (output)
	    g_ptr_array_add (a, output);
    }
    g_ptr_array_add (a, NULL);
    crtc->possible_outputs = (UkuiRROutput **)g_ptr_array_free (a, FALSE);
    
    /* Rotations */
    crtc->current_rotation = ukui_rr_rotation_from_xrotation (info->rotation);
    crtc->rotations = ukui_rr_rotation_from_xrotation (info->rotations);
    
    XRRFreeCrtcInfo (info);

    /* get an store gamma size */
    crtc->gamma_size = XRRGetCrtcGammaSize (DISPLAY (crtc), crtc->id);

    return TRUE;
}
#endif

static void
crtc_free (UkuiRRCrtc *crtc)
{
    g_free (crtc->current_outputs);
    g_free (crtc->possible_outputs);
    g_slice_free (UkuiRRCrtc, crtc);
}

/* UkuiRRMode */
static UkuiRRMode *
mode_new (ScreenInfo *info, RRMode id)
{
    UkuiRRMode *mode = g_slice_new0 (UkuiRRMode);
    
    mode->id = id;
    mode->info = info;
    
    return mode;
}

guint32
ukui_rr_mode_get_id (UkuiRRMode *mode)
{
    g_return_val_if_fail (mode != NULL, 0);
    return mode->id;
}

guint
ukui_rr_mode_get_width (UkuiRRMode *mode)
{
    g_return_val_if_fail (mode != NULL, 0);
    return mode->width;
}

int
ukui_rr_mode_get_freq (UkuiRRMode *mode)
{
    g_return_val_if_fail (mode != NULL, 0);
    return (mode->freq) / 1000;
}

guint
ukui_rr_mode_get_height (UkuiRRMode *mode)
{
    g_return_val_if_fail (mode != NULL, 0);
    return mode->height;
}

#ifdef HAVE_RANDR
static void
mode_initialize (UkuiRRMode *mode, XRRModeInfo *info)
{
    g_assert (mode != NULL);
    g_assert (info != NULL);
    
    mode->name = g_strdup (info->name);
    mode->width = info->width;
    mode->height = info->height;
    mode->freq = ((info->dotClock / (double)info->hTotal) / info->vTotal + 0.5) * 1000;
}
#endif /* HAVE_RANDR */

static UkuiRRMode *
mode_copy (const UkuiRRMode *from)
{
    UkuiRRMode *to = g_slice_new0 (UkuiRRMode);

    to->id = from->id;
    to->info = from->info;
    to->name = g_strdup (from->name);
    to->width = from->width;
    to->height = from->height;
    to->freq = from->freq;

    return to;
}

static void
mode_free (UkuiRRMode *mode)
{
    g_free (mode->name);
    g_slice_free (UkuiRRMode, mode);
}

void
ukui_rr_crtc_set_gamma (UkuiRRCrtc *crtc, int size,
			 unsigned short *red,
			 unsigned short *green,
			 unsigned short *blue)
{
#ifdef HAVE_RANDR
    int copy_size;
    XRRCrtcGamma *gamma;

    g_return_if_fail (crtc != NULL);
    g_return_if_fail (red != NULL);
    g_return_if_fail (green != NULL);
    g_return_if_fail (blue != NULL);

    if (size != crtc->gamma_size)
	return;

    gamma = XRRAllocGamma (crtc->gamma_size);

    copy_size = crtc->gamma_size * sizeof (unsigned short);
    memcpy (gamma->red, red, copy_size);
    memcpy (gamma->green, green, copy_size);
    memcpy (gamma->blue, blue, copy_size);

    XRRSetCrtcGamma (DISPLAY (crtc), crtc->id, gamma);
    XRRFreeGamma (gamma);
#endif /* HAVE_RANDR */
}

/**
 * ukui_rr_crtc_get_gamma:
 * @crtc: a #UkuiRRCrtc
 * @size:
 * @red: (out): the minimum width
 * @green: (out): the maximum width
 * @blue: (out): the minimum height
 *
 * Returns: %TRUE for success
 */
gboolean
ukui_rr_crtc_get_gamma (UkuiRRCrtc *crtc, int *size,
			 unsigned short **red, unsigned short **green,
			 unsigned short **blue)
{
#ifdef HAVE_RANDR
    int copy_size;
    unsigned short *r, *g, *b;
    XRRCrtcGamma *gamma;

    g_return_val_if_fail (crtc != NULL, FALSE);

    gamma = XRRGetCrtcGamma (DISPLAY (crtc), crtc->id);
    if (!gamma)
	return FALSE;

    copy_size = crtc->gamma_size * sizeof (unsigned short);

    if (red) {
	r = g_new0 (unsigned short, crtc->gamma_size);
	memcpy (r, gamma->red, copy_size);
	*red = r;
    }

    if (green) {
	g = g_new0 (unsigned short, crtc->gamma_size);
	memcpy (g, gamma->green, copy_size);
	*green = g;
    }

    if (blue) {
	b = g_new0 (unsigned short, crtc->gamma_size);
	memcpy (b, gamma->blue, copy_size);
	*blue = b;
    }

    XRRFreeGamma (gamma);

    if (size)
	*size = crtc->gamma_size;

    return TRUE;
#else
    return FALSE;
#endif /* HAVE_RANDR */
}

