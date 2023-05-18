/**
 * @file	ei_implementation.c
 *
 * @brief 	Private definitions.
 *
 */

#include "ei_implementation.h"

uint32_t id = 0;

/* Pixel */

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

/* Clipping */

bool in_clipper(int x, int y, int xc_min, int xc_max, int yc_min, int yc_max, const ei_rect_t* clipper)
{
	return clipper == NULL || (xc_min <= x && x < xc_max && yc_min <= y && y < yc_max);
}

/* Rectangle */

bool ei_rect_cmp(ei_rect_t rect1, ei_rect_t rect2)
{
	int x1 = rect1.top_left.x;
	int y1 = rect1.top_left.y;
	int width_1 = rect1.size.width;
	int height_1 = rect1.size.height;
	int x2 = rect1.top_left.x;
	int y2 = rect1.top_left.y;
	int width_2 = rect1.size.width;
	int height_2 = rect2.size.height;
	return x1 == x2 && y1 == y2 && width_1 == width_2 && height_1 == height_2;
}

ei_rect_t ei_rect_intersect(ei_rect_t rect1, ei_rect_t rect2)
{
	int x_min, x_max, width_min, width_max;

	if (rect1.top_left.x < rect2.top_left.x) {
		x_min = rect1.top_left.x;
		x_max = rect2.top_left.x;
		width_min = rect1.size.width;
		width_max = rect2.size.width;
	} else {
		x_min = rect2.top_left.x;
		x_max = rect1.top_left.x;
		width_min = rect2.size.width;
		width_max = rect1.size.width;
	}

	int y_min, y_max, height_min, height_max;

	if (rect1.top_left.y < rect2.top_left.y) {
		y_min = rect1.top_left.y;
		y_max = rect2.top_left.y;
		height_min = rect1.size.height;
		height_max = rect2.size.height;
	} else {
		y_min = rect2.top_left.y;
		y_max = rect1.top_left.y;
		height_min = rect2.size.height;
		height_max = rect1.size.height;
	}

	if (x_max <= x_min + width_min && y_max <= y_min + height_min) {
		ei_point_t point = ei_point(x_max, y_max);
		int width = width_max;
		int height = height_max;

		if (x_max + width_max > x_min + width_min) {
			width = width_min - (x_max - x_min);
		}

		if (y_max + height_max > y_min + height_min) {
			height = height_min - (y_max - y_min);
		}

		ei_size_t size = ei_size(width, height);

		return ei_rect(point, size);
	} else {
		return ei_rect(ei_point(0, 0), ei_size(0, 0));
	}
}

bool ei_in_rect(ei_point_t point, ei_rect_t rect)
{
	return rect.top_left.x <= point.x && point.x <= rect.top_left.x + rect.size.width && rect.top_left.y <= point.y && point.y <= rect.top_left.y + rect.size.height;
}

