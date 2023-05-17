/**
 * @file	ei_implementation.h
 *
 * @brief 	Private definitions.
 * 
 */

#ifndef EI_IMPLEMENTATION_H
#define EI_IMPLEMENTATION_H

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
uint32_t ei_impl_map_rgba(ei_surface_t surface, ei_color_t color);

/**
 * @brief	Remplit un pixel avec une couleur donnée.
 *
 * @param	surface		Surface de dessin.
 * @param  	color		Couleur du pixel.
 * @param	pixel		Pixel à remplir.
 */
void ei_fill_pixel(ei_surface_t surface, const ei_color_t *color, ei_point_t pixel);

/**
 * @brief	Teste si clipper pointe vers NULL ou si le pixel appartient au clippeur.
 *
 * @param	x		Abscisse du pixel.
 * @param	y		Ordonnée du pixel.
 * @param	xc_min		Abscisse minimale du clippeur.
 * @param	xc_max		Abscisse maximale du clippeur.
 * @param	yc_min		Ordonnée minimale du clippeur.
 * @param	yc_max		Ordonnée maximale du clippeur.
 * @param	clipper		Pointeur vers clippeur.
 *
 * @return 	true si clipper pointe vers NULL ou si le pixel appartient au clippeur.
 */
bool in_clipper(int x, int y, int xc_min, int xc_max, int yc_min, int yc_max, const ei_rect_t *clipper);

/**
 * @brief	Compare deux rectangles.
 *
 * @param	rect1		Premier rectangle.
 * @param	rect2		Second rectangle.
 *
 * @return 	true si les deux rectangles sont égaux, false sinon.
 */
bool ei_rect_cmp(ei_rect_t rect1, ei_rect_t rect2);

/**
 * @brief	Calcule l'intersection entre deux rectangles.
 *
 * @param	rect1		Premier rectangle.
 * @param	rect2		Second rectangle.
 *
 * @return 	L'intersection entre les deux rectangles si elle est non vide, sinon un rectangle vide.
 */
ei_rect_t ei_rect_intersect(ei_rect_t rect1, ei_rect_t rect2);

bool in_rect(ei_point_t point, ei_rect_t rect);

/**
 * @brief	Copie le rectangle source dans la surface de destination à la position du rectangle de destination.
 *
 * @param	destination	Surface de destination.
 * @param	dst_rect	Rectangle de destination sur lequel faut copier le rectangle src_rect.
 * @param	source		Surface source.
 * @param	src_rect	Rectangle à copier.
 * @param	alpha		Paramètre de transparence.
 *
 */
void	ei_rect_cpy	(ei_surface_t		destination,
				const ei_rect_t*	dst_rect,
				ei_surface_t		source,
				const ei_rect_t*	src_rect,
				bool			alpha);

/**
 * \brief	Fields common to all types of widget. Every widget classes specializes this base
 *		class by adding its own fields. 
 */
typedef struct ei_impl_widget_t {
	ei_widgetclass_t*	wclass;		///< The class of widget of this widget. Avoids the field name "class" which is a keyword in C++.
	uint32_t		pick_id;	///< Id of this widget in the picking offscreen.
	ei_color_t		pick_color;	///< pick_id encoded as a color.
	void*			user_data;	///< Pointer provided by the programmer for private use. May be NULL.
	ei_widget_destructor_t	destructor;	///< Pointer to the programmer's function to call before destroying this widget. May be NULL.

	/* Widget Hierachy Management */
	ei_widget_t		parent;		///< Pointer to the parent of this widget.
	ei_widget_t		children_head;	///< Pointer to the first child of this widget.	Children are chained with the "next_sibling" field.
	ei_widget_t		children_tail;	///< Pointer to the last child of this widget.
	ei_widget_t		next_sibling;	///< Pointer to the next child of this widget's parent widget.

	/* Geometry Management */
	struct ei_impl_placer_params_t* placer_params;	///< Pointer to the placer parameters for this widget. If NULL, the widget is not currently managed and thus, is not displayed on the screen.
	ei_size_t		requested_size;	///< Size requested by the widget (big enough for its label, for example), or by the programmer. This can be different than its screen size defined by the placer.
	ei_rect_t		screen_location;///< Position and size of the widget expressed in the root window reference.
	ei_rect_t*		content_rect;	///< Where to place children, when this widget is used as a container. By defaults, points to the screen_location.
} ei_impl_widget_t;

void ei_widget_set_pick(ei_widget_t widget);

void pick(ei_widget_t widget, uint32_t pick_id_, ei_widget_t *widget_ptr);

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

/**
 * @brief	
 *
 * @param	anchor_ptr		
 * @param	rect		
 *
 * @return 	
 */
ei_point_t ei_anchor_rect(ei_anchor_t *anchor_ptr, ei_rect_t *rect);

/**
 * @brief	
 *
 * @param	anchor_ptr		
 * @param	rect		
 * @param	limit		
 *
 * @return 	
 */
ei_point_t ei_anchor_text_img(ei_anchor_t *anchor_ptr, ei_rect_t *rect, ei_rect_t *limit);

/**
 * \brief	A structure storing the placement parameters of a widget.
 *		You have to define this structure: no suggestion provided.
 */
struct ei_impl_placer_params_t {
    int		x;
    int		y;
    int		width;
    int		height;
    float	rel_x;
    float	rel_y;
    float	*rel_width;
    float	*rel_height;
    ei_anchor_t	anchor;
};

/**
 * \brief	Tells the placer to recompute the geometry of a widget.
 *		The widget must have been previsouly placed by a call to \ref ei_place.
 *		Geometry re-computation is necessary for example when the text label of
 *		a widget has changed, and thus the widget "natural" size has changed.
 *
 * @param	widget		The widget which geometry must be re-computed.
 */
void ei_impl_placer_run(ei_widget_t widget);

void ei_impl_app_run_children(ei_widget_t widget);

void ei_impl_app_run_siblings(ei_widget_t widget);

#endif
