/*
 * byllk.h
 *
 *  Created on: 2010-9-25
 *      Author: cai
 */

#ifndef BYLLK_H_
#define BYLLK_H_


#define	COLOR_NUM	7
#define LINEX	25
#define LINEY	16

static GdkGLConfig	 * glconfig;


typedef struct DisplayResource{
	GLint	background_displaylist;
	GLint	colorball_displaylist[COLOR_NUM];
//	GLint	clicked_displaylist;
	GLint	balls[LINEX][LINEY];
	GdkPoint	clicked;
}DisplayResource;

void initialize_gl_sence(DisplayResource *displayrc);
gboolean on_draw(GtkWidget * widget , GdkEventExpose * event, gpointer user_data);

#endif /* BYLLK_H_ */
