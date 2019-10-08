#pragma once

#include <initializer_list>
#include <utility>

#include "agg_trans_affine.h"
#include "agg_path_storage.h"
#include "agg_conv_transform.h"
#include "agg_conv_dash.h"

#include "canvas_object.h"
#include "canvas_path.h"
#include "my_conv_simple_marker.h"

namespace graphics {

class dash_path : public canvas_path {
public:
    dash_path() : canvas_path(), m_path_dash(m_path_scaling), m_svg_dash_array(16) { }

    void rewind(unsigned path_id) override {
        m_path_dash.rewind(path_id);
    }

    unsigned vertex(double* x, double* y) override {
        return m_path_dash.vertex(x, y);
    }

    void add_dash(double a, double b) {
        m_path_dash.add_dash(a, b);
        m_dash_lengths.add(a);
        m_dash_lengths.add(b);
        m_svg_dash_array.append("", ',');
        m_svg_dash_array.printf_add("%g,%g", a, b);
    }

    svg_property_list* svg_path(str& s, double h) override {
        svg_coords_from_vs(&m_path_scaling, s, h);
        svg_property_item item(stroke_dasharray, m_svg_dash_array.cstr());
        return new svg_property_list(item, nullptr);
    }

    canvas_object *copy() const override {
        dash_path *new_object = new dash_path();
        vertex_source_copy(new_object->m_path, m_path);
        new_object->m_dash_lengths = m_dash_lengths;
        new_object->m_svg_dash_array = m_svg_dash_array;
        for (unsigned k = 0; k < m_dash_lengths.size(); k += 2) {
            float a = m_dash_lengths[k], b = m_dash_lengths[k + 1];
            new_object->m_path_dash.add_dash(a, b);
        }
        return new_object;
    }

protected:
    agg::conv_dash<agg::conv_transform<agg::path_storage>> m_path_dash;
    agg::pod_bvector<float> m_dash_lengths;
    str m_svg_dash_array;
};

class polygon : public canvas_path {
public:
    polygon(): canvas_path() { }
    polygon(std::initializer_list<std::pair<double, double>> lst): canvas_path() {
        path_from_initializer_list(lst);
        close_polygon();
    }
};

class markers : public canvas_path {
public:
    markers(double size, canvas_object* sym) : canvas_path(),
        m_marker_conv(m_path_scaling, *sym), m_size(size), m_scale(m_size), m_symbol(sym) {
        // we need to apply the scale transform here to ensure that
        // any call to bounding_box have the correct informations about
        // the symbol size, even if it is called before apply_transform
        m_symbol->apply_transform(m_scale, 1.0);
    }

    ~markers() override {
        delete m_symbol;
    }

    void apply_transform(const agg::trans_affine& m, double as) override {
        path_base_scaling::apply_transform(m, as);
        m_symbol->apply_transform(m_scale, as);
    }

    void rewind(unsigned path_id) override {
        m_marker_conv.rewind(path_id);
    }

    unsigned vertex(double* x, double* y) override {
        return m_marker_conv.vertex(x, y);
    }

    canvas_object *copy() const override {
        canvas_object *new_symbol = m_symbol->copy();
        markers *new_object = new markers(m_size, new_symbol);
        vertex_source_copy(new_object->m_path, m_path);
        return new_object;
    }

    str write_svg(int id, agg::rgba8 c, double h) override {
        str marker_id;
        str marker_def = gen_svg_marker_def(id, c, marker_id);

        str path;
        svg_coords_from_vs(&m_path_scaling, path, h);

        str marker_url = gen_marker_url(marker_id);
        const char* murl = marker_url.cstr();
        svg_property_item item1(marker_start, murl);
        svg_property_item item2(marker_mid, murl);
        svg_property_item item3(marker_end, murl);
        auto ls = new svg_property_list(item1, nullptr);
        ls = new svg_property_list(item2, ls);
        ls = new svg_property_list(item3, ls);

        str svg = svg_marker_path(path, m_size, id, ls);
        svg_property_list::free(ls);

        return str::print("%s\n   %s", marker_def.cstr(), svg.cstr());
    }

private:
    str gen_svg_marker_def(int id, agg::rgba8 c, str& marker_id) {
        const double pad = 2.0;
        double tx_save = m_scale.tx, ty_save = m_scale.ty;

        m_scale.tx = m_size / 2.0 + pad;
        m_scale.ty = m_size / 2.0 + pad;

        marker_id.printf("marker%i", id);

        const double S = m_size + 2*pad;
        const double wf = S / m_size;

        str marker_svg = m_symbol->write_svg(-1, c, S);

        str s = str::print("<defs><marker id=\"%s\" "
                           "refX=\"%g\" refY=\"%g\" "
                           "viewBox=\"0 0 %g %g\" orient=\"0\" "
                           "markerWidth=\"%g\" markerHeight=\"%g\">"
                           "%s"
                           "</marker></defs>",
                           marker_id.cstr(), S/2, S/2, S, S, wf, wf,
                           marker_svg.cstr());

        m_scale.tx = tx_save;
        m_scale.ty = ty_save;

        return s;
    }

    static str gen_marker_url(str& marker_id) {
        return str::print("url(#%s)", marker_id.cstr());
    }


private:
    my::conv_simple_marker<agg::conv_transform<agg::path_storage>, canvas_object> m_marker_conv;
    double m_size;
    agg::trans_affine_scaling m_scale;
    canvas_object* m_symbol;
};

class curve_path : public canvas_path_base {
public:
    curve_path() : canvas_path_base(), m_path_curve(m_path), m_scaling_matrix(), m_path_scaling(m_path_curve, m_scaling_matrix) { }

    virtual void apply_transform(const agg::trans_affine& m, double as) {
        m_scaling_matrix = m;
        m_path_curve.approximation_scale(as * m.scale());
    }

    virtual void rewind(unsigned path_id) {
        m_path_scaling.rewind(path_id);
    }

    virtual unsigned vertex(double* x, double* y) {
        return m_path_scaling.vertex(x, y);
    }

    virtual canvas_object *copy() const {
        curve_path *new_object = new curve_path();
        vertex_source_copy(new_object->m_path, m_path);
        return new_object;
    }

private:
    agg::conv_curve<agg::path_storage> m_path_curve;
    agg::trans_affine m_scaling_matrix;
    agg::conv_transform<agg::conv_curve<agg::path_storage>> m_path_scaling;
};

}
