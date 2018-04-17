#ifndef AGGPLOT_PATH_H
#define AGGPLOT_PATH_H

#include "sg_path.h"
#include "sg_element.h"

namespace graphics {

// FIXME: sg_element does not own trans::scaling_a
class path : public sg_element {
public:
    path(agg::rgba8 fc, agg::rgba8 sc, float sw, unsigned flags = ELEMENT_DEFAULT)
    : sg_element(nullptr, fc, sc, sw, flags), m_path(), m_path_scaling(new trans::scaling_a(&m_path)) {
        set_object(m_path_scaling);
    }

    void line_to(double x, double y) {
        m_path.self().line_to(x, y);
    }

    void move_to(double x, double y) {
        m_path.self().move_to(x, y);
    }

    void close_polygon() {
        m_path.self().close_polygon();
    }

private:
    sg_path m_path;
    trans::scaling_a *m_path_scaling;
};

}

#endif
