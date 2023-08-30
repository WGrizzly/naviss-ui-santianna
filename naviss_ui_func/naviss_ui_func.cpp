#include <iostream>
#include <string>
#include <thread>
#include <opencv2/opencv.hpp>
#include <glog/logging.h>

#include <gtk-3.0/gtk/gtk.h>
#include <glib.h>
#include <gdk/gdk.h>
#include <epoxy/gl.h>
#include <GL/gl.h>
// #include <gtkmm-3.0/gtkmm.h>

#define IGNORE_VAR(type, identifier) \
{ \
  type IGNORED_VARIABLE_abcd = identifier; \
  identifier = IGNORED_VARIABLE_abcd; \
}

typedef struct _NAVISSMainWindow
{
    char *message;

    GtkWidget *main_window;
    GtkWidget *fixed_window;

    GtkWidget *flip_btn;
    GtkWidget *option_btn;
    GtkWidget *history_btn;

    GtkWidget *full_btn;

    GtkWidget *top_view;
    GtkWidget *cam1_view;
    GtkWidget *cam2_view;
    GtkWidget *cam3_view;
    GtkWidget *cam4_view;

    GtkWidget *front_view;

    GtkWidget *icon_view0;
    GtkWidget *icon_view1;
    GtkWidget *icon_view2;
    GtkWidget *icon_view3;

    GtkWidget *full_view;

    GtkWidget *naviss_logo;
    GtkWidget *full_btn_img;
    GtkWidget *org_btn_img;

    GtkWidget *naview_btn;
    GtkWidget *berthview_btn;

    GtkWidget *gl_area;

    bool _b_enable_left_view;
    bool _b_enable_full_view;
    bool _b_enable_nav_view;
    bool _b_enable_berth_view;

    int _i_save_option;
} NAVISSMainWindow;

GLuint gl_vao, gl_buffer, gl_program;

const GLchar *vert_src ="\n" \
"#version 330                                  \n" \
"#extension GL_ARB_explicit_attrib_location: enable  \n" \
"                                              \n" \
"layout(location = 0) in vec2 in_position;     \n" \
"                                              \n" \
"void main()                                   \n" \
"{                                             \n" \
"  gl_Position = vec4(in_position, 0.0, 1.0);  \n" \
"}                                             \n";

const GLchar *frag_src ="\n" \
"void main (void)                              \n" \
"{                                             \n" \
"  gl_FragColor = vec4(1.0, 1.0, 1.0, 1.0);    \n" \
"}                                             \n";


/*global variable*/

static void activate_gui(GtkApplication *app, NAVISSMainWindow nmw, gpointer user_data);
void on_flip_btn_clicked(GtkWidget *widget, gpointer data);
void on_history_btn_clicked(GtkWidget *widget, gpointer data);
void on_option_btn_clicked(GtkWidget *widget, gpointer data);
void on_naview_btn_clicked(GtkWidget *widget, gpointer data);
void on_berthview_btn_clicked(GtkWidget *widget, gpointer data);

void on_full_btn_clicked(GtkWidget *widget, gpointer data);

static void destroy(GtkWidget *widget, gpointer data);

gboolean main_thread_func(gpointer data);