void	ei_rect_cpy	(ei_surface_t		destination,
			const ei_rect_t*	dst_rect,
			ei_surface_t		source,
			const ei_rect_t*	src_rect,
			bool			alpha)
{
	int height_src_rect = src_rect->size.height;
	int width_src_rect = src_rect->size.width;

	int height_dst_rect = dst_rect->size.height;
	int width_dst_rect = dst_rect->size.width;

	uint8_t *pix_ptr_src = hw_surface_get_buffer(source);
	uint8_t *pix_ptr_dst = hw_surface_get_buffer(destination);
	ei_size_t size_surface_dst = hw_surface_get_size(destination);
	ei_size_t size_surface_src = hw_surface_get_size(source);

	int x_src = src_rect->top_left.x;
	int y_src = src_rect->top_left.y;
	int x_dst = dst_rect->top_left.x;
	int y_dst = dst_rect->top_left.y;
	int width_src = size_surface_src.width;
	int width_dst = size_surface_dst.width;

	int min_height = height_dst_rect < height_src_rect ? height_dst_rect : height_src_rect;
	int min_width = width_dst_rect < width_src_rect ? width_dst_rect : width_src_rect;

	for (int y = 0; y < min_height; y++) {
		for (int x = 0; x < min_width + 1; x++) {
			if (alpha) {
				int ir, ig, ib, ia;
				hw_surface_get_channel_indices(destination, &ir, &ig, &ib, &ia);
				ia = (ia == -1) ? 6 - ir - ig - ib : ia;
				uint8_t pa = pix_ptr_src[4 * (y * width_src + (x_src + y_src * width_src) + x) + ia];
				uint8_t pr = pix_ptr_src[4 * (y * width_src + (x_src + y_src * width_src) + x) + ir];
				uint8_t pg = pix_ptr_src[4 * (y * width_src + (x_src + y_src * width_src) + x) + ig];
				uint8_t pb = pix_ptr_src[4 * (y * width_src + (x_src + y_src * width_src) + x) + ib];
				uint8_t sr = pix_ptr_dst[4 * (y * width_dst + (x_dst + y_dst * width_dst) + x) + ir];
				uint8_t sg = pix_ptr_dst[4 * (y * width_dst + (x_dst + y_dst * width_dst) + x) + ig];
				uint8_t sb = pix_ptr_dst[4 * (y * width_dst + (x_dst + y_dst * width_dst) + x) + ib];
				pix_ptr_dst[4 * (y * width_dst + (x_dst + y_dst * width_dst) + x) + ir] = (pa * pr + (255 - pa) * sr) / 255;
				pix_ptr_dst[4 * (y * width_dst + (x_dst + y_dst * width_dst) + x) + ig] = (pa * pg + (255 - pa) * sg) / 255;
				pix_ptr_dst[4 * (y * width_dst + (x_dst + y_dst * width_dst) + x) + ib] = (pa * pb + (255 - pa) * sb) / 255;
			} else {
				for (int i = 0; i < 4; i++) {
					pix_ptr_dst[4 * (y * width_dst + (x_dst + y_dst * width_dst) + x) + i] = pix_ptr_src[4 * (y * width_src + (x_src + y_src * width_src) + x) + i];
				}
			}
		}
	}
}

/* Picking */

void ei_widget_set_pick(ei_widget_t widget)
{
	widget->pick_id = id;

	/* Changement provisoire pour faciliter ei_widget_pick */

	// widget->pick_color.red = (id >> 16) & 0xFF;
	// widget->pick_color.green = (id >> 8) & 0xFF;
	// widget->pick_color.blue = id & 0xFF;
	// widget->pick_color.alpha = 0xFF;

	// id += 100;

	widget->pick_color.red = id;
	widget->pick_color.green = id;
	widget->pick_color.blue = id;
	widget->pick_color.alpha = 0xFF;

	id += 1;
}

void ei_pick(ei_widget_t widget, uint32_t pick_id, ei_widget_t *widget_ptr)
{
	if (widget != NULL) {
		if (widget->pick_id == pick_id) {
			*widget_ptr = widget;
		}

		ei_widget_t child = widget->children_head;
		ei_widget_t next_child;

		if (child != NULL) {
			next_child = child->next_sibling;
		}

		while (child != NULL) {
			ei_pick(child, pick_id, widget_ptr);
			child = next_child;
			next_child = (child == NULL) ? NULL : child->next_sibling;
		}
	}
}

/* Widget drawing */

void ei_impl_widget_draw_children      (ei_widget_t		widget,
					ei_surface_t		surface,
					ei_surface_t		pick_surface,
					ei_rect_t*		clipper)
{
	if (widget != NULL) {
		widget->wclass->drawfunc(widget, surface, pick_surface, clipper);
		widget = widget->next_sibling;

		while (widget != NULL) {
			widget->wclass->drawfunc(widget, surface, pick_surface, clipper);
			widget = widget->next_sibling;
		}
	}
}

/* Anchor */

ei_point_t ei_anchor_rect(ei_anchor_t *anchor_ptr, ei_rect_t *rect)
{
	ei_point_t top_left = rect->top_left;
	int width = rect->size.width;
	int height = rect->size.height;

	if (anchor_ptr) {
		ei_anchor_t anchor = *anchor_ptr;

		switch (anchor) {
			case ei_anc_center:
				top_left = ei_point_sub(rect->top_left, ei_point(width/2, height/2));
				break;
			case ei_anc_north:
				top_left = ei_point_sub(rect->top_left, ei_point(width/2, 0));
				break;
			case ei_anc_northeast:
				top_left = ei_point_sub(rect->top_left, ei_point(width, 0));
				break;
			case ei_anc_east:
				top_left = ei_point_sub(rect->top_left, ei_point(width, height/2));
				break;
			case ei_anc_southeast:
				top_left = ei_point_sub(rect->top_left, ei_point(width, height));
				break;
			case ei_anc_south:
				top_left = ei_point_sub(rect->top_left, ei_point(width/2, height));
				break;
			case ei_anc_southwest:
				top_left = ei_point_sub(rect->top_left, ei_point(0, height));
				break;
			case ei_anc_west:
				top_left = ei_point_sub(rect->top_left, ei_point(0, height/2));
				break;
			case ei_anc_northwest:
			case ei_anc_none:
				top_left = rect->top_left;
				break;
			default:
				break;
		}
	} else {
		top_left = rect->top_left;
	}

	return top_left;
}

