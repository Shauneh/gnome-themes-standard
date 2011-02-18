/* Adwaita - a GTK+ engine
 *
 * Copyright (C) 2011 Carlos Garnacho <carlosg@gnome.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Project contact: <gnome-themes-list@gnome.org>
 */

#include <gtk/gtk.h>
#include <gmodule.h>
#include <math.h>

#define ADWAITA_NAMESPACE "adwaita"

typedef struct _AdwaitaEngine AdwaitaEngine;
typedef struct _AdwaitaEngineClass AdwaitaEngineClass;

struct _AdwaitaEngine
{
  GtkThemingEngine parent_object;
};

struct _AdwaitaEngineClass
{
  GtkThemingEngineClass parent_class;
};

#define ADWAITA_TYPE_ENGINE		 (adwaita_engine_get_type ())
#define ADWAITA_ENGINE(object)		 (G_TYPE_CHECK_INSTANCE_CAST ((object), ADWAITA_TYPE_ENGINE, AdwaitaEngine))
#define ADWAITA_ENGINE_CLASS(klass)	 (G_TYPE_CHECK_CLASS_CAST ((klass), ADWAITA_TYPE_ENGINE, AdwaitaEngineClass))
#define ADWAITA_IS_ENGINE(object)	 (G_TYPE_CHECK_INSTANCE_TYPE ((object), ADWAITA_TYPE_ENGINE))
#define ADWAITA_IS_ENGINE_CLASS(klass)	 (G_TYPE_CHECK_CLASS_TYPE ((klass), ADWAITA_TYPE_ENGINE))
#define ADWAITA_ENGINE_GET_CLASS(obj)	 (G_TYPE_INSTANCE_GET_CLASS ((obj), ADWAITA_TYPE_ENGINE, AdwaitaEngineClass))

GType adwaita_engine_get_type	    (void) G_GNUC_CONST;
void  adwaita_engine_register_types (GTypeModule *module);


G_DEFINE_DYNAMIC_TYPE (AdwaitaEngine, adwaita_engine, GTK_TYPE_THEMING_ENGINE)


void
adwaita_engine_register_types (GTypeModule *module)
{
  adwaita_engine_register_type (module);
}

static void
adwaita_engine_init (AdwaitaEngine *clearlooks_engine)
{
}

static void
_cairo_round_rectangle (cairo_t *cr,
			gdouble	  x,
			gdouble	  y,
			gdouble	  w,
			gdouble	  h,
			gdouble	  radius)
{
	g_return_if_fail (cr != NULL);

	if (radius < 0.0001)
	{
		cairo_rectangle (cr, x, y, w, h);
		return;
	}

	cairo_move_to (cr, x+radius, y);
	cairo_arc (cr, x+w-radius, y+radius, radius, G_PI * 1.5, G_PI * 2);
	cairo_arc (cr, x+w-radius, y+h-radius, radius, 0, G_PI * 0.5);
	cairo_arc (cr, x+radius,   y+h-radius, radius, G_PI * 0.5, G_PI);
	cairo_arc (cr, x+radius,   y+radius,   radius, G_PI, G_PI * 1.5);
}

static void
adwaita_engine_render_arrow (GtkThemingEngine *engine,
			     cairo_t	      *cr,
			     gdouble	       angle,
			     gdouble	       x,
			     gdouble	       y,
			     gdouble	       size)
{
	double arrow_width;
	double arrow_height;
	double line_width_2;
	GtkStateFlags state;
	GdkRGBA color;

	cairo_save (cr);

	arrow_width = MIN (size * 2.0 + MAX (1.0, ceil (size * 2.0 / 6.0 * 2.0) / 2.0) / 2.0, size);
	line_width_2 = MAX (1.0, ceil (arrow_width / 6.0 * 2.0) / 2.0) / 2.0;
	arrow_height = arrow_width / 2.0 + line_width_2;

	cairo_translate (cr, x + size / 2, y + size / 2);
	cairo_rotate (cr, angle - G_PI);

	cairo_translate (cr, 0, - arrow_height / 2);

	cairo_move_to (cr, -arrow_width / 2.0, line_width_2);
	cairo_line_to (cr, -arrow_width / 2.0 + line_width_2, 0);
	/* cairo_line_to (cr, 0, arrow_height - line_width_2); */
	cairo_arc_negative (cr, 0, arrow_height - 2*line_width_2 - 2*line_width_2 * sqrt(2), 2*line_width_2, G_PI_2 + G_PI_4, G_PI_4);
	cairo_line_to (cr, arrow_width / 2.0 - line_width_2, 0);
	cairo_line_to (cr, arrow_width / 2.0, line_width_2);
	cairo_line_to (cr, 0, arrow_height);
	cairo_close_path (cr);

	state = gtk_theming_engine_get_state (engine);
	gtk_theming_engine_get_color (engine, state, &color);
	gdk_cairo_set_source_rgba (cr, &color);
	cairo_fill (cr);

	cairo_restore (cr);
}

