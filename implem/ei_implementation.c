/**
 * @file	ei_implementation.c
 *
 * @brief 	Private definitions.
 *
 */

#include "ei_implementation.h"
#include "hw_interface.h"
#include "ei_types.h"
#include "ei_widget.h"
#include "ei_utils.h"


/**
 * \brief	Converts the red, green, blue and alpha components of a color into a 32 bits integer
 * 		than can be written directly in the memory returned by \ref hw_surface_get_buffer.
 * 		The surface parameter provides the channel order.
 *
 * @param	surface		The surface where to store this pixel, provides the channels order.
 * @param	color		The color to convert.
 *
 * @return 			The 32 bit integer corresponding to the color. The alpha component
 *				of the color is ignored in the case of surfaces that don't have an
 *				alpha channel.
 */
uint32_t ei_impl_map_rgba(ei_surface_t surface, ei_color_t color)
{
	int ir, ig, ib, ia;
	uint32_t color_32 = 0;
	hw_surface_get_channel_indices(surface, &ir, &ig, &ib, &ia);

	for (int i = 0; i < 4; i++) {
		if (i == ir) {
			color_32 += color.red << 8*i;
		} else if (i == ig) {
			color_32 += color.green << 8*i;
		} else if (i == ib) {
			color_32 += color.blue << 8*i;
		} else {
			color_32 += color.alpha << 8*i;
		}
	}
	
	return color_32;
}

/**
 * \brief	Tells the placer to recompute the geometry of a widget.
 *		The widget must have been previously placed by a call to \ref ei_place.
 *		Geometry re-computation is necessary for example when the text label of
 *		a widget has changed, and thus the widget "natural" size has changed.
 *
 * @param	widget		The widget which geometry must be re-computed.
 */
void ei_impl_placer_run(ei_widget_t widget);

/**
 * @brief	Draws the children of a widget.
 * 		The children are draw withing the limits of the clipper and
 * 		the widget's content_rect.
 *
 * @param	widget		The widget which children are drawn.
 * @param	surface		A locked surface where to draw the widget's children.
 * @param	pick_surface	The picking offscreen.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle
 *				(expressed in the surface reference frame).
 */
void ei_impl_widget_draw_children      (ei_widget_t		widget,
					ei_surface_t		surface,
					ei_surface_t		pick_surface,
					ei_rect_t*		clipper);

void ei_fill_pixel(ei_surface_t surface, const ei_color_t *color, ei_point_t pixel)
{
	uint32_t *pix_ptr = (uint32_t*) hw_surface_get_buffer(surface);
	ei_size_t size = hw_surface_get_size(surface);
	int width = size.width;
	uint32_t couleur = ei_impl_map_rgba(surface, *color);
	int x = pixel.x;
	int y = pixel.y;
	pix_ptr[y * width + x] = couleur;
}

bool in_clipper(int x, int y, int xc_min, int xc_max, int yc_min, int yc_max, const ei_rect_t* clipper)
{
	if (clipper == NULL) {
		return true;
	} else {
		return x >= xc_min && x <= xc_max && y >= yc_min && y <= yc_max;
	}
}

void tc_init(ei_cote_t **tc, int yp_min, ei_point_t *point_array, size_t point_array_size)
{
	for (size_t i = 0; i < point_array_size; i++) {
		ei_point_t pt1 = point_array[i];
		ei_point_t pt2 = point_array[(i+1) % point_array_size];

		if (pt1.y != pt2.y) {
			ei_point_t pt_ymin = (pt1.y < pt2.y) ? pt1 : pt2;
			int y_min = pt_ymin.y;
			int y_max = (pt1.y > pt2.y) ? pt1.y : pt2.y;
		 	float pente = (float) (pt2.y - pt1.y) / (pt2.x - pt1.x);
			ei_cote_t *nv_cote = malloc(sizeof(ei_cote_t));
			*nv_cote = (ei_cote_t) { y_max, pt_ymin.x, 1/pente, NULL };
			int i_scan = y_min - yp_min;

			if (tc[i_scan] == NULL) {
				tc[i_scan] = nv_cote;
			} else {
				ei_cote_t *cote = tc[i_scan];
				while (cote->suiv != NULL) {
					cote = cote->suiv;
				}
				cote->suiv = nv_cote;
			}
		}
	}
}

