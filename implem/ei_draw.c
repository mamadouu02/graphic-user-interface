/**
 *  @file	ei_draw.c
 *  @brief	Graphical primitives to draw lines, polygons, text, and operation of drawing
 *		surfaces.
 *
 */

#include "ei_draw.h"
#include "ei_implementation.h"



/**
 * \brief	Draws a line that can be made of many line segments.
 *
 * @param	surface 	Where to draw the line. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	point_array 	The array of points defining the polyline. Its size is provided in the
 * 				next parameter (point_array_size). The array can be empty (i.e. nothing
 * 				is drawn) or it can have a single point, or more.
 *				If the last point is the same as the first point, then this pixel is
 *				drawn only once.
 * @param	point_array_size The number of points in the point_array. Can be 0.
 * @param	color		The color used to draw the line. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void	ei_draw_polyline	(ei_surface_t		surface,
				     ei_point_t*		point_array,
				     size_t			point_array_size,
				     ei_color_t		color,
				     const ei_rect_t*	clipper)
{
	uint32_t *premier = (uint32_t*) hw_surface_get_buffer(surface);
	int largeur = hw_surface_get_size(surface).width;
	uint32_t couleur = ei_impl_map_rgba(surface, color);

	int xc_min = (clipper) ? clipper->top_left.x : 0;
	int xc_max = (clipper) ? xc_min + clipper->size.width : 0;
	int yc_min = (clipper) ? clipper->top_left.y : 0;
	int yc_max = (clipper) ? yc_min + clipper->size.height : 0;

	for (size_t i = 0; i < point_array_size - 1; i++) {
		int x1 = point_array[i].x;
		int y1 = point_array[i].y;
		int x2 = point_array[i+1].x;
		int y2 = point_array[i+1].y;

		int delta_x = x2 - x1;
		int delta_y = y2 - y1;

		int sign_delta_x = (delta_x > 0) ? 1 : -1;
		int sign_delta_y = (delta_y > 0) ? 1 : -1;

		if (sign_delta_y == -1) {
			int tmp = x2;
			x2 = x1;
			x1 = tmp;

			tmp = y2;
			y2 = y1;
			y1 = tmp;

			sign_delta_x = -sign_delta_x;
			sign_delta_y = 1;

			delta_x = -delta_x;
			delta_y = -delta_y;
		}

		int x = x1;
		int y = y1;
		int E = 0;
		
		if (in_clipper(x, y, xc_min, xc_max, yc_min, yc_max, clipper)) {
			*(premier + y * largeur + x) = couleur;
		}

		if (sign_delta_x == 1 && delta_x > delta_y) {
			while (x != x2) {
				x++;
				E += delta_y;

				if (2*E > delta_x) {
					y++;
					E -= delta_x;
				}

				if (in_clipper(x, y, xc_min, xc_max, yc_min, yc_max, clipper)) {
					*(premier + y * largeur + x) = couleur;
				}
			}
		} else if (sign_delta_x == 1 && delta_x <= delta_y) {
			while (y != y2) {
				y++;
				E += delta_x;

				if (2*E > delta_y) {
					x++;
					E -=  delta_y;
				}

				if (in_clipper(x, y, xc_min, xc_max, yc_min, yc_max, clipper)) {
					*(premier + y * largeur + x) = couleur;
				}
			}
		} else if (sign_delta_x == -1 && sign_delta_x * delta_x > delta_y) {
			while (x != x2) {
				x--;
				E += delta_y;

				if (2*E > sign_delta_x * delta_x) {
					y++;
					E -= sign_delta_x * delta_x;
				}

				if (in_clipper(x, y, xc_min, xc_max, yc_min, yc_max, clipper)) {
					*(premier + y * largeur + x) = couleur;
				}
			}
		} else if (sign_delta_x == -1 && sign_delta_x * delta_x <= delta_y) {
			while (y != y2) {
				y++;
				E += sign_delta_x * delta_x;

				if (2*E > delta_y) {
					x--;
					E -= delta_y;
				}

				if (in_clipper(x, y, xc_min, xc_max, yc_min, yc_max, clipper)) {
					*(premier + y * largeur + x) = couleur;
				}
			}
		}
	}
}

/**
 * \brief	Draws a filled polygon.
 *
 * @param	surface 	Where to draw the polygon. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	point_array 	The array of points defining the polygon. Its size is provided in the
 * 				next parameter (point_array_size). The array can be empty (i.e. nothing
 * 				is drawn) or else it must have more than 2 points.
 * @param	point_array_size The number of points in the point_array. Must be 0 or more than 2.
 * @param	color		The color used to draw the polygon. The alpha channel is managed.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void	ei_draw_polygon		(ei_surface_t		surface,
					    ei_point_t*		point_array,
					    size_t			point_array_size,
					    ei_color_t		color,
					    const ei_rect_t*	clipper)
{
	// Initialisation de TC
	int yp_min = point_array[0].y;
	int yp_max = yp_min;

	for (size_t i = 1; i < point_array_size; i++) {
		int y = point_array[i].y;
		yp_min = (y < yp_min) ? y : yp_min;
		yp_max = (y > yp_max) ? y : yp_max;
	}

	int tc_size = yp_max - yp_min + 1;
	ei_cote_t **tc = calloc(tc_size, sizeof(ei_cote_t *));
	tc_init(tc, yp_min, point_array, point_array_size);

	// Initialisation de TCA et de la scanline
	ei_cote_t *tca = NULL;
	int y_scan = yp_min;

	while (y_scan <= yp_max) {
		int i_scan = y_scan - yp_min;
		tca_insert(&tca, tc, i_scan);
		tca_remove(&tca, tc, y_scan);

		// Modification des pixels
		ei_cote_t *tca_cote = tca;
		while (tca_cote != NULL) {
			int x_min = (int) tca_cote->x_ymin;
			int x_max = (int) tca_cote->suiv->x_ymin;
			ei_point_t pt1 = { x_min, y_scan };
			ei_point_t pt2 = { x_max, y_scan };
			ei_point_t line_point_array[2] = { pt1, pt2 };
			size_t line_point_array_size = 2;
			ei_draw_polyline(surface, line_point_array, line_point_array_size, color, clipper);
			tca_cote = tca_cote->suiv->suiv;
		}

		y_scan++;

		// Mise à jour de la TCA
		tca_cote = tca;
		while (tca_cote != NULL) {
			tca_cote->x_ymin += tca_cote->inv_pente;
			tca_cote = tca_cote->suiv;
		}
	}

	free(tc);
}

/**
 * \brief	Draws text by calling \ref hw_text_create_surface.
 *
 * @param	surface 	Where to draw the text. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	where		Coordinates, in the surface, where to anchor the top-left corner of
 *				the rendered text.
 * @param	text		The string of the text. Can't be NULL.
 * @param	font		The font used to render the text. If NULL, the \ref ei_default_font
 *				is used.
 * @param	color		The text color. Can't be NULL. The alpha parameter is not used.
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void	ei_draw_text		(ei_surface_t		surface,
					 const ei_point_t*	where,
					 ei_const_string_t	text,
					 ei_font_t		font,
					 ei_color_t		color,
					 const ei_rect_t*	clipper);

/**
 * \brief	Fills the surface with the specified color.
 *
 * @param	surface		The surface to be filled. The surface must be *locked* by
 *				\ref hw_surface_lock.
 * @param	color		The color used to fill the surface. If NULL, it means that the
 *				caller want it painted black (opaque).
 * @param	clipper		If not NULL, the drawing is restricted within this rectangle.
 */
