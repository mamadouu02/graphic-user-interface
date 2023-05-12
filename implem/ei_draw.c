/**
 *  @file	ei_draw.c
 *  @brief	Graphical primitives to draw lines, polygons, text, and operation of drawing
 *		surfaces.
 *
 */

#include "ei_draw.h"
#include "ei_implementation.h"



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

void	ei_draw_text		(ei_surface_t		surface,
					 const ei_point_t*	where,
					 ei_const_string_t	text,
					 ei_font_t		font,
					 ei_color_t		color,
					 const ei_rect_t*	clipper)
{
	ei_surface_t surface_cop = hw_text_create_surface(text, font, color);
	ei_surface_t surface_copy = hw_create_window(hw_surface_get_size(surface_cop),false);
	ei_rect_t rect_copy = hw_surface_get_rect(surface_cop);

	int height_text_copy = rect_copy.size.height;
	int width_text_copy = rect_copy.size.width;
	ei_point_t top_left_text = rect_copy.top_left;

	int height_text = 0;
	int width_text = 0;

	if (clipper != NULL) {
//		int height_clipper = clipper->size.height;
//		int width_clipper = clipper->size.width;
		ei_point_t top_left_clipper = clipper->top_left;

		if (top_left_text.x >= 5) {
			height_text = (top_left_text.y + height_text_copy < top_left_clipper.y) ? \
                                                top_left_text.y + height_text_copy : top_left_clipper.y;
			height_text -= top_left_text.y;
			width_text = (top_left_text.x + width_text_copy < top_left_clipper.x) ? \
                                                top_left_text.x + width_text_copy : top_left_clipper.x;
			width_text -= top_left_text.x;
		}
	} else {
		height_text = height_text_copy;
		width_text = width_text_copy;
	}

	ei_rect_t rect_text = ei_rect(*where, ei_size(width_text, height_text));

	ei_copy_surface(surface_copy, &rect_text, surface_cop, &rect_copy, true);
	hw_surface_free(surface_cop);
}

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

int	ei_copy_surface		(ei_surface_t		destination,
					   const ei_rect_t*	dst_rect,
					   ei_surface_t		source,
					   const ei_rect_t*	src_rect,
					   bool			alpha)
{
	uint8_t *pix_ptr_src = hw_surface_get_buffer(source);
	uint8_t *pix_ptr_dst = hw_surface_get_buffer(destination);
	ei_size_t size_surface_dst = hw_surface_get_size(destination);
	ei_size_t size_surface_src = hw_surface_get_size(source);
	ei_rect_t rect_src, rect_dst;

	if (dst_rect == NULL && src_rect == NULL) {
		rect_src = ei_rect(ei_point(0, 0), size_surface_src);
		rect_dst = ei_rect(ei_point(0, 0), size_surface_dst);
	} else if (dst_rect == NULL && src_rect != NULL) {
		rect_src = *src_rect;
		rect_dst = ei_rect(ei_point(0, 0), size_surface_dst);
	} else if (dst_rect != NULL && src_rect == NULL) {
		rect_src = ei_rect(ei_point(0, 0), size_surface_src);
		rect_dst = *dst_rect;
	} else {
		rect_src = *src_rect;
		rect_dst = *dst_rect;
	}

	ei_rect_t inter = rect_intersection(rect_src, rect_dst);
	int height_rect = inter.size.height;
	int width_rect = inter.size.width;
	int top_left_x_rect = inter.top_left.x;
	int top_left_y_rect = inter.top_left.y;

	if (size_surface_src.width == size_surface_dst.width && size_surface_src.height == size_surface_dst.height) {
		for (int y = 0; y < height_rect; y++) {
			for (int x = 0; x < width_rect + 1; x++) {
				if (alpha) {
					int ir, ig, ib, ia;
					hw_surface_get_channel_indices(destination, &ir, &ig, &ib, &ia);
					ia = (ia == -1) ? 6 - ir - ig - ib : ia;
					uint8_t pa = pix_ptr_src[4 * (y * size_surface_src.width + (top_left_x_rect + top_left_y_rect * size_surface_src.width) + x) + ia];
					uint8_t pr = pix_ptr_src[4 * (y * size_surface_src.width + (top_left_x_rect + top_left_y_rect * size_surface_src.width) + x) + ir];
					uint8_t pg = pix_ptr_src[4 * (y * size_surface_src.width + (top_left_x_rect + top_left_y_rect * size_surface_src.width) + x) + ig];
					uint8_t pb = pix_ptr_src[4 * (y * size_surface_src.width + (top_left_x_rect + top_left_y_rect * size_surface_src.width) + x) + ib];
					uint8_t sr = pix_ptr_dst[4 * (y * size_surface_dst.width + (top_left_x_rect + top_left_y_rect * size_surface_dst.width) + x) + ir];
					uint8_t sg = pix_ptr_dst[4 * (y * size_surface_dst.width + (top_left_x_rect + top_left_y_rect * size_surface_dst.width) + x) + ig];
					uint8_t sb = pix_ptr_dst[4 * (y * size_surface_dst.width + (top_left_x_rect + top_left_y_rect * size_surface_dst.width) + x) + ib];
					pix_ptr_dst[4 * (y * size_surface_dst.width + (top_left_x_rect + top_left_y_rect * size_surface_dst.width) + x) + ir] = (pa * pr + (255 - pa) * sr)/255;
					pix_ptr_dst[4 * (y * size_surface_dst.width + (top_left_x_rect + top_left_y_rect * size_surface_dst.width) + x) + ig] = (pa * pg + (255 - pa) * sg)/255;
					pix_ptr_dst[4 * (y * size_surface_dst.width + (top_left_x_rect + top_left_y_rect * size_surface_dst.width) + x) + ib] = (pa * pb + (255 - pa) * sb)/255;
				} else {
					for (int i = 0; i < 4; i++) {
						pix_ptr_dst[4 * (y * size_surface_dst.width + (top_left_x_rect + top_left_y_rect * size_surface_dst.width) + x) + i] = pix_ptr_src[4 * (y * size_surface_src.width + (top_left_x_rect + top_left_y_rect * size_surface_src.width) + x) + i];
					}
				}
			}
		}
		return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}
}