void tca_insert(ei_cote_t **tca_ptr, ei_cote_t **tc, int i_scan)
{
	while (tc[i_scan] != NULL) {
		ei_cote_t *tc_cote = tc[i_scan];
		tc[i_scan] = tc_cote->suiv;
		ei_cote_t *tca_cote_prec = NULL;
		ei_cote_t *tca_cote = *tca_ptr;
		
		while (tca_cote != NULL && tc_cote->x_ymin >= tca_cote->x_ymin) {
			tca_cote_prec = tca_cote;
			tca_cote = tca_cote->suiv;
		}

		tc_cote->suiv = tca_cote;

		if (tca_cote_prec) {
			tca_cote_prec->suiv = tc_cote;
		} else {
			*tca_ptr = tc_cote;
		}
	}
}

void tca_remove(ei_cote_t **tca_ptr, ei_cote_t **tc, int y_scan)
{
	ei_cote_t *tca_cote_prec = NULL;
	ei_cote_t *tca_cote = *tca_ptr;

	while (tca_cote != NULL) {
		if (tca_cote->y_max == y_scan) {
			tca_cote = tca_cote->suiv;
			if (tca_cote_prec) {
				free(tca_cote_prec->suiv);
				tca_cote_prec->suiv = tca_cote;
			} else {
				free(*tca_ptr);
				*tca_ptr = tca_cote;
			}
		} else {
			tca_cote_prec = tca_cote;
			tca_cote = tca_cote->suiv;
		}
	}
}

int ei_octant_array_size(int rayon)
{
	int x = 0;
	int y = rayon;
	int m = 5 - 4*rayon;
	int tab_size = 0;

	while (x <= y) {
		tab_size++;
		if (m > 0) {
			y--;
			m -= 8*y;
		}
		x++;
		m += 8*x + 4;
	}

	return tab_size;
}

ei_point_t* ei_octant(ei_point_t centre, int rayon, int octant, int octant_array_size)
{
	int x_centre = centre.x;
	int y_centre = centre.y;
	int x = 0;
	int y = rayon;
	int m = 5 - 4*rayon;
	ei_point_t *tab = malloc(sizeof(ei_point_t[octant_array_size]));
	int sign_x, sign_y, inverse;

	switch (octant) {
		case 0:
			sign_x = 1, sign_y = -1, inverse = 0; 
			break;
		case 1:
			sign_x = 1, sign_y = -1, inverse = 1;
			break;
		case 2:
			sign_x = 1, sign_y = 1, inverse = 1;
			break;
		case 3:
			sign_x = 1, sign_y = 1, inverse = 0;
			break;
		case 4:
			sign_x = -1, sign_y = 1, inverse = 0;
			break;
		case 5:
			sign_x = -1, sign_y = 1, inverse = 1;
			break;
		case 6:
			sign_x = -1, sign_y = -1, inverse = 1;
			break;
		case 7:
			sign_x = -1, sign_y = -1, inverse = 0;
			break;
	}

	if (octant % 2 == 0) {
		for (int i = 0; i < octant_array_size; i++) {
			tab[i].x = sign_x * ((y - x) * inverse + x) + x_centre;
			tab[i].y = sign_y * ((y - x) * (1 - inverse) + x) + y_centre;
			if (m > 0) {
				y--;
				m -= 8 * y;
			}
			x++;
			m += 8 * x + 4;
		}
	} else {
		for (int i = octant_array_size - 1; i > -1; i--) {
			tab[i].x = sign_x * ((y - x) * inverse + x) + x_centre;
			tab[i].y = sign_y * ((y - x) * (1 - inverse) + x) + y_centre;
			if (m > 0) {
				y--;
				m -= 8 * y;
			}
			x++;
			m += 8 * x + 4;
		}
	}

	return tab;
}

int ei_octant_lines_array_size(int rayon)
{
	int x = 0;
	int y = rayon;
	int m = 5 - 4*rayon;
	int tab_size = 0;

	while (x <= y) {
		if (m > 0) {
			y--;
			m -= 8*y;
			tab_size++;
		}
		x++;
		m += 8*x + 4;
	}

	return tab_size;
}