int main(int argc, char *argv[])
{
    GtkApplication *app;
    int status = 0;

    /* Initialize the environment */
    app = gtk_application_new("naviss_app.co", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate_gui), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

bool g_do_first_init = false;
cv::Mat _dp_img[7][10];
int _file_idx = 0;
gboolean main_thread_func(gpointer data)
{
    NAVISSMainWindow* nmw = (NAVISSMainWindow*)data;

    if (!g_do_first_init)
    {
        std::string _img_base_path = "/home/hank/glade_ws/ListData";

        std::string _cam0_path = _img_base_path + "/0/";
        std::string _cam1_path = _img_base_path + "/1/";
        std::string _cam2_path = _img_base_path + "/2/";
        std::string _cam3_path = _img_base_path + "/3/";
        std::string _top_view_path = _img_base_path + "/th/";
        std::string _full_view_path = _img_base_path + "/tv/";
        std::string _front_view_path = _img_base_path + "/st_0/";

        for (int i = 0; i < 5; i++)
        {
            std::cout << _cam0_path + std::to_string(i) + ".png" << std::endl;
            _dp_img[0][i] = cv::imread(_cam0_path + std::to_string(i) + ".png");
            cv::resize(_dp_img[0][i], _dp_img[0][i], cv::Size(630, 455));
            cv::cvtColor(_dp_img[0][i], _dp_img[0][i], cv::COLOR_RGB2BGR);
            _dp_img[1][i] = cv::imread(_cam1_path + std::to_string(i) + ".png");
            cv::resize(_dp_img[1][i], _dp_img[1][i], cv::Size(630, 455));
            cv::cvtColor(_dp_img[1][i], _dp_img[1][i], cv::COLOR_RGB2BGR);
            _dp_img[2][i] = cv::imread(_cam2_path + std::to_string(i) + ".png");
            cv::resize(_dp_img[2][i], _dp_img[2][i], cv::Size(630, 455));
            cv::cvtColor(_dp_img[2][i], _dp_img[2][i], cv::COLOR_RGB2BGR);
            _dp_img[3][i] = cv::imread(_cam3_path + std::to_string(i) + ".png");
            cv::resize(_dp_img[3][i], _dp_img[3][i], cv::Size(630, 455));
            cv::cvtColor(_dp_img[3][i], _dp_img[3][i], cv::COLOR_RGB2BGR);
            _dp_img[4][i] = cv::imread(_top_view_path + std::to_string(i) + ".png");
            cv::resize(_dp_img[4][i], _dp_img[4][i], cv::Size(630, 1070));
            cv::cvtColor(_dp_img[4][i], _dp_img[4][i], cv::COLOR_RGB2BGR);
            _dp_img[5][i] = cv::imread(_full_view_path + std::to_string(i) + ".png");
            cv::resize(_dp_img[5][i], _dp_img[5][i], cv::Size(1910, 1070));
            cv::cvtColor(_dp_img[5][i], _dp_img[5][i], cv::COLOR_RGB2BGR);
            _dp_img[6][i] = cv::imread(_front_view_path + std::to_string(i) + ".png");
            cv::resize(_dp_img[6][i], _dp_img[6][i], cv::Size(1270, 455));
            cv::cvtColor(_dp_img[6][i], _dp_img[6][i], cv::COLOR_RGB2BGR);

        }
        g_do_first_init = true;

        printf("g_do_first_init end\n");
    }

    // while(g_b_thread_running)
    {
        // std::chrono::system_clock::time_point _tp = std::chrono::system_clock::now();

        // for (int j = 0; j < 6; j++)
        // {
        //     // cv::cvtColor(_dp_img[j][i], _dp_img[j][i], cv::COLOR_BGR2RGB);
        //     g_pixbuf[j][i] = gdk_pixbuf_new_from_data(_dp_img[j][i].data, GDK_COLORSPACE_RGB, false, 8, _dp_img[j][i].cols, _dp_img[j][i].rows, _dp_img[j][i].step[0], NULL, NULL);
        // }

        // g_mutex_lkock(&g_gui_mutex_if);
        if (!nmw->_b_enable_full_view)
        {
            if (nmw->_b_enable_nav_view)
            {
                GdkPixbuf *_front_view = gdk_pixbuf_new_from_data(_dp_img[6][_file_idx].data,
                                                                 GDK_COLORSPACE_RGB,
                                                                 false,
                                                                 8,
                                                                 _dp_img[6][_file_idx].cols,
                                                                 _dp_img[6][_file_idx].rows,
                                                                 _dp_img[6][_file_idx].step[0], NULL, NULL);
                gtk_image_set_from_pixbuf(GTK_IMAGE(nmw->front_view), _front_view);
                g_object_unref(_front_view);                
            }
            else
            {
                GdkPixbuf *_cam1_view = gdk_pixbuf_new_from_data(_dp_img[0][_file_idx].data,
                                                                 GDK_COLORSPACE_RGB,
                                                                 false,
                                                                 8,
                                                                 _dp_img[0][_file_idx].cols,
                                                                 _dp_img[0][_file_idx].rows,
                                                                 _dp_img[0][_file_idx].step[0], NULL, NULL);
                gtk_image_set_from_pixbuf(GTK_IMAGE(nmw->cam1_view), _cam1_view);
                g_object_unref(_cam1_view);

                GdkPixbuf *_cam2_view = gdk_pixbuf_new_from_data(_dp_img[1][_file_idx].data,
                                                                 GDK_COLORSPACE_RGB,
                                                                 false,
                                                                 8,
                                                                 _dp_img[1][_file_idx].cols,
                                                                 _dp_img[1][_file_idx].rows,
                                                                 _dp_img[1][_file_idx].step[0], NULL, NULL);
                gtk_image_set_from_pixbuf(GTK_IMAGE(nmw->cam2_view), _cam2_view);
                g_object_unref(_cam2_view);
            }

            GdkPixbuf *_cam3_view = gdk_pixbuf_new_from_data(_dp_img[2][_file_idx].data,
                                                             GDK_COLORSPACE_RGB,
                                                             false,
                                                             8,
                                                             _dp_img[2][_file_idx].cols,
                                                             _dp_img[2][_file_idx].rows,
                                                             _dp_img[2][_file_idx].step[0], NULL, NULL);
            gtk_image_set_from_pixbuf(GTK_IMAGE(nmw->cam3_view), _cam3_view);
            g_object_unref(_cam3_view);

            GdkPixbuf *_cam4_view = gdk_pixbuf_new_from_data(_dp_img[3][_file_idx].data,
                                                             GDK_COLORSPACE_RGB,
                                                             false,
                                                             8,
                                                             _dp_img[3][_file_idx].cols,
                                                             _dp_img[3][_file_idx].rows,
                                                             _dp_img[3][_file_idx].step[0], NULL, NULL);
            gtk_image_set_from_pixbuf(GTK_IMAGE(nmw->cam4_view), _cam4_view);
            g_object_unref(_cam4_view);


            GdkPixbuf *_top_view = gdk_pixbuf_new_from_data(_dp_img[4][_file_idx].data,
                                                             GDK_COLORSPACE_RGB,
                                                             false,
                                                             8,
                                                             _dp_img[4][_file_idx].cols,
                                                             _dp_img[4][_file_idx].rows,
                                                             _dp_img[4][_file_idx].step[0], NULL, NULL);
            gtk_image_set_from_pixbuf(GTK_IMAGE(nmw->top_view), _top_view);
            g_object_unref(_top_view);
        }
        else
        {
            GdkPixbuf *_full_view = gdk_pixbuf_new_from_data(_dp_img[5][_file_idx].data,
                                                             GDK_COLORSPACE_RGB,
                                                             false,
                                                             8,
                                                             _dp_img[5][_file_idx].cols,
                                                             _dp_img[5][_file_idx].rows,
                                                             _dp_img[5][_file_idx].step[0], NULL, NULL);
            gtk_image_set_from_pixbuf(GTK_IMAGE(nmw->full_view), _full_view);
            g_object_unref(_full_view);
        }

        // g_mutex_unlock(&g_gui_mutex_if);

        _file_idx = (_file_idx + 1) % 5;

        // printf("thread run widh idx: %d\n", _file_idx);

        // while (gtk_events_pending())
        //     gtk_main_iteration();

        // int _i_duration_ms = 100 - std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - _tp).count();
        // if (_i_duration_ms > 0)
        // {
        //     printf("thread running within %d.\n", _i_duration_ms);
        //     std::this_thread::sleep_for(std::chrono::milliseconds(_i_duration_ms));
        // }
    }

    // free(nmw);

    // g_b_thread_running = false;

    // Return false to detach update ui thread, reattach it after a timeout so CPU doesn't spin unnecessarily.
    // pthread_t _recursive;
    // if (pthread_create(&_recursive, NULL, ui_update_restart_thread, nmw))
    //     exit(-1);

    return TRUE;
}

static gboolean render(GtkGLArea *area, GdkGLContext *context)
{
    IGNORE_VAR(GdkGLContext *, context);
    IGNORE_VAR(GtkGLArea *, area);

    // inside this function it's safe to use GL; the given
    // `GdkGLContext` has been made current to the drawable
    // surface used by the `GtkGLArea` and the viewport has
    // already been set to be the size of the allocation

    // we can start by clearing the buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 255.0, 1.0);

    glUseProgram(gl_program);
    glBindVertexArray(gl_vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    glBindVertexArray(0);
    glUseProgram(0);

    glFlush();
    // draw your object
    // draw_an_object();

    // we completed our drawing; the draw commands will be
    // flushed at the end of the signal emission chain, and
    // the buffers will be drawn on the window
    return TRUE;
}

static gboolean on_realize(GtkGLArea *area, GdkGLContext *context)
{
    IGNORE_VAR(GdkGLContext *, context);

    printf("on_realize: start\n");
	gtk_gl_area_make_current(GTK_GL_AREA(area));
	if(gtk_gl_area_get_error(area) != NULL)
	{
		printf("on_realize: error: gtk_gl_area_make_current\n");
		return FALSE;
	}

    GLfloat verts[] =
        {
            +0.0f,
            +1.0f,
            -1.0f,
            -1.0f,
            +1.0f,
            -1.0f,
        };

    GLuint frag_shader, vert_shader;
    frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
    vert_shader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(frag_shader, 1, &frag_src, NULL);
    glShaderSource(vert_shader, 1, &vert_src, NULL);

    glCompileShader(frag_shader);
    glCompileShader(vert_shader);

    gl_program = glCreateProgram();
    glAttachShader(gl_program, frag_shader);
    glAttachShader(gl_program, vert_shader);
    glLinkProgram(gl_program);

    glGenVertexArrays(1, &gl_vao);
    glBindVertexArray(gl_vao);

    glGenBuffers(1, &gl_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, gl_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glBindVertexArray(0);

    glDeleteBuffers(1, &gl_buffer);

    printf("GL_VENDOR: %s\n", glGetString(GL_VENDOR));
	printf("GL_RENDERER: %s\n", glGetString(GL_RENDERER));
	printf("GL_VERSION: %s\n", glGetString(GL_VERSION));
	printf("GL_SHADING_LANGUAGE_VERSION: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("on_realize: end\n");
	return TRUE;
}

void activate_gui(GtkApplication *app, NAVISSMainWindow nmw, gpointer user_data)
{
    GtkBuilder *builder;
    GtkWidget *window;

    nmw.message = "Set main window\n";

    /* Import UI designed via Glade */
    builder = gtk_builder_new();
    gtk_builder_add_from_file(builder, "/home/hank/glade_ws/NAVISS_MAIN_UI.glade", NULL);

    /* get pointers to the Widgets */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "NAVISS_MAIN"));
    nmw.main_window = window;

    nmw.fixed_window = GTK_WIDGET(gtk_builder_get_object(builder, "NAVISS_MAIN_FIXED"));

    nmw.flip_btn = GTK_WIDGET(gtk_builder_get_object(builder, "BUTTON_FLIPVIEW"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(nmw.flip_btn), true);
    nmw._b_enable_left_view = true;

    nmw.option_btn = GTK_WIDGET(gtk_builder_get_object(builder, "BUTTON_OPTION"));

    nmw.history_btn = GTK_WIDGET(gtk_builder_get_object(builder, "BUTTON_HISTORY"));

    nmw.cam1_view = GTK_WIDGET(gtk_builder_get_object(builder, "IMAGE_CAM1"));
    nmw.cam2_view = GTK_WIDGET(gtk_builder_get_object(builder, "IMAGE_CAM2"));
    nmw.cam3_view = GTK_WIDGET(gtk_builder_get_object(builder, "IMAGE_CAM3"));
    nmw.cam4_view = GTK_WIDGET(gtk_builder_get_object(builder, "IMAGE_CAM4"));

    nmw.top_view = GTK_WIDGET(gtk_builder_get_object(builder, "IMAGE_TOP_VIEW"));

    nmw.icon_view0 = GTK_WIDGET(gtk_builder_get_object(builder, "ICON_VIEW0"));
    gtk_image_set_from_file(GTK_IMAGE(nmw.icon_view0), "/home/hank/glade_ws/resources/view_0.svg");
    nmw.icon_view1 = GTK_WIDGET(gtk_builder_get_object(builder, "ICON_VIEW1"));
    gtk_image_set_from_file(GTK_IMAGE(nmw.icon_view1), "/home/hank/glade_ws/resources/view_2.svg");
    nmw.icon_view2 = GTK_WIDGET(gtk_builder_get_object(builder, "ICON_VIEW2"));
    gtk_image_set_from_file(GTK_IMAGE(nmw.icon_view2), "/home/hank/glade_ws/resources/view_5.svg");
    nmw.icon_view3 = GTK_WIDGET(gtk_builder_get_object(builder, "ICON_VIEW3"));
    gtk_image_set_from_file(GTK_IMAGE(nmw.icon_view3), "/home/hank/glade_ws/resources/view_8.svg");        

    nmw.front_view = GTK_WIDGET(gtk_builder_get_object(builder, "IMAGE_FRONT"));

    nmw.full_view = GTK_WIDGET(gtk_builder_get_object(builder, "IMAGE_FULLVIEW"));
    nmw._b_enable_full_view = false;

    nmw.full_btn = GTK_WIDGET(gtk_builder_get_object(builder, "BUTTON_FULLVIEW"));

    // nmw.full_btn_img = gtk_image_new_from_file("/home/hank/glade_ws/resources/btn_full.svg");
    // nmw.org_btn_img = gtk_image_new_from_file("/home/hank/glade_ws/resources/btn_mod.svg");
    // gtk_button_set_image(GTK_BUTTON(nmw.full_btn), nmw.full_btn_img);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(nmw.full_btn), false);
    // gtk_button_set_relief(GTK_BUTTON(nmw.full_btn), GTK_RELIEF_NONE);

    nmw.naviss_logo = GTK_WIDGET(gtk_builder_get_object(builder, "IMAGE_NAVISS_LOGO"));
    gtk_image_set_from_file(GTK_IMAGE(nmw.naviss_logo), "/home/hank/glade_ws/resources/logo.svg");

    nmw.naview_btn = GTK_WIDGET(gtk_builder_get_object(builder, "BUTTON_NAVIEW"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(nmw.naview_btn), false);
    nmw._b_enable_nav_view = false;

    nmw.berthview_btn = GTK_WIDGET(gtk_builder_get_object(builder, "BUTTON_DOCKVIEW"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(nmw.berthview_btn), false);
    nmw._b_enable_berth_view = false;

    nmw.gl_area = GTK_WIDGET(gtk_builder_get_object(builder, "GLVIEW_DEMO"));

    /* Register callbacks */
    // Connecting the signals in the code
    gtk_builder_connect_signals(builder, NULL);

    // Passing pointer to a structure of pointers to a callback function
    g_signal_connect(nmw.flip_btn, "toggled", G_CALLBACK(on_flip_btn_clicked), &nmw);
    g_signal_connect(nmw.history_btn, "clicked", G_CALLBACK(on_history_btn_clicked), &nmw);
    g_signal_connect(nmw.option_btn, "clicked", G_CALLBACK(on_option_btn_clicked), &nmw);
    g_signal_connect(nmw.full_btn, "toggled", G_CALLBACK(on_full_btn_clicked), &nmw);
    g_signal_connect(nmw.naview_btn, "toggled", G_CALLBACK(on_naview_btn_clicked), &nmw);
    g_signal_connect(nmw.berthview_btn, "toggled", G_CALLBACK(on_berthview_btn_clicked), &nmw);
    g_signal_connect(nmw.gl_area, "realize", G_CALLBACK(on_realize), NULL);
    g_signal_connect(nmw.gl_area, "render", G_CALLBACK(render), NULL);
    // g_signal_connect(nmw.gl_area, "unrealize", G_CALLBACK(unrealize), &nmw);

    g_object_unref(builder);

    g_timeout_add(100, main_thread_func, &nmw);

    gtk_widget_show(window);
    gtk_widget_hide(nmw.full_view);
    gtk_widget_hide(nmw.front_view);

    gtk_main();
}

void on_naview_btn_clicked(GtkWidget *widget, gpointer data)
{
    NAVISSMainWindow *nmw;
    nmw = (NAVISSMainWindow *)data;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(nmw->naview_btn)))
    {
        nmw->_b_enable_nav_view = true;
        gtk_widget_hide(nmw->cam1_view);
        gtk_widget_hide(nmw->cam2_view);
        gtk_widget_hide(nmw->icon_view0);
        gtk_widget_hide(nmw->icon_view1);

        gtk_widget_show(nmw->front_view);        

        if (nmw->_b_enable_left_view)
        {
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->front_view), 645, 150);
        }
        else
        {
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->front_view), 5, 150);
        }
    }
    else
    {
        nmw->_b_enable_nav_view = false;
        gtk_widget_show(nmw->cam1_view);
        gtk_widget_show(nmw->cam2_view);
        gtk_widget_show(nmw->icon_view0);
        gtk_widget_show(nmw->icon_view1);

        gtk_widget_hide(nmw->front_view);         
    }
}

