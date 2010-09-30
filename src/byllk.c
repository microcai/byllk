
#include <config.h>

#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gtk/gtkgl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include "byllk.h"

static void on_realize(GtkWidget *widget, gpointer user_data);
static gboolean on_resize(GtkWidget *widget, GdkEventConfigure *event,gpointer user_data);

static gboolean on_click(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	DisplayResource	*displayrc;
	gint	width,height;

	g_object_get(widget,"user-data",&displayrc,NULL);

	gdk_drawable_get_size(widget->window,&width,&height);

	double xsize =(double)width/LINEX;
	double ysize =(double)height/LINEY;

	displayrc->clicked.x = event->x/ xsize;
	displayrc->clicked.y = LINEY - event->y / ysize;

	//好了，检测

	if(displayrc->balls[displayrc->clicked.x][displayrc->clicked.y]>=0)
	{
		displayrc->clicked.x = displayrc->clicked.y = -1;
	}else //点到的是空白的，呵呵
	{
		GdkPoint		balls[4];
		GLint		balls_color[4]={-1,-1,-1,-1};
		//四个方向发散
		balls[0] = displayrc->clicked;
		balls[0].y = displayrc->clicked.y + 1;

		balls[1] = displayrc->clicked;
		balls[1].y = displayrc->clicked.y - 1;

		balls[2] = displayrc->clicked;
		balls[2].x = displayrc->clicked.x - 1;

		balls[3] = displayrc->clicked;
		balls[3].x = displayrc->clicked.x + 1;

		while(  balls[0].y < LINEY && displayrc->balls[balls[0].x][balls[0].y] <0 )
			balls[0].y ++;
		if(balls[0].y < LINEY)
			balls_color[0] = displayrc->balls[balls[0].x][balls[0].y];

		while(  balls[1].y > 0 && displayrc->balls[balls[1].x][balls[1].y] <0 )
			balls[1].y --;
		balls_color[1] = displayrc->balls[balls[1].x][balls[1].y];

		while(  balls[2].x >0  && displayrc->balls[balls[2].x][balls[2].y] <0 )
			balls[2].x --;
		balls_color[2] = displayrc->balls[balls[2].x][balls[2].y];

		while(  balls[3].x < LINEX  && displayrc->balls[balls[3].x][balls[3].y] <0 )
			balls[3].x ++;
		if(balls[3].x < LINEX)
			balls_color[3] = displayrc->balls[balls[3].x][balls[3].y];

		//处理好了，现在，我们需要去掉碰撞的啦
		for(int i=0;i<3;i++)
		{
			if(balls_color[i]<0)continue;
			for(int j=i+1;j<4;j++)
			{
				if(balls_color[j]<0)continue;
				if(balls_color[i] == balls_color[j])
				{
					displayrc->balls[balls[i].x][balls[i].y]=-1;
					displayrc->balls[balls[j].x][balls[j].y]=-1;
				}
			}
		}
	}
	gtk_widget_queue_draw(widget);
	return FALSE;
}

static gboolean on_iconfy(GtkWidget *widget, GdkEventWindowState *event,GtkWidget *draw_area)
{
	if(event->new_window_state & GDK_WINDOW_STATE_ICONIFIED)
	{

	}else
	{

	}
	return FALSE;
}

int main(int argc, char * argv[], char * env[])
{
	GtkWidget* window;
	GtkWidget* draw_area;

	setlocale(LC_ALL,"");
	textdomain(PACKAGE_NAME);

	g_thread_init(NULL);

	gtk_init(&argc,&argv);
	gtk_gl_init(&argc,&argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	draw_area = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window),draw_area);

	g_assert(glconfig = gdk_gl_config_new_by_mode(GDK_GL_MODE_RGBA|GDK_GL_MODE_DOUBLE|GDK_GL_MODE_MULTISAMPLE|GDK_GL_MODE_DEPTH));
	g_assert(gtk_widget_set_gl_capability(draw_area,glconfig,NULL,TRUE,GDK_GL_RGBA_TYPE));


	g_signal_connect(window,"destroy",gtk_main_quit,NULL);
	g_signal_connect(draw_area,"expose-event",G_CALLBACK(on_draw),NULL);
	g_signal_connect(draw_area,"configure-event",G_CALLBACK(on_resize),NULL);
	g_signal_connect(draw_area,"realize",G_CALLBACK(on_realize),NULL);
	g_signal_connect(draw_area,"button-press-event",G_CALLBACK(on_click),NULL);

	g_signal_connect(draw_area,"window-state-event",G_CALLBACK(on_iconfy),draw_area);

	gtk_widget_add_events(draw_area,GDK_BUTTON_PRESS_MASK|GDK_EXPOSURE_MASK|GDK_STRUCTURE_MASK);
	gtk_widget_add_events(window,GDK_STRUCTURE_MASK);

	gtk_window_set_default_size(GTK_WINDOW(window),25*32,16*32);

	gtk_widget_show_all(window);

//	g_timeout_add(100,(GSourceFunc)(gtk_widget_queue_draw),draw_area);

	gtk_main();
	return 0;
}

static void build_projection()
{
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	glFrustum(-0.5, 0.5, -0.5, 0.5, 1, 1000);
	glTranslated(0, 0, -2);

	glMatrixMode(GL_MODELVIEW);
}

static void on_realize(GtkWidget *widget, gpointer user_data)
{
	int	x,y;

	DisplayResource	* displayrc = g_new(DisplayResource,1);
	g_object_set(widget,"user-data",displayrc,NULL);

	srand(time(NULL));

	for(x=0;x < LINEX ; x++)
	{
		for(y=0;y<LINEY;y++)
		{
			displayrc->balls[x][y] = rand() % (COLOR_NUM+1) -1;
		}
	}

	GdkGLContext * glcontext;
	GdkGLDrawable* gldrawable;

	glcontext = gtk_widget_get_gl_context(widget);
	gldrawable= gtk_widget_get_gl_drawable(widget);

	gdk_gl_drawable_gl_begin(gldrawable,glcontext);


	build_projection();

	initialize_gl_sence(displayrc);

	gdk_gl_drawable_gl_end(gldrawable);

}

static gboolean on_resize(GtkWidget *widget, GdkEventConfigure *event,gpointer user_data)
{
	GdkGLContext * glcontext;
	GdkGLDrawable* gldrawable;

	glcontext = gtk_widget_get_gl_context(widget);
	gldrawable= gtk_widget_get_gl_drawable(widget);

	if (gdk_gl_drawable_gl_begin(gldrawable, glcontext))
	{
		glViewport(0, 0, event->width, event->height);

		build_projection();

		gdk_gl_drawable_gl_end(gldrawable);
	}

	return FALSE;
}