static void
adwaita_engine_render_focus (GtkThemingEngine *engine,
			     cairo_t	      *cr,
			     gdouble	       x,
			     gdouble	       y,
			     gdouble	       width,
			     gdouble	       height)
{
	GdkRGBA *fill_color, *border_color;
	GtkStateFlags state;

	state = gtk_theming_engine_get_state (engine);
	gtk_theming_engine_get (engine, state,
				"-adwaita-focus-fill-color", &fill_color,
				"-adwaita-focus-border-color", &border_color,
				NULL);

	cairo_save (cr);
	cairo_set_line_width (cr, 1);

	_cairo_round_rectangle (cr, x + 0.5, y + 0.5, width - 1, height - 1, 3);

	gdk_cairo_set_source_rgba (cr, fill_color);
	cairo_fill_preserve (cr);

	gdk_cairo_set_source_rgba (cr, border_color);
	cairo_stroke (cr);

	cairo_restore (cr);

	gdk_rgba_free (fill_color);
	gdk_rgba_free (border_color);
}

static void
adwaita_engine_render_check (GtkThemingEngine *engine,
			     cairo_t	      *cr,
			     gdouble	       x,
			     gdouble	       y,
			     gdouble	       width,
			     gdouble	       height)
{
	GdkRGBA *bg, *border, *fg;
	gboolean inconsistent = FALSE;
	gboolean draw_bullet;
	GtkStateFlags state;
	gint radius;

	state = gtk_theming_engine_get_state (engine);
	inconsistent = (state & GTK_STATE_FLAG_INCONSISTENT) != 0;
	draw_bullet = (state & GTK_STATE_FLAG_ACTIVE);
	draw_bullet |= inconsistent;

	cairo_save (cr);

	gtk_theming_engine_get (engine, state,
				"border-radius", &radius,
				"background-color", &bg,
				"border-color", &border,
				"color", &fg,
				NULL);

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, 1);

	_cairo_round_rectangle (cr, 0.5, 0.5, width-1, height-1, 2);

	if ((state & GTK_STATE_FLAG_INSENSITIVE) == 0)
	{
		gdk_cairo_set_source_rgba (cr, bg);
		cairo_fill_preserve (cr);
	}

	gdk_cairo_set_source_rgba (cr, border);
	cairo_stroke (cr);

	if (draw_bullet)
	{
		if (inconsistent) /* Inconsistent */
		{
			cairo_set_line_width (cr, 2.0);
			cairo_move_to (cr, 3, height*0.5);
			cairo_line_to (cr, width-3, height*0.5);
		}
		else
		{
			cairo_set_line_width (cr, 1.7);
			cairo_move_to (cr, 0.5 + (width*0.2), (height*0.5));
			cairo_line_to (cr, 0.5 + (width*0.4), (height*0.7));

			cairo_curve_to (cr, 0.5 + (width*0.4), (height*0.7),
					    0.5 + (width*0.5), (height*0.4),
					    0.5 + (width*0.70), (height*0.25));

		}

		gdk_cairo_set_source_rgba (cr, fg);
		cairo_stroke (cr);
	}

	cairo_restore (cr);

	gdk_rgba_free (bg);
	gdk_rgba_free (fg);
	gdk_rgba_free (border);
}