void on_berthview_btn_clicked(GtkWidget *widget, gpointer data)
{
    NAVISSMainWindow *nmw;
    nmw = (NAVISSMainWindow *)data;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(nmw->berthview_btn)))
    {

    }
    else
    {

    }        
}

void on_flip_btn_clicked(GtkWidget *widget, gpointer data)
{
    NAVISSMainWindow *nmw;
    nmw = (NAVISSMainWindow *)data;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(nmw->flip_btn)))
    {
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->top_view), 5, 5);
        if(nmw->_b_enable_nav_view)
        {
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->front_view), 645, 150);
        }
        else
        {
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam1_view), 645, 150);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam2_view), 1285, 150);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view0), 1223, 155);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view1), 1863, 155);
        }
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam3_view), 645, 615);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam4_view), 1285, 615);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->full_btn), 530, 970);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->naviss_logo), 1794, 25);

        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view2), 1223, 620);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view3), 1863, 620);
        gtk_button_set_label(GTK_BUTTON(nmw->flip_btn), "LeftView");
        // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(nmw->right_btn), false);
        nmw->_b_enable_left_view = true;
    }
    else
    {
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->top_view), 1285, 5);
        if(nmw->_b_enable_nav_view)
        {
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->front_view), 5, 150);
        }
        else
        {
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam1_view), 5, 150);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam2_view), 645, 150);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view0), 588, 155);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view1), 1223, 155);
        }        
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam3_view), 5, 615);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam4_view), 645, 615);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->full_btn), 1810, 970);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->naviss_logo), 10, 25);

        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view2), 588, 620);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view3), 1223, 620);
        gtk_button_set_label(GTK_BUTTON(nmw->flip_btn), "RightView");
        // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(nmw->right_btn), true);
        nmw->_b_enable_left_view = false;
    }

    // free(nmw);
}

