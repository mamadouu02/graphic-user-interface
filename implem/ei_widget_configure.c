/**
 * @file	ei_widget_configure.c
 *
 * @brief 	API for widgets configuration.
 *
 */

#include "ei_widget_configure.h"
#include "ei_frame.h"
#include "ei_button.h"
#include "ei_draw.h"

void    ei_frame_configure      (ei_widget_t		widget,
				 ei_size_t*		requested_size,
				 const ei_color_t*	color,
				 int*			border_width,
				 ei_relief_t*		relief,
				 ei_string_t*		text,
				 ei_font_t*		text_font,
				 ei_color_t*		text_color,
				 ei_anchor_t*		text_anchor,
				 ei_surface_t*		img,
				 ei_rect_ptr_t*		img_rect,
				 ei_anchor_t*		img_anchor)
{
	ei_impl_frame_t *frame = (ei_impl_frame_t *) widget;

	widget->requested_size.height = (requested_size) ? requested_size->height : widget->requested_size.height;
	widget->requested_size.width = (requested_size) ? requested_size->width : widget->requested_size.width;
	widget->screen_location.size.height = (requested_size) ? requested_size->height : widget->requested_size.height;
	widget->screen_location.size.width = (requested_size) ? requested_size->width : widget->requested_size.width;
	widget->content_rect = &widget->screen_location;

	if (widget->parent != NULL) {
		if (widget->placer_params == NULL) {
			widget->placer_params = malloc(sizeof(struct ei_impl_placer_params_t));
		}

		widget->placer_params->height = (requested_size) ? requested_size->height : widget->requested_size.height;
		widget->placer_params->width = (requested_size) ? requested_size->width : widget->requested_size.width;
	}

	frame->color = (color) ? *color : frame->color;
	frame->border_width = (border_width) ? *border_width : frame->border_width;
	frame->relief = (relief) ? *relief : frame->relief;

	if (frame->text)
		free(frame->text);

	frame->text = NULL;

	if (text && *text) {
		if (frame->text == NULL)
			frame->text = malloc(20 * sizeof(ei_string_t));

		strcpy(frame->text, *text);
	}

	frame->text_font = (text_font) ? *text_font : frame->text_font;
	frame->text_color = (text_color) ? *text_color : frame->text_color;
	frame->text_anchor = (text_anchor) ? *text_anchor : frame->text_anchor;

	ei_rect_t *rect_img = frame->img_rect;

	if (rect_img == NULL) {
		frame->img_rect = malloc(sizeof(ei_rect_t));
		rect_img = frame->img_rect;
	}

	if (img && *img) {
		ei_surface_t surface_image;
		surface_image = hw_surface_create(*img, hw_surface_get_size(*img), true);

		hw_surface_lock(surface_image);
		hw_surface_lock(*img);

		ei_copy_surface(surface_image, NULL, *img, NULL, false);

		hw_surface_unlock(surface_image);
		hw_surface_unlock(*img);

		frame->img = surface_image;

		*rect_img = hw_surface_get_rect(surface_image);
	}

	frame->img_rect->top_left.x = (img_rect) ? (*img_rect)->top_left.x : rect_img->top_left.x;
	frame->img_rect->top_left.y = (img_rect) ? (*img_rect)->top_left.y : rect_img->top_left.y;
	frame->img_rect->size.width = (img_rect) ? (*img_rect)->size.width : rect_img->size.width;
	frame->img_rect->size.height = (img_rect) ? (*img_rect)->size.height : rect_img->size.height;

	frame->img_anchor = (img_anchor) ? *img_anchor : frame->img_anchor;
}

void    ei_button_configure	(ei_widget_t		widget,
				    ei_size_t*		requested_size,
				    const ei_color_t*	color,
				    int*			border_width,
				    int*			corner_radius,
				    ei_relief_t*		relief,
				    ei_string_t*		text,
				    ei_font_t*		text_font,
				    ei_color_t*		text_color,
				    ei_anchor_t*		text_anchor,
				    ei_surface_t*		img,
				    ei_rect_ptr_t*		img_rect,
				    ei_anchor_t*		img_anchor,
				    ei_callback_t*		callback,
				    ei_user_param_t*	user_param)
{
	ei_impl_button_t *button = (ei_impl_button_t *) widget;
	widget->requested_size = (requested_size) ? *requested_size : widget->requested_size;
	button->color = (color) ? *color : button->color;
	button->border_width = (border_width) ? *border_width : button->border_width;
	button->corner_radius = (corner_radius) ? *corner_radius : button->corner_radius;
	button->relief = (relief) ? *relief : button->relief;

	if (button->text)
		free(button->text);

	button->text = NULL;

	if (text && *text) {
		if (button->text == NULL)
			button->text = malloc(20 * sizeof(ei_string_t));
		strcpy(button->text, *text);
	}

	button->text_font = (text_font) ? *text_font : button->text_font;
	button->text_color = (text_color) ? *text_color : button->text_color;
	button->text_anchor = (text_anchor) ? *text_anchor : button->text_anchor;

	ei_rect_t *rect_img = button->img_rect;

	if (rect_img == NULL) {
		button->img_rect = malloc(sizeof(ei_rect_t));
		rect_img = button->img_rect;
	}

	if (img && *img) {
		ei_surface_t surface_image;
		surface_image = hw_surface_create(*img, hw_surface_get_size(*img), true);

		hw_surface_lock(surface_image);
		hw_surface_lock(*img);

		ei_copy_surface(surface_image, NULL, *img, NULL, false);

		hw_surface_unlock(surface_image);
		hw_surface_unlock(*img);

		button->img = surface_image;

		*rect_img = hw_surface_get_rect(surface_image);
	}

	button->img_rect->top_left.x = (img_rect) ? (*img_rect)->top_left.x : rect_img->top_left.x;
	button->img_rect->top_left.y = (img_rect) ? (*img_rect)->top_left.y : rect_img->top_left.y;
	button->img_rect->size.width = (img_rect) ? (*img_rect)->size.width : rect_img->size.width;
	button->img_rect->size.height = (img_rect) ? (*img_rect)->size.height : rect_img->size.height;

	button->img_anchor = (img_anchor) ? *img_anchor : button->img_anchor;
	button->callback = (callback) ? *callback : button->callback;
	button->user_param = (user_param) ? *user_param : button->user_param;
}

void    ei_toplevel_configure	(ei_widget_t		widget,
				      ei_size_t*		requested_size,
				      ei_color_t*		color,
				      int*			border_width,
				      ei_string_t*		title,
				      bool*			closable,
				      ei_axis_set_t*		resizable,
				      ei_size_ptr_t*		min_size)
{
	ei_impl_toplevel_t *toplevel = (ei_impl_toplevel_t *) widget;

	widget->requested_size = (requested_size) ? *requested_size : widget->requested_size;
	toplevel->color = (color) ? *color : toplevel->color;
	toplevel->border_width = (border_width) ? *border_width : toplevel->border_width;

	if (title) {
		toplevel->title = malloc(20 * sizeof(ei_string_t));
		strcpy(toplevel->title, *title);
	}

	toplevel->closable = (closable) ? *closable : toplevel->closable;
	toplevel->resizable = (resizable) ? *resizable : toplevel->resizable;
	toplevel->min_size = (min_size) ? *min_size : toplevel->min_size;
}