static void
adwaita_engine_render_option (GtkThemingEngine *engine,
			      cairo_t	       *cr,
			      gdouble		x,
			      gdouble		y,
			      gdouble		width,
			      gdouble		height)
{
	GdkRGBA *bg, *border, *fg, highlight;
	GtkSymbolicColor *sym, *shade;
	gboolean inconsistent;
	gboolean draw_bullet;
	gdouble w, h, cx, cy, radius;
	GtkStateFlags state;

	w = (gdouble) width;
	h = (gdouble) height;
	cx = width / 2.0;
	cy = height / 2.0;
	radius = MIN (width, height) / 2.0;

	cairo_save (cr);

	state = gtk_theming_engine_get_state (engine);

	gtk_theming_engine_get (engine, state,
				"background-color", &bg,
				"border-color", &border,
				"color", &fg,
				NULL);

	inconsistent = (state & GTK_STATE_FLAG_INCONSISTENT) != 0;
	draw_bullet = (state & GTK_STATE_FLAG_ACTIVE) != 0;
	draw_bullet |= inconsistent;

	sym = gtk_symbolic_color_new_literal (bg);
	shade = gtk_symbolic_color_new_shade (sym, 1.1);
	gtk_symbolic_color_resolve (shade, NULL, &highlight);

	cairo_translate (cr, x, y);
	cairo_set_line_width (cr, MAX (1.0, floor (radius/6)));

	cairo_new_sub_path (cr);
	cairo_arc (cr, ceil (cx), ceil (cy), MAX (1.0, ceil (radius) - 1.5), 0, G_PI*2);

	if ((state & GTK_STATE_FLAG_INSENSITIVE) == 0)
	{
		gdk_cairo_set_source_rgba (cr, bg);
		cairo_fill_preserve (cr);
	}

	gdk_cairo_set_source_rgba (cr, border);
	cairo_stroke (cr);

	if (draw_bullet)
	{
		if (inconsistent)
		{
			gdouble line_width = floor (radius * 2 / 3);

			cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
			cairo_set_line_width (cr, line_width);

			cairo_move_to (cr, ceil (cx - radius/3.0 - line_width) + line_width, ceil (cy - line_width) + line_width);
			cairo_line_to (cr, floor (cx + radius/3.0 + line_width) - line_width, ceil (cy - line_width) + line_width);

			gdk_cairo_set_source_rgba (cr, fg);
			cairo_stroke (cr);
		}
		else
		{
			cairo_new_sub_path (cr);
			cairo_arc (cr, ceil (cx), ceil (cy), floor (radius/2.0), 0, G_PI*2);
			gdk_cairo_set_source_rgba (cr, fg);
			cairo_fill (cr);

			cairo_arc (cr, floor (cx - radius/10.0), floor (cy - radius/10.0), floor (radius/6.0), 0, G_PI*2);
			cairo_set_source_rgba (cr, highlight.red, highlight.green, highlight.blue, 0.5);
			cairo_fill (cr);
		}
	}

	cairo_restore (cr);

	gdk_rgba_free (bg);
	gdk_rgba_free (fg);
	gdk_rgba_free (border);

	gtk_symbolic_color_unref (sym);
	gtk_symbolic_color_unref (shade);
}

static void
adwaita_engine_render_extension (GtkThemingEngine *engine,
				 cairo_t	  *cr,
				 gdouble	   x,
				 gdouble	   y,
				 gdouble	   width,
				 gdouble	   height,
				 GtkPositionType   gap_side)
{
	GtkStateFlags state;

	GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_extension (engine, cr,
										  x, y, width, height,
										  gap_side);

	state = gtk_theming_engine_get_state (engine);

	if (state & GTK_STATE_FLAG_ACTIVE)
	{
		GdkRGBA *fill;

		gtk_theming_engine_get (engine, state,
					"-adwaita-selected-tab-color", &fill,
					NULL);

		switch (gap_side)
		{
		case GTK_POS_BOTTOM:
			_cairo_round_rectangle (cr, x + 1, y + 1, width - 2, 3, 0);
			break;
		case GTK_POS_TOP:
			_cairo_round_rectangle (cr, x + 1, y + height - 4, width - 2, 3, 0);
			break;
		case GTK_POS_RIGHT:
			_cairo_round_rectangle (cr, x + 1, y + 1, 3, height - 2, 0);
			break;
		case GTK_POS_LEFT:
			_cairo_round_rectangle (cr, x + width - 4, y + 1, 3, height - 2, 0);
			break;
		}

		gdk_cairo_set_source_rgba (cr, fill);
		cairo_fill (cr);

		gdk_rgba_free (fill);
	}
}

