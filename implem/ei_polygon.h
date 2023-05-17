/**
 * @file	ei_polygon.h
 *
 * @brief 	Polygons managment.
 *
 */

#ifndef EI_POLYGON_H
#define EI_POLYGON_H

#include "ei_frame.h"

/**
 * @brief	Structure représentant un côté.
 */
typedef struct ei_cote {
    int y_max;
    float x_ymin;
    float inv_pente;
    struct ei_cote *suiv;
} ei_cote_t;

/**
 * @brief	Initialise TC avec tous les côtés du polygone en ignorant les côtés horizontaux
 *
 * @param	tc			Pointeur vers TC.
 * @param	yp_min			Ordonnée minimale du polygone.
 * @param	point_array		Vecteur de points définissant le polygone.
 * @param	point_array_size	Taille du vecteur de points.
 */
void tc_init(ei_cote_t **tc, int yp_min, ei_point_t *point_array, size_t point_array_size);

/**
 * @brief	Déplace les côtés de TC(y) dans TCA et trie TCA par abscisses croissantes des
 * 		intersections de côtés avec la scanline.
 *
 * @param	tca_ptr		Pointeur vers TCA.
 * @param	tc		Table des côtés (TC).
 * @param	i_scan		Index de la scanline dans TC.
 */
void ei_tca_insert(ei_cote_t **tca_ptr, ei_cote_t **tc, int i_scan);

/**
 * @brief	Supprime de TCA les côtés tels que y_max = y_scan.
 *
 * @param	tca_ptr		Pointeur vers TCA.
 * @param	tc		Table des côtés (TC).
 * @param	y_scan		Ordonnée de la scanline.
 */
void tca_remove(ei_cote_t **tca_ptr, ei_cote_t **tc, int y_scan);

/**
 * @brief	Calcule la taille du tableau de points définissant un octant.
 *
 * @param	rayon		Rayon de l'octant.
 *
 * @return 	La taille du tableau de points.
 */
int ei_octant_array_size(int rayon);

/**
 * @brief	Crée un tableau de points définissant un octant.
 *
 * @param	centre		Centre de l'octant.
 * @param	rayon		Rayon de l'octant.
 * @param	octant		Numéro d'octant.
 *
 * @return 	Le tableau de points définissant l'octant.
 */
ei_point_t *ei_octant_array(ei_point_t centre, int rayon, int octant, int octant_array_size);

/**
 * @brief	
 *
 * @param	rayon		
 *
 * @return 	
 */
int ei_octant_lines_array_size(int rayon);

/**
 * @brief	
 *
 * @param	centre		Centre de l'octant.
 * @param	rayon		Rayon de l'octant.
 *
 * @return 	
 */
ei_point_t *ei_octant_lines_array(ei_point_t centre, int rayon);

/**
 * @brief	Identifies one part of a frame.
 */
typedef enum {
	ei_frame_bottom,
	ei_frame_top,
	ei_frame_total
} ei_frame_part_t;

/**
 * @brief	Crée un tableau de points définissant la partie haute ou basse d'un cadre aux bords arrondis.
 *
 * @param	rect		Rectangle définissant la zone de dessin.
 * @param	rayon		Rayon des arrondis.
 * @param	part		Partie du cadre (top, bottom ou total).
 *
 * @return 	Le tableau de points définissant la partie haute ou basse du cadre aux bords arrondis.
 */
ei_point_t *ei_rounded_frame(ei_rect_t rect, int rayon, ei_frame_part_t part);

/**
 * @brief	Dessine une frame.
 *
 * @param	surface		Surface de dessin.
 * @param  	rect		Rectangle définissant la zone de dessin.
 * @param	color		Couleur de la frame.
 * @param	relief		Relief de la frame.
 * @param	clipper		Clippeur.
 */
void ei_draw_frame(ei_surface_t *surface, ei_rect_t rect, ei_color_t color, ei_relief_t relief, ei_rect_t *clipper);

/**
 * @brief	Dessine un bouton.
 *
 * @param	surface		Surface de dessin.
 * @param  	rect		Rectangle définissant la zone de dessin.
 * @param	color		Couleur du bouton.
 * @param	radius		Rayon des coins.
 * @param	relief		Relief du bouton.
 * @param	clipper		Clippeur.
 * @param	text		Texte.
 * @param	text_font	Fonte du texte.
 * @param	text_color	Couleur du texte.
 * @param	text_anchor	Ancre du texte.
 */
void ei_draw_button(ei_surface_t *surface, ei_rect_t rect, ei_color_t color, int radius, ei_relief_t relief, ei_rect_t *clipper, ei_string_t *text, ei_font_t *text_font, ei_color_t *text_color, ei_anchor_t *text_anchor);

#endif
