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
	int tc_size;
	int y_mini = point_array[0].y;
	int y_maxi = y_mini;

	for (size_t i = 1; i < point_array_size; i++) {
		int y = point_array[i].y;
		y_mini = (y < y_mini) ? y : y_mini;
		y_maxi = (y > y_maxi) ? y : y_maxi;
	}

	tc_size = y_maxi - y_mini + 1;
	ei_cote **tc = calloc(tc_size, sizeof(ei_cote *));

	// Initialisation de tc
	for (size_t i = 0; i < point_array_size; i++) {
		ei_point_t pt1, pt2;
		if (i < point_array_size-1) {
			pt1 = point_array[i];
			pt2 = point_array[i + 1];
		}
		else{
			pt1 = point_array[point_array_size-1];
			pt2 = point_array[0];
		}
		if (pt1.y != pt2.y) {
		 	float pente = (float) (pt2.y - pt1.y) / (float) (pt2.x - pt1.x);
			int y_min = (pt1.y < pt2.y) ? pt1.y : pt2.y;
			ei_point_t pt_y_min = (pt1.y < pt2.y) ? pt1 : pt2;
			int y_max = (pt1.y < pt2.y) ? pt2.y : pt1.y;
			ei_cote *nv_cote = malloc(sizeof(ei_cote));
			*nv_cote = (ei_cote) {y_max, pt_y_min.x, 1 / pente, NULL};
			if (tc[y_min] == NULL) {
				tc[y_min] = nv_cote;
			} else {
				ei_cote *cote_courant = tc[y_min];
				while (cote_courant->ptr_cote != NULL) {
					cote_courant = cote_courant->ptr_cote;
				}
				cote_courant->ptr_cote = nv_cote;
			}
		}
	}

	int scanline = y_mini;
	ei_cote *tca = NULL;

	while (scanline < y_mini + tc_size || tca != NULL) {

		// Supprimer de TCA les cotes tels que y_max = y
		ei_cote *pp = NULL;
		ei_cote *pc = tca;
		while (pc != NULL) {
			pp = pc;
			pc = pc->ptr_cote;
			if (pc->y_max == scanline) {
				pp->ptr_cote = pc->ptr_cote;
				free(pc);
			}
		}

		// Tri de TCA par abscisse croissant des intersection de côté avec la scanline
		while (tc[scanline] != NULL) {
			ei_cote *cote_courant_tc = tc[scanline];
			tc[scanline] = cote_courant_tc->ptr_cote;
			cote_courant_tc->ptr_cote = NULL;

			ei_cote *cote_courant_tca = tca;
			bool exit = false;
			if (tca == NULL) {
				tca = cote_courant_tc;
			}
			else if (cote_courant_tca->ptr_cote==NULL){
				if (cote_courant_tca->x_ymin<cote_courant_tc->x_ymin){
					cote_courant_tca->ptr_cote=cote_courant_tc;
			}
				else{
					cote_courant_tc->ptr_cote=cote_courant_tca;
					tca=cote_courant_tc;
				}
			}
			while (cote_courant_tca->ptr_cote != NULL && !exit) {
				if (cote_courant_tca->ptr_cote->x_ymin > cote_courant_tc->x_ymin){
					exit = true;
					cote_courant_tc->ptr_cote = cote_courant_tca->ptr_cote;
					cote_courant_tca->ptr_cote = cote_courant_tc;
				} else {
					cote_courant_tca = cote_courant_tca->ptr_cote;
				}
			}
			if (!exit) {
				cote_courant_tca->ptr_cote = cote_courant_tc;
			}
		}


		// Modification des pixels de l’image sur la scanline, dans les intervalles intérieurs au polygone
		ei_cote *cote_courant_tca = tca;
		while (cote_courant_tca != NULL) {
			int xmin = cote_courant_tca->x_ymin;
			int xmax = cote_courant_tca->ptr_cote->x_ymin;
			ei_point_t point1={xmin,scanline};
			ei_point_t point2={xmax,scanline};
			ei_point_t point_array_ligne[2] = { point1, point2 };
			size_t point_array_size_ligne = 2;
			ei_draw_polyline(surface, point_array_ligne, point_array_size_ligne, color, clipper);
			cote_courant_tca=cote_courant_tca->ptr_cote->ptr_cote;
		}

		// Incrémentation de y
		scanline++;

		// Mise à jour les abscisses d’intersections des côtés de TCA avec la nouvelle scanline
		cote_courant_tca = tca;
		while (cote_courant_tca != NULL) {
			cote_courant_tca->x_ymin += (int) cote_courant_tca->inv_pente;
			cote_courant_tca = cote_courant_tca->ptr_cote;
		}
	}
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
	uint32_t *ptr_pix = (uint32_t*) hw_surface_get_buffer(surface);
	ei_size_t size = hw_surface_get_size(surface);

	if (clipper) {
		int xc_min = clipper->top_left.x;
		int xc_max = xc_min + clipper->size.width;
		int yc_min = clipper->top_left.y;
		int yc_max = yc_min + clipper->size.height;

		for (int i = 0, x, y; i < size.width * size.height; i++, ptr_pix++) {
			x = i % size.width;
			y = i / size.width;
			if (in_clipper(x, y, xc_min, xc_max, yc_min, yc_max, clipper)) {
				*ptr_pix = ei_impl_map_rgba(surface, *color);
			}
		}
	} else {
		for (int i = 0; i < size.width * size.height; i++, ptr_pix++) {
			*ptr_pix = ei_impl_map_rgba(surface, *color);
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