static void
adwaita_engine_render_frame (GtkThemingEngine *engine,
			     cairo_t          *cr,
			     gdouble           x,
			     gdouble           y,
			     gdouble           width,
			     gdouble           height)
{
	const GtkWidgetPath *path;
	GtkRegionFlags flags = 0;
	gint len;

	path = gtk_theming_engine_get_path (engine);
	len = gtk_widget_path_length (path);
	cairo_save (cr);

	if (gtk_widget_path_iter_has_region (path, len - 2,
					     GTK_STYLE_REGION_COLUMN_HEADER,
					     &flags))
	{
		GtkStateFlags state;
		GdkRGBA color;

		if ((flags & GTK_REGION_LAST) != 0)
			return;

		/* Column header */
		if (gtk_theming_engine_get_direction (engine) == GTK_TEXT_DIR_RTL)
		{
			cairo_move_to (cr, x + 0.5, y + 2);
			cairo_line_to (cr, x + 0.5, y + height - 4);
		}
		else
		{
			cairo_move_to (cr, x + width - 0.5, y + 2);
			cairo_line_to (cr, x + width - 0.5, y + height - 4);
		}

		state = gtk_theming_engine_get_state (engine);
		gtk_theming_engine_get_border_color (engine, state, &color);

		cairo_set_line_width (cr, 1);
		gdk_cairo_set_source_rgba (cr, &color);
		cairo_stroke (cr);
	}
	else
	{
		if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
		    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH))
		{
			/* Render GtkScale trough thinner */
			if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
			{
				y += height / 2 - 3;
				height = 6;
			}
			else
			{
				x += width / 2 - 3;
				width = 6;
			}
		}

		GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_frame (engine, cr, x, y,
										      width, height);
	}

	cairo_restore (cr);
}

static void
adwaita_engine_render_background (GtkThemingEngine *engine,
				  cairo_t          *cr,
				  gdouble           x,
				  gdouble           y,
				  gdouble           width,
				  gdouble           height)
{
	const GtkWidgetPath *path;

	path = gtk_theming_engine_get_path (engine);

	if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
	    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_TROUGH))
	{
		/* Render GtkScale trough thinner */
		if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
		{
			y += height / 2 - 3;
			height = 6;
		}
		else
		{
			x += width / 2 - 3;
			width = 6;
		}
	}

	GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_background (engine, cr, x, y,
										   width, height);
}

static void
adwaita_engine_render_expander (GtkThemingEngine *engine,
				cairo_t          *cr,
				gdouble           x,
				gdouble           y,
				gdouble           width,
				gdouble           height)
{
	GdkRGBA border, bg, fg;
	GtkStateFlags state;
	gint side;

	side = MIN (width, height);

	x += ((int) width / 2) - (side / 2);
	y += ((int) height / 2) - (side / 2);

	state = gtk_theming_engine_get_state (engine);
	gtk_theming_engine_get_border_color (engine, state, &border);
	gtk_theming_engine_get_background_color (engine, state, &bg);
	gtk_theming_engine_get_color (engine, state, &fg);

	cairo_save (cr);

	cairo_set_line_width (cr, 1);

	_cairo_round_rectangle (cr, x + 0.5, y + 0.5, side, side, 3);
	gdk_cairo_set_source_rgba (cr, &bg);
	cairo_fill_preserve (cr);

	gdk_cairo_set_source_rgba (cr, &border);
	cairo_stroke (cr);

	cairo_set_line_width (cr, 3);
	gdk_cairo_set_source_rgba (cr, &fg);

	cairo_move_to (cr, x + 2, y + side / 2 + 0.5);
	cairo_line_to (cr, x + side - 1, y + side / 2 + 0.5);

	if ((state & GTK_STATE_FLAG_ACTIVE) == 0)
	{
		cairo_move_to (cr, x + side / 2 + 0.5, y + 2);
		cairo_line_to (cr, x + side / 2 + 0.5, y + side - 1);
	}

	cairo_stroke (cr);

	cairo_restore (cr);
}

