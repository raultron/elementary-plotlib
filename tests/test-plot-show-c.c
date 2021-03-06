#include <math.h>

#include "elem/elem.h"

static double xfsin(double x) {
    return sin(x) / x;
}

static double xfcos(double x) {
    return cos(x) / x;
}

int main() {
    elem_plot *plot = elem_plot_new(elem_plot_show_units | elem_plot_auto_limits);
    elem_plot_set_clip_mode(plot, false);

    // TODO add PI definition
    const double x0 = 0.8, x1 = 8 * 2 * 3.14159265359;
    elem_path *spath = elem_path_new();
    elem_path_move_to(spath, x0, xfsin(x0));
    const int n = 256;
    for (int i = 1; i < n; i++) {
        const double x = x0 + i * (x1 - x0) / (n - 1);
        elem_path_line_to(spath, x, xfsin(x));
    }
    elem_plot_add(plot, path_as_object(spath), elem_color_blue, 1.5, elem_color_none, elem_stroke);

    elem_plot_set_title(plot, "Function plot example");
    elem_plot_set_x_axis_title(plot, "x variable");
    elem_plot_set_y_axis_title(plot, "y variable");
    elem_plot_show(plot, 640, 480, elem_window_resize);

    elem_path *cpath = elem_path_new();
    elem_path_move_to(cpath, x0, xfcos(x0));
    for (int i = 1; i < n; i++) {
        const double x = x0 + i * (x1 - x0) / (n - 1);
        elem_path_line_to(cpath, x, xfcos(x));
    }
    elem_plot_add(plot, path_as_object(cpath), elem_color_red, 1.5, elem_color_none, elem_stroke);

    elem_plot_wait(plot);
    elem_plot_free(plot);
    return 0;
}
