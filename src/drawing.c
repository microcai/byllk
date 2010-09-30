/*
 * drawing.c
 *
 *  Created on: 2010-9-25
 *      Author: cai
 */


#include <config.h>

#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glext.h>

#include "byllk.h"


GLubyte pixels[3*64*64];

void make2dImage(void)
{
	int j,i;
	for(i = 0 ; i < 64;i++)
	{
		for (j = 0; j < 64; j++)
		{
			pixels[i*j] = 255;
			pixels[i*j + 1 ] = 255-2*j;
			pixels[i*j + 2 ] =  255-2*i;
		}
	}
}

static void draw_quad(double xsize,double ysize)
{
	double	point[4][2] = { {0,0},{0,ysize},{xsize,ysize},{xsize,0}};

	glBegin(GL_QUADS);

	glVertex2dv(point[0]);
	glVertex2dv(point[1]);
	glVertex2dv(point[2]);
	glVertex2dv(point[3]);

	glEnd();
}

static void draw_quads()
{
	int i,j;

	float color[2][3] = { {.7,.7,.7},{1,1,1} };

	double xsize =2.0/LINEX;
	double ysize =2.0/LINEY;

	glPushMatrix();

	glTranslated(-1,-1,0); //移到左下角

	for (i = 0; i < 16; i++)
	{
		glPushMatrix(); //这样做是为了排除累计误差

		glTranslated(0,i*ysize,0);

		for (j = 0; j < 25; j++)
		{
			glColor3fv(color[ (i^ j )&1 ]);
			draw_quad(xsize,ysize);
			glTranslated(xsize,0,0);
		}
		glPopMatrix();
	}
	glPopMatrix();
}

static void draw_ball(double xsize,double ysize,float color[])
{
	GLfloat Notes[8]={0.0f,0.0f,0.0f,0.0f,1.0f,1.0f,1.0f,1.0f};

	GLfloat	controlpoints[4][4][3]={
			{{-0.06,1,-0.05},	{0.2,1,0},		{0.8,1,0},	{1.06,1,-0.05},},
			{{0,0.8,0},	{0.2,0.8,0.4},	{0.8,0.8,0.4},	{1,0.8,0},},
			{{0,0.2,0},	{0.2,0.2,0.4},	{0.8,0.2,0.4},	{1,0.2,0},},
			{{-0.06,0,-0.05},	{0.2,0,0},		{0.8,0,0.0},	{1.06,0,-0.05},},
	};


	glPushMatrix();

	glScaled(xsize,ysize,(xsize+ysize)/2);
	glTranslated(0.1,0.1,0);
	glScaled(0.8,0.8,1);

	glColor3fv(color);
	glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
//	glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,70);
//	glMaterialf(GL_FRONT_AND_BACK,GL_SPECULAR,1);

	//让我们制造有亮点的小块吧，(*^__^*) 嘻嘻……

	GLUnurbs *Nurb = gluNewNurbsRenderer();

	//设置NURBS曲面对象属性
	gluNurbsProperty(Nurb,GLU_SAMPLING_TOLERANCE,150.0f);
//	gluNurbsProperty(Nurb,GLU_DISPLAY_MODE,GLU_FILL);
	//开始NURB区间绘制
	gluBeginSurface(Nurb);
	//定义NURBS曲面的形状
	gluNurbsSurface(Nurb,8,Notes,8,Notes,4*3,3,&controlpoints[0][0][0],4,4,GL_MAP2_VERTEX_3);
	//结束曲面绘制
	gluEndSurface(Nurb);
	gluDeleteNurbsRenderer(Nurb);

	glPopMatrix();
}

static void make_display_list(DisplayResource * displayrc)
{
	int i;
	GLint	ball_list;

	float	color[COLOR_NUM][4] = {
			{1,0.1,0.1,1},{1,1,0.1,1},{0.1,1,0.1,1},{0.2,0.3,1,1},
			{1,0.2,1,1},{0,0,0,.5},{0.6,.6,0.6,1}
	};

	double xsize =2.0/LINEX;
	double ysize =2.0/LINEY;

	ball_list = glGenLists(COLOR_NUM);

	for(i=0;i<COLOR_NUM;i++)
	{
		displayrc->colorball_displaylist[i] = ball_list + i;

		glNewList(displayrc->colorball_displaylist[i],GL_COMPILE);

		draw_ball(xsize,ysize,color[i]);

		glEndList();
	}

	displayrc->background_displaylist = glGenLists(1);
	glNewList(displayrc->background_displaylist,GL_COMPILE);
	draw_quads();
	glEndList();

/*	displayrc->clicked_displaylist = glGenLists(1);
	glNewList(displayrc->clicked_displaylist,GL_COMPILE);
	glColor4f(0,0,0,0.5);
	draw_quad(xsize,ysize);
	glEndList();
*/
}

gboolean on_draw(GtkWidget * widget , GdkEventExpose * event, gpointer user_data)
{
	GdkGLContext * glcontext;
	GdkGLDrawable* gldrawable;
	DisplayResource	* displayrc;

	int x,y;

	double xsize =2.0/LINEX;
	double ysize =2.0/LINEY;

	float	color[COLOR_NUM][3] = {
			{1,0,0},{1,1,0},{0,1,0},{0,0,1}
	};

	g_object_get(widget,"user-data",&displayrc,NULL);

	glcontext = gtk_widget_get_gl_context(widget);
	gldrawable= gtk_widget_get_gl_drawable(widget);

	gdk_gl_drawable_gl_begin(gldrawable,glcontext);

	glLoadIdentity();

	glClear(GL_DEPTH_BUFFER_BIT);

	//这样背景就显示好了，(*^__^*) 嘻嘻……
	glCallList(displayrc->background_displaylist);

	glEnable(GL_LIGHTING);
	glEnable(GL_MULTISAMPLE);

	glPushMatrix();

	glTranslated(-1,-1,0); //移到左下角

	for (y= 0; y < LINEY; y++)
	{
		glPushMatrix();

		glTranslated(0,ysize*y,0);

		for (x = 0; x < LINEX; x++)
		{
			if(displayrc->balls[x][y]>=0)
			{
				glCallList(displayrc->colorball_displaylist[displayrc->balls[x][y]]);
			}
			glTranslated(xsize,0,0);
		}
		glPopMatrix();
	}

	glPopMatrix();

	glDisable(GL_LIGHTING);
	glDisable(GL_MULTISAMPLE);
/*
	//clicked box
	if (displayrc->clicked.x >= 0 && displayrc->clicked.y >= 0)
	{
		glPushMatrix();
		glTranslated(xsize * displayrc->clicked.x - 1, ysize* displayrc->clicked.y - 1, 0);
		glCallList(displayrc->clicked_displaylist);
		glPopMatrix();
	}
*/
	if(gdk_gl_drawable_is_double_buffered(gldrawable))
		gdk_gl_drawable_swap_buffers(gldrawable);
	else
		glFlush();

//	g_error("%s",gluErrorString(glGetError()));


	gdk_gl_drawable_gl_end(gldrawable);
	return TRUE;
}

void initialize_gl_sence(DisplayResource *displayrc)
{

	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_AUTO_NORMAL);
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHT0);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);

//	glShadeModel(GL_FLAT);

	GLfloat	light_position[4] =  {
			1,1,20,1
	};

//	glLightfv(GL_LIGHT0,GL_POSITION,light_position);

	glSampleCoverage(0.9,GL_TRUE);

	make_display_list(displayrc);

}