ei_point_t* ei_octant_lines(ei_point_t centre, int rayon)
{
	int x_centre = centre.x;
	int y_centre = centre.y;
	int x = 0;
	int y = rayon;
	int m = 5 - 4*rayon;
	int tab_size = ei_octant_lines_array_size(rayon);
	ei_point_t *tab = malloc(sizeof(ei_point_t[tab_size]));
	int i = 0;

	while (i < tab_size) {
		if (m > 0) {
			y--;
			m -= 8 * y;
			tab[i] = ei_point(x + x_centre, y+1 + y_centre);
			i++;
		}
		x++;
		m += 8*x + 4;
	}

	return tab;
}

ei_point_t *ei_rounded_frame(ei_rect_t rect, int rayon, int part)
{
	int x0 = rect.top_left.x;
	int y0 = rect.top_left.y;
	int width = rect.size.width;
	int height = rect.size.height;
	int h = height / 2;

	ei_point_t pt0 = { x0 + width - rayon, y0 + rayon };
	ei_point_t pt1 = { x0 + width - rayon, y0 + height - rayon };
	ei_point_t pt2 = { x0 + rayon, y0 + height - rayon };
	ei_point_t pt3 = { x0 + rayon, y0 + rayon };
	ei_point_t points[4] = { pt0, pt1, pt2, pt3 };

	int octant_array_size = ei_octant_array_size(rayon);
	ei_point_t *tab;

	if (part == TOTAL) {
		tab = malloc(sizeof(ei_point_t[8 * octant_array_size]));
		for (int octant = 0; octant < 8; octant++) {
			ei_point_t *octant_array = ei_octant(points[octant / 2], rayon, octant, octant_array_size);
			for (int i = 0; i < octant_array_size; i++) {
				tab[octant * octant_array_size + i] = octant_array[i];
			}
			free(octant_array);
		}
	} else {
		tab = malloc(sizeof(ei_point_t[4 * octant_array_size + 2]));
		if (part == TOP) {
			for (int octant = 5; octant <= 8; octant++) {
				ei_point_t *octant_array = ei_octant(points[octant % 8 / 2], rayon, octant % 8, octant_array_size);
				for (int i = 0; i < octant_array_size; i++) {
					tab[(octant - 5) * octant_array_size + i] = octant_array[i];
				}
				free(octant_array);
			}
		} else {
			for (int octant = 1; octant <= 4; octant++) {
				ei_point_t *octant_array = ei_octant(points[octant / 2], rayon, octant, octant_array_size);
				for (int i = 0; i < octant_array_size; i++) {
					tab[(octant - 1) * octant_array_size + i] = octant_array[i];
				}
				free(octant_array);
			}
		}
		tab[4 * octant_array_size] = ei_point(x0 + (width-2*h)*part + h, y0 + h);
		tab[4 * octant_array_size + 1] = ei_point(x0 - (width-2*h)*(part-1) + h, y0 + h);
	}

	return tab;
}

void draw_button(ei_surface_t *surface, ei_rect_t rect, ei_color_t color, ei_rect_t *clipper)
{
	int rayon = rect.size.height / 6;

	unsigned char red = (color.red * 1.1 > 255) ? 255 : color.red * 1.1;
	unsigned char green = (color.green * 1.1 > 255) ? 255 : color.green * 1.1;
	unsigned char blue = (color.blue * 1.1 > 255) ? 255 : color.blue * 1.1;
	ei_color_t light_color = { red, green, blue, color.alpha };
	ei_color_t dark_color = { color.red / 1.1, color.green / 1.1 , color.blue / 1.1, color.alpha };

	int octant_array_size = ei_octant_array_size(rayon);

	ei_point_t *top = ei_rounded_frame(rect, rayon, TOP);
	ei_draw_polygon(surface, top, 4 * octant_array_size + 2, light_color, NULL);
	free(top);

	ei_point_t *bottom = ei_rounded_frame(rect, rayon, BOTTOM);
	ei_draw_polygon(surface, bottom, 4 * octant_array_size + 2, dark_color, NULL);
	free(bottom);

	float scale = 0.04;
	rect.top_left.x += scale * rect.size.height;
	rect.top_left.y += scale * rect.size.height;
	int width = rect.size.width - 2 * rect.size.height * scale;
	int height = rect.size.height * (1 - 2*scale);

	rect = ei_rect(rect.top_left, ei_size(width, height));
	rayon = rect.size.height / 6;
	octant_array_size = ei_octant_array_size(rayon);
	ei_point_t *button = ei_rounded_frame(rect, rayon, TOTAL);
	ei_draw_polygon(surface, button, 8 * octant_array_size, color, NULL);
	free(button);
}
