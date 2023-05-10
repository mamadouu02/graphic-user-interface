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