void	ei_fill			(ei_surface_t		surface,
					    const ei_color_t*	color,
					    const ei_rect_t*	clipper)
{
	uint32_t *pix_ptr = (uint32_t*) hw_surface_get_buffer(surface);
	ei_size_t size = hw_surface_get_size(surface);

	if (clipper) {
		int xc_min = clipper->top_left.x;
		int xc_max = xc_min + clipper->size.width;
		int yc_min = clipper->top_left.y;
		int yc_max = yc_min + clipper->size.height;

		for (int i = 0, x, y; i < size.width * size.height; i++, pix_ptr++) {
			x = i % size.width;
			y = i / size.width;
			if (in_clipper(x, y, xc_min, xc_max, yc_min, yc_max, clipper)) {
				*pix_ptr = ei_impl_map_rgba(surface, *color);
			}
		}
	} else {
		for (int i = 0; i < size.width * size.height; i++, pix_ptr++) {
			*pix_ptr = ei_impl_map_rgba(surface, *color);
		}
	}
}

/**
 * \brief	Copies pixels from a source surface to a destination surface.
 *		The source and destination areas of the copy (either the entire surfaces, or
 *		subparts) must have the same size before considering clipping.
 *		Both surfaces must be *locked* by \ref hw_surface_lock.
 *
 * @param	destination	The surface on which to copy pixels.
 * @param	dst_rect	If NULL, the entire destination surface is used. If not NULL,
 *				defines the rectangle on the destination surface where to copy
 *				the pixels.
 * @param	source		The surface from which to copy pixels.
 * @param	src_rect	If NULL, the entire source surface is used. If not NULL, defines the
 *				rectangle on the source surface from which to copy the pixels.
 * @param	alpha		If true, the final pixels are a combination of source and
 *				destination pixels weighted by the source alpha channel and
 *				the transparency of the final pixels is set to opaque.
 *				If false, the final pixels are an exact copy of the source pixels,
 				including the alpha channel.
 *
 * @return			Returns 0 on success, 1 on failure (different sizes between
 * 				source and destination).
 */
int	ei_copy_surface		(ei_surface_t		destination,
					   const ei_rect_t*	dst_rect,
					   ei_surface_t		source,
					   const ei_rect_t*	src_rect,
					   bool			alpha);