static void
adwaita_engine_render_activity (GtkThemingEngine *engine,
				cairo_t          *cr,
				gdouble           x,
				gdouble           y,
				gdouble           width,
				gdouble           height)
{
	const GtkWidgetPath *path;

	cairo_save (cr);
	path = gtk_theming_engine_get_path (engine);

	if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE) &&
	    gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PROGRESSBAR))
	{
		/* Render GtkScale fill level thinner */
		if (!gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_VERTICAL))
		{
			y += height / 2 - 3;
			height = 6;
		}
		else
		{
			x += width / 2 - 3;
			width = 6;
		}
	}

	GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_activity (engine, cr,
										 x, y, width, height);

	if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_PROGRESSBAR))
	{
		cairo_pattern_t *pattern;

		pattern = cairo_pattern_create_linear (0, 0, 20, 20);
		cairo_pattern_add_color_stop_rgba (pattern, 0, 0, 0, 0, 0);
		cairo_pattern_add_color_stop_rgba (pattern, 0.5, 0, 0, 0, 0);
		cairo_pattern_add_color_stop_rgba (pattern, 0.5, 0, 0, 0, 0.1);
		cairo_pattern_add_color_stop_rgba (pattern, 1, 0, 0, 0, 0.1);

		cairo_pattern_set_extend (pattern, CAIRO_EXTEND_REPEAT);
		cairo_rectangle (cr, x, y, width, height);
		cairo_set_source (cr, pattern);
		cairo_fill (cr);

		cairo_pattern_destroy (pattern);
	}

	cairo_restore (cr);
}

static void
adwaita_engine_render_slider (GtkThemingEngine *engine,
			      cairo_t          *cr,
			      gdouble           x,
			      gdouble           y,
			      gdouble           width,
			      gdouble           height,
			      GtkOrientation    orientation)
{
	const GtkWidgetPath *path;

	path = gtk_theming_engine_get_path (engine);
	cairo_save (cr);

	if (gtk_widget_path_is_type (path, GTK_TYPE_SCALE))
	{
		cairo_pattern_t *pattern;
		cairo_matrix_t matrix;
		GtkStateFlags state;
		GdkRGBA color;

		state = gtk_theming_engine_get_state (engine);

		if (orientation == GTK_ORIENTATION_HORIZONTAL)
		{
			cairo_move_to (cr, x + width / 2, y);
			cairo_line_to (cr, x, y + height / 2);
			cairo_line_to (cr, x, y + height);
			cairo_line_to (cr, x + width, y + height);
			cairo_line_to (cr, x + width, y + height / 2);
			cairo_line_to (cr, x + width / 2, y);
		}
		else if (gtk_theming_engine_get_direction (engine) == GTK_TEXT_DIR_RTL)
		{
			cairo_move_to (cr, x, y + height / 2);
			cairo_line_to (cr, x + width / 2, y);
			cairo_line_to (cr, x + width, y);
			cairo_line_to (cr, x + width, y + height);
			cairo_line_to (cr, x + width / 2, y + height);
			cairo_line_to (cr, x, y + height / 2);
		}
		else
		{
			cairo_move_to (cr, x + width, y + height / 2);
			cairo_line_to (cr, x + width / 2, y);
			cairo_line_to (cr, x, y);
			cairo_line_to (cr, x, y + height);
			cairo_line_to (cr, x + width / 2, y + height);
			cairo_line_to (cr, x + width, y + height / 2);
		}
		cairo_close_path (cr);

		cairo_set_line_width (cr, 1);

		gtk_theming_engine_get (engine, state,
					"background-image", &pattern,
					NULL);

		cairo_matrix_init_scale (&matrix, 1 / width, 1 / height);
		cairo_matrix_translate (&matrix, -x, -y);
		cairo_pattern_set_matrix (pattern, &matrix);

		cairo_set_source (cr, pattern);
		cairo_fill_preserve (cr);

		gtk_theming_engine_get_border_color (engine, state, &color);
		gdk_cairo_set_source_rgba (cr, &color);
		cairo_stroke (cr);

		cairo_pattern_destroy (pattern);
	}
	else
	{
		GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_slider (engine, cr,
										       x, y, width, height,
										       orientation);
	}

	cairo_restore (cr);
}

