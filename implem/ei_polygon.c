/**
 * @file	ei_polygon.c
 *
 * @brief 	Polygons managment.
 *
 */

#include "ei_polygon.h"

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

void ei_tca_insert(ei_cote_t **tca_ptr, ei_cote_t **tc, int i_scan)
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

void ei_tca_remove(ei_cote_t **tca_ptr, ei_cote_t **tc, int y_scan)
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

int ei_octant_array_size(int radius)
{
	int x = 0;
	int y = radius;
	int m = 5 - 4 * radius;
	int tab_size = 0;

	while (x <= y) {
		tab_size++;

		if (m > 0) {
			y--;
			m -= 8 * y;
		}

		x++;
		m += 8 * x + 4;
	}

	return tab_size;
}

ei_point_t* ei_octant_array(ei_point_t centre, int radius, int octant, int octant_array_size)
{
	int x = 0;
	int y = radius;
	int m = 5 - 4 * radius;
	int x_c = centre.x;
	int y_c = centre.y;
	int sign_x, sign_y, inverse;
	ei_point_t *tab = malloc(sizeof(ei_point_t[octant_array_size]));

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
		default:
			break;
	}

	if (octant % 2 == 0) {
		for (int i = 0; i < octant_array_size; i++) {
			tab[i].x = sign_x * ((y - x) * inverse + x) + x_c;
			tab[i].y = sign_y * ((y - x) * (1 - inverse) + x) + y_c;

			if (m > 0) {
				y--;
				m -= 8 * y;
			}

			x++;
			m += 8 * x + 4;
		}
	} else {
		for (int i = octant_array_size - 1; i > -1; i--) {
			tab[i].x = sign_x * ((y - x) * inverse + x) + x_c;
			tab[i].y = sign_y * ((y - x) * (1 - inverse) + x) + y_c;

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

int ei_octant_lines_array_size(int radius)
{
	int x = 0;
	int y = radius;
	int m = 5 - 4 * radius;
	int tab_size = 0;

	while (x <= y) {
		if (m > 0) {
			y--;
			m -= 8 * y;
			tab_size++;
		}

		x++;
		m += 8 * x + 4;
	}

	return tab_size;
}

ei_point_t* ei_octant_lines_array(ei_point_t centre, int radius)
{
	int x = 0;
	int y = radius;
	int m = 5 - 4 * radius;
	int x_c = centre.x;
	int y_c = centre.y;
	int tab_size = ei_octant_lines_array_size(radius);
	ei_point_t *tab = malloc(sizeof(ei_point_t[tab_size]));
	int i = 0;

	while (i < tab_size) {
		if (m > 0) {
			y--;
			m -= 8 * y;
			tab[i] = ei_point(x + x_c, y + 1 + y_c);
			i++;
		}

		x++;
		m += 8 * x + 4;
	}

	return tab;
}

ei_point_t *ei_rounded_frame(ei_rect_t rect, int radius, ei_frame_part_t part)
{
	int x0 = rect.top_left.x;
	int y0 = rect.top_left.y;
	int width = rect.size.width;
	int height = rect.size.height;
	int h = height / 2;

	ei_point_t pt0 = { x0 + width - radius, y0 + radius };
	ei_point_t pt1 = { x0 + width - radius, y0 + height - radius };
	ei_point_t pt2 = { x0 + radius, y0 + height - radius };
	ei_point_t pt3 = { x0 + radius, y0 + radius };
	ei_point_t points[4] = { pt0, pt1, pt2, pt3 };

	int octant_array_size = ei_octant_array_size(radius);
	ei_point_t *tab;

	if (part == ei_frame_total) {
		tab = malloc(sizeof(ei_point_t[8 * octant_array_size]));

		for (int octant = 0; octant < 8; octant++) {
			ei_point_t *octant_array = ei_octant_array(points[octant / 2], radius, octant, octant_array_size);

			for (int i = 0; i < octant_array_size; i++) {
				tab[octant * octant_array_size + i] = octant_array[i];
			}

			free(octant_array);
		}
	} else {
		tab = malloc(sizeof(ei_point_t[4 * octant_array_size + 2]));

		if (part == ei_frame_top) {
			for (int octant = 5; octant <= 8; octant++) {
				ei_point_t *octant_array = ei_octant_array(points[octant % 8 / 2], radius, octant % 8, octant_array_size);

				for (int i = 0; i < octant_array_size; i++) {
					tab[(octant - 5) * octant_array_size + i] = octant_array[i];
				}

				free(octant_array);
			}
		} else {
			for (int octant = 1; octant <= 4; octant++) {
				ei_point_t *octant_array = ei_octant_array(points[octant / 2], radius, octant, octant_array_size);

				for (int i = 0; i < octant_array_size; i++) {
					tab[(octant - 1) * octant_array_size + i] = octant_array[i];
				}

				free(octant_array);
			}
		}

		tab[4 * octant_array_size] = ei_point(x0 + (width - 2 * h) * part + h, y0 + h);
		tab[4 * octant_array_size + 1] = ei_point(x0 - (width - 2 * h) * (part - 1) + h, y0 + h);
	}

	return tab;
}

void ei_draw_frame(ei_surface_t *surface, ei_rect_t rect, ei_color_t color, ei_relief_t relief, ei_rect_t *clipper)
{
	unsigned char light_red = (color.red * 1.3 > 255) ? 255 : color.red * 1.3;
	unsigned char light_green = (color.green * 1.3 > 255) ? 255 : color.green * 1.3;
	unsigned char light_blue = (color.blue * 1.3 > 255) ? 255 : color.blue * 1.3;
	ei_color_t light_color = { light_red, light_green, light_blue, color.alpha };
	ei_color_t dark_color = { color.red / 1.3, color.green / 1.3 , color.blue / 1.3, color.alpha };

	ei_color_t top_color, bottom_color;

	switch (relief) {
		case ei_relief_none:
			top_color = color;
			bottom_color = color;
			break;
		case ei_relief_raised:
			top_color = light_color;
			bottom_color = dark_color;
			break;
		case ei_relief_sunken:
			top_color = dark_color;
			bottom_color = light_color;
			break;
		default:
			break;
	}

	ei_point_t top_right = rect.top_left;
	ei_point_t bottom_left = rect.top_left;
	ei_point_t bottom_right = rect.top_left;
	top_right.x += rect.size.width;
	bottom_right.x += rect.size.width;
	bottom_right.y += rect.size.height;
	bottom_left.y += rect.size.height;

	ei_point_t top[3] = { rect.top_left, top_right, bottom_left };
	ei_draw_polygon(surface, top, 3, top_color, clipper);

	ei_point_t bottom[3] = { bottom_right, top_right, bottom_left };
	ei_draw_polygon(surface, bottom, 3, bottom_color, clipper);

	float scale = 0.04;
	float scale_width = scale * rect.size.width;
	float scale_height = (float) rect.size.height / rect.size.width * scale_width;
	rect.top_left.x += scale_width;
	rect.top_left.y += scale_height;
	bottom_left.x += scale_width;
	bottom_left.y -= scale_height;
	bottom_right.x -= scale_width;
	bottom_right.y -= scale_height;
	top_right.x -= scale_width;
	top_right.y += scale_height;

	ei_point_t button[4] = { rect.top_left, top_right, bottom_right, bottom_left };
	ei_draw_polygon(surface, button, 4, color, clipper);
}

void ei_draw_button(ei_surface_t *surface, ei_rect_t rect, ei_color_t color, int border_width, int radius, ei_relief_t relief, ei_string_t *text, ei_font_t *text_font, ei_color_t *text_color, ei_anchor_t *text_anchor, ei_rect_t *clipper)
{
	unsigned char light_red = (color.red * 1.3 > 255) ? 255 : color.red * 1.3;
	unsigned char light_green = (color.green * 1.3 > 255) ? 255 : color.green * 1.3;
	unsigned char light_blue = (color.blue * 1.3 > 255) ? 255 : color.blue * 1.3;
	ei_color_t light_color = { light_red, light_green, light_blue, color.alpha };
	ei_color_t dark_color = { color.red / 1.3, color.green / 1.3 , color.blue / 1.3, color.alpha };

	ei_color_t top_color, bottom_color;

	switch (relief) {
		case ei_relief_none:
			top_color = color;
			bottom_color = color;
			break;
		case ei_relief_raised:
			top_color = light_color;
			bottom_color = dark_color;
			break;
		case ei_relief_sunken:
			top_color = dark_color;
			bottom_color = light_color;
			break;
		default:
			break;
	}

	int octant_array_size = ei_octant_array_size(radius);

	ei_point_t *top = ei_rounded_frame(rect, radius, ei_frame_top);
	ei_draw_polygon(surface, top, 4 * octant_array_size + 2, top_color, clipper);
	free(top);

	ei_point_t *bottom = ei_rounded_frame(rect, radius, ei_frame_bottom);
	ei_draw_polygon(surface, bottom, 4 * octant_array_size + 2, bottom_color, clipper);
	free(bottom);

	rect.top_left.x += border_width;
	rect.top_left.y += border_width;
	rect.size.width -= 2 * border_width;
	rect.size.height -= 2 * border_width;

	octant_array_size = ei_octant_array_size(radius);
	ei_point_t *button = ei_rounded_frame(rect, radius, ei_frame_total);
	ei_draw_polygon(surface, button, 8 * octant_array_size, color, clipper);

	if (text && *text) {
		ei_rect_t text_clipper = rect;

		ei_surface_t text_surface = hw_text_create_surface(*text, *text_font, *text_color);
		ei_rect_t text_rect = hw_surface_get_rect(text_surface);

		text_rect.top_left = ei_anchor_text_img(text_anchor, &text_rect, &text_clipper);

		if (clipper) {
			text_clipper = ei_rect_intersect(ei_rect_intersect(rect, *clipper), *clipper);
		}

		ei_draw_text(surface, &text_rect.top_left, (ei_const_string_t) *text, *text_font, *text_color, &text_clipper);

		hw_surface_free(text_surface);
	}

	free(button);
}