void on_history_btn_clicked(GtkWidget *widget, gpointer data)
{
    g_return_if_fail(data != NULL);

    NAVISSMainWindow *nmw;
    nmw = (NAVISSMainWindow *)data;

    GtkWidget *history_modal;
    GtkBuilder *history_builder;

    history_builder = gtk_builder_new_from_file("/home/hank/glade_ws/NAVISS_MAIN_UI.glade");
    history_modal = GTK_WIDGET(gtk_builder_get_object(history_builder, "NAVISS_HISTORY"));

    gtk_widget_show_all(history_modal);

    gtk_dialog_run(GTK_DIALOG(history_modal));

    gtk_widget_destroy(GTK_WIDGET(history_modal));
    g_object_unref(G_OBJECT(history_builder));
}

void on_option_btn_clicked(GtkWidget *widget, gpointer data)
{
    g_return_if_fail (data != NULL);

    NAVISSMainWindow *nmw;
    nmw = (NAVISSMainWindow *)data;

    GtkWidget *option_modal;
    GtkBuilder *option_builder;

    option_builder = gtk_builder_new_from_file("/home/hank/glade_ws/NAVISS_MAIN_UI.glade");
    option_modal = GTK_WIDGET(gtk_builder_get_object(option_builder, "NAVISS_OPTION"));

    // gtk_window_set_modal(GTK_WINDOW(GTK_DIALOG(option_modal)), TRUE);
    // gtk_window_set_transient_for(GTK_WINDOW(option_modal), GTK_WINDOW(data));

    gtk_widget_show_all(option_modal);

    nmw->_i_save_option = gtk_dialog_run(GTK_DIALOG(option_modal));

    gtk_widget_destroy(GTK_WIDGET(option_modal));
    g_object_unref(G_OBJECT(option_builder));
}