static void
adwaita_engine_render_handle (GtkThemingEngine *engine,
			      cairo_t          *cr,
			      gdouble           x,
			      gdouble           y,
			      gdouble           width,
			      gdouble           height)
{
	if (gtk_theming_engine_has_class (engine, GTK_STYLE_CLASS_GRIP))
	{
		GdkRGBA dark, highlight, bg;
		GtkSymbolicColor *sym, *shade;
		GtkJunctionSides sides;
		GtkStateFlags state;
		int lx, ly;
		int x_down;
		int y_down;
		int dots;

		state = gtk_theming_engine_get_state (engine);
		gtk_theming_engine_get_background_color (engine, state, &bg);

		sym = gtk_symbolic_color_new_literal (&bg);

		shade = gtk_symbolic_color_new_shade (sym, 0.7);
		gtk_symbolic_color_resolve (shade, NULL, &dark);
		gtk_symbolic_color_unref (shade);

		shade = gtk_symbolic_color_new_shade (sym, 1.3);
		gtk_symbolic_color_resolve (shade, NULL, &highlight);
		gtk_symbolic_color_unref (shade);

		gtk_symbolic_color_unref (sym);

		/* The number of dots fitting into the area. Just hardcoded to 4 right now. */
		/* dots = MIN (width - 2, height - 2) / 3; */
		dots = 4;

		cairo_save (cr);

		sides = gtk_theming_engine_get_junction_sides (engine);

		switch (sides)
		{
		case GTK_JUNCTION_CORNER_TOPRIGHT:
			x_down = 0;
			y_down = 0;
			cairo_translate (cr, x + width - 3*dots + 2, y + 1);
			break;
		case GTK_JUNCTION_CORNER_BOTTOMRIGHT:
			x_down = 0;
			y_down = 1;
			cairo_translate (cr, x + width - 3*dots + 2, y + height - 3*dots + 2);
			break;
		case GTK_JUNCTION_CORNER_BOTTOMLEFT:
			x_down = 1;
			y_down = 1;
			cairo_translate (cr, x + 1, y + height - 3*dots + 2);
			break;
		case GTK_JUNCTION_CORNER_TOPLEFT:
			x_down = 1;
			y_down = 0;
			cairo_translate (cr, x + 1, y + 1);
			break;
		default:
			/* Not implemented. */
			return;
		}

		for (lx = 0; lx < dots; lx++) /* horizontally */
		{
			for (ly = 0; ly <= lx; ly++) /* vertically */
			{
				int mx, my;
				mx = x_down * dots + (1 - x_down * 2) * lx - x_down;
				my = y_down * dots + (1 - y_down * 2) * ly - y_down;

				gdk_cairo_set_source_rgba (cr, &highlight);
				cairo_rectangle (cr, mx*3-1, my*3-1, 2, 2);
				cairo_fill (cr);

				gdk_cairo_set_source_rgba (cr, &dark);
				cairo_rectangle (cr, mx*3-1, my*3-1, 1, 1);
				cairo_fill (cr);
			}
		}

		cairo_restore (cr);
	}
	else
	{
		GTK_THEMING_ENGINE_CLASS (adwaita_engine_parent_class)->render_handle (engine, cr,
										       x, y, width, height);
	}
}

static void
adwaita_engine_class_init (AdwaitaEngineClass *klass)
{
	GtkThemingEngineClass *engine_class = GTK_THEMING_ENGINE_CLASS (klass);

	engine_class->render_arrow = adwaita_engine_render_arrow;
	engine_class->render_focus = adwaita_engine_render_focus;
	engine_class->render_check = adwaita_engine_render_check;
	engine_class->render_option = adwaita_engine_render_option;
	engine_class->render_extension = adwaita_engine_render_extension;
	engine_class->render_frame = adwaita_engine_render_frame;
	engine_class->render_background = adwaita_engine_render_background;
	engine_class->render_expander = adwaita_engine_render_expander;
	engine_class->render_activity = adwaita_engine_render_activity;
	engine_class->render_slider = adwaita_engine_render_slider;
	engine_class->render_handle = adwaita_engine_render_handle;

	gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
					      g_param_spec_boxed ("focus-border-color",
								  "Focus border color",
								  "Focus border color",
								  GDK_TYPE_RGBA, 0));
	gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
					      g_param_spec_boxed ("focus-fill-color",
								  "Focus fill color",
								  "Focus fill color",
								  GDK_TYPE_RGBA, 0));
	gtk_theming_engine_register_property (ADWAITA_NAMESPACE, NULL,
					      g_param_spec_boxed ("selected-tab-color",
								  "Selected tab color",
								  "Selected tab color",
								  GDK_TYPE_RGBA, 0));
}

static void
adwaita_engine_class_finalize (AdwaitaEngineClass *klass)
{
}

G_MODULE_EXPORT void
theme_init (GTypeModule *module)
{
	adwaita_engine_register_types (module);
}

G_MODULE_EXPORT void
theme_exit (void)
{
}

G_MODULE_EXPORT GtkThemingEngine *
create_engine (void)
{
	return GTK_THEMING_ENGINE (g_object_new (ADWAITA_TYPE_ENGINE,
						 "name", "adwaita",
						 NULL));
}