ei_point_t ei_anchor_text_img(ei_anchor_t *anchor_ptr, ei_rect_t *rect, ei_rect_t *limit)
{
	ei_point_t top_left = rect->top_left;
	int width = rect->size.width;
	int height = rect->size.height;

	int width_limit = limit->size.width;
	int height_limit = limit->size.height;

	if (anchor_ptr) {
		ei_anchor_t anchor = *anchor_ptr;

		switch (anchor) {
			case ei_anc_north:
				top_left = ei_point((width_limit - width)/2, 0);
				break;
			case ei_anc_northeast:
				top_left = ei_point((width_limit - width), 0);
				break;
			case ei_anc_east:
				top_left = ei_point((width_limit - width), (height_limit - height)/2);
				break;
			case ei_anc_southeast:
				top_left = ei_point((width_limit - width), (height_limit - height));
				break;
			case ei_anc_south:
				top_left = ei_point((width_limit - width)/2, (height_limit - height));
				break;
			case ei_anc_southwest:
				top_left = ei_point(0, (height_limit - height));
				break;
			case ei_anc_west:
				top_left = ei_point(0, (height_limit - height)/2);
				break;
			case ei_anc_northwest:
				top_left = ei_point_zero();
				break;
			case ei_anc_center:
			case ei_anc_none:
			default:
				top_left = ei_point((width_limit - width)/2, (height_limit - height)/2);
				break;
		}
	} else {
		top_left = ei_point((width_limit - width)/2 , (height_limit - height)/2);
	}

	return ei_point(top_left.x + limit->top_left.x, top_left.y+ limit->top_left.y);
}

/* Geometry managment */

void ei_impl_placer_run(ei_widget_t widget)
{
	int parent_height = widget->parent->content_rect->size.height;
	int parent_width = widget->parent->content_rect->size.width;

	int x = widget->placer_params->x;
	int y = widget->placer_params->y;
	int width = widget->placer_params->width;
	int height = widget->placer_params->height;
	float rel_x = widget->placer_params->rel_x;
	float rel_y = widget->placer_params->rel_y;
	float *rel_width = widget->placer_params->rel_width;
	float *rel_height = widget->placer_params->rel_height;

	int height_widget = (rel_height == NULL) ? height : (*rel_height * parent_height);
	int width_widget = (rel_width == NULL) ? width : (*rel_width * parent_width);

	ei_point_t *where = malloc(sizeof(ei_point_t));
	where->x = rel_x * parent_width + x;
	where->y = rel_y * parent_height + y;

	ei_rect_t widget_rectangle = ei_rect(*where, ei_size(width_widget, height_widget));

	if (widget->placer_params == NULL) {
		widget->placer_params = malloc(sizeof(struct ei_impl_placer_params_t));
	}

	widget->screen_location = widget_rectangle;
	widget->screen_location.top_left = ei_anchor_rect(&widget->placer_params->anchor, &widget->screen_location);

	free(where);
}

void ei_impl_app_run_siblings(ei_widget_t widget)
{
	if (widget != NULL) {
		if (widget->placer_params) {
			ei_impl_placer_run(widget);
		}

		widget = widget->next_sibling;

		while (widget != NULL) {
			if (widget->placer_params) {
				ei_impl_placer_run(widget);
			}

			widget = widget->next_sibling;
		}
	}
}

void ei_impl_app_run_children(ei_widget_t widget)
{
	if (widget != NULL) {
		if (widget->placer_params) {
			ei_impl_app_run_siblings(widget);
		}

		widget = widget->children_head;

		while (widget != NULL) {
			if (widget->placer_params) {
				ei_impl_app_run_siblings(widget);
			}

			widget = widget->children_head;
		}
	}
}