void on_full_btn_clicked(GtkWidget *widget, gpointer data)
{
    NAVISSMainWindow *nmw;
    nmw = (NAVISSMainWindow *)data;

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(nmw->full_btn)))
    {
        nmw->_b_enable_full_view = true;
        gtk_widget_hide(nmw->cam1_view);
        gtk_widget_hide(nmw->cam2_view);
        gtk_widget_hide(nmw->cam3_view);
        gtk_widget_hide(nmw->cam4_view);
        gtk_widget_hide(nmw->flip_btn);
        gtk_widget_hide(nmw->option_btn);
        gtk_widget_hide(nmw->history_btn);
        gtk_widget_hide(nmw->naview_btn);
        gtk_widget_hide(nmw->berthview_btn);
        gtk_widget_hide(nmw->top_view);
        gtk_widget_hide(nmw->icon_view0);
        gtk_widget_hide(nmw->icon_view1);
        gtk_widget_hide(nmw->icon_view2);
        gtk_widget_hide(nmw->icon_view3);
        gtk_widget_hide(nmw->front_view);
        gtk_widget_show(nmw->full_view);
        gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->full_btn), 1810, 970);
        gtk_button_set_label(GTK_BUTTON(nmw->full_btn), "ModView");
        gtk_widget_show(nmw->full_btn);
        // gtk_button_set_relief(GTK_BUTTON(nmw->full_btn), GTK_RELIEF_NONE);
        // gtk_button_set_image(GTK_BUTTON(nmw->full_btn), nmw->org_btn_img);
    }
    else
    {
        nmw->_b_enable_full_view = false;

        if(nmw->_b_enable_nav_view)
        {
            gtk_widget_show(nmw->front_view);
        }
        else
        {
            gtk_widget_show(nmw->cam1_view);
            gtk_widget_show(nmw->cam2_view);
            gtk_widget_show(nmw->icon_view0);
            gtk_widget_show(nmw->icon_view1);
        }

        gtk_widget_show(nmw->cam3_view);
        gtk_widget_show(nmw->cam4_view);
        gtk_widget_show(nmw->flip_btn);
        gtk_widget_show(nmw->option_btn);
        gtk_widget_show(nmw->history_btn);
        gtk_widget_show(nmw->naview_btn);
        gtk_widget_show(nmw->berthview_btn);
        gtk_widget_show(nmw->top_view);
        gtk_widget_show(nmw->icon_view2);
        gtk_widget_show(nmw->icon_view3);
        gtk_widget_hide(nmw->full_view);

        if(nmw->_b_enable_left_view)
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(nmw->flip_btn), true);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->top_view), 5, 5);
            if (nmw->_b_enable_nav_view)
            {
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->front_view), 645, 150);
            }
            else
            {
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam1_view), 645, 150);
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam2_view), 1285, 150);
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view0), 1223, 155);
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view1), 1863, 155);
            }
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam3_view), 645, 615);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam4_view), 1285, 615);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->full_btn), 530, 970);

            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view2), 1223, 620);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view3), 1863, 620);
        }
        else
        {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(nmw->flip_btn), false);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->top_view), 1280, 5);
            if (nmw->_b_enable_nav_view)
            {
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->front_view), 5, 150);
            }
            else
            {
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam1_view), 5, 150);
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam2_view), 645, 150);
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view0), 588, 155);
                gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view1), 1223, 155);
            }
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam3_view), 5, 615);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->cam4_view), 645, 615);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->full_btn), 1810, 970);

            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view2), 588, 620);
            gtk_fixed_move(GTK_FIXED(nmw->fixed_window), GTK_WIDGET(nmw->icon_view3), 1223, 620);
        }
        gtk_button_set_label(GTK_BUTTON(nmw->full_btn), "FullView");
        // gtk_button_set_relief(GTK_BUTTON(nmw->full_btn), GTK_RELIEF_NONE);
        // gtk_button_set_image(GTK_BUTTON(nmw->full_btn), nmw->full_btn_img);
    }

    // free(nmw);
}

// =====================================================================
/* function definition */
// called when window is closed
static void destroy(GtkWidget *widget, gpointer data)
{
    // try
    // {
    //     g_b_thread_running = false;
    //     // if (g_main_thread.joinable())
    //     //     g_main_thread.join();
    //     // pthread_cancel(g_main_thread.native_handle());
    // }
    // catch (const std::exception &e)
    // {
    //     std::cerr << "Thread did't stoped with exception: " << e.what();
    // }

    gtk_main_quit();
}