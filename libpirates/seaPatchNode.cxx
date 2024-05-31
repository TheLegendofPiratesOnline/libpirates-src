////////////////////////////////////////////////////////////////////
//
// Filename: seaPatchNode.cxx
// Created by:  TLOPO (20Apr17)
//
// The Legend of Pirates Online Software
// Copyright (c) The Legend of Pirates Online. All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "seaPatchNode.h"

#include "geomNode.h"
#include "geomVertexRewriter.h"
#include "cullTraverser.h"
#include "cullTraverserData.h"
#include "clockObject.h"

TypeHandle SeaPatchNode::_type_handle;

bool SeaPatchNode::has_cull_callback() const
{
    return true;
}

bool SeaPatchNode::safe_to_transform() const
{
    return false;
}

bool SeaPatchNode::cull_callback(CullTraverser *trav, CullTraverserData &data)
{
    MutexHolder holder(m_lock);

    if (!(m_root && m_root->is_enabled()))
        return true;

    float time = ClockObject::get_global_clock()->get_frame_time();
    if (time == m_last_update)
        return true;

    m_last_update = time;

    NodePath np = trav->get_scene()->get_cull_center();
    if (np.is_empty())
        np = trav->get_scene()->get_camera_path();

    if (np.is_empty() || np.get_error_type() != NodePath::ET_ok)
    {
        pirates_cat.error() << "invalid np:" << np << std::endl;
        pirates_cat.error() << "  empty: " << np.is_empty() << std::endl;
        pirates_cat.error() << "  error_type: " << np.get_error_type() << std::endl;
        return false;
    }

    Thread* current_thread = trav->get_current_thread();
    CPT(TransformState) transform = data.get_node_path().get_net_transform(current_thread);

    CPT(TransformState) anchor_trans = m_root->get_anchor().get_net_transform(current_thread);
    anchor_trans = anchor_trans->invert_compose(transform);
    LMatrix4f anchor_mat = anchor_trans->get_mat();

    CPT(TransformState) center_trans = m_root->get_center().get_net_transform(current_thread);
    center_trans = center_trans->invert_compose(transform);
    LMatrix4f center_mat = center_trans->get_mat();

    CPT(TransformState) anchor_rel_trans = m_root->get_anchor().get_transform(np, current_thread);
    LMatrix4f anchor_rel_mat = anchor_rel_trans->get_mat();

    for (pvector<GeomVertexData*>::iterator it = m_flat_geoms.begin();
         it != m_flat_geoms.end(); ++it)
          do_flat(*it, anchor_mat, center_mat, anchor_rel_mat, current_thread);

    for (pvector<GeomVertexData*>::iterator it = m_geoms.begin();
         it != m_geoms.end(); ++it)
          do_wave(*it, anchor_mat, center_mat, anchor_rel_mat, current_thread);

    return true;
}

void SeaPatchNode::collect_geometry()
{
    m_geoms.clear();
    m_flat_geoms.clear();

    r_collect_geometry(this, false);

    for (pvector<GeomVertexData*>::iterator it = m_flat_geoms.begin();
         it != m_flat_geoms.end(); ++it)
          save_vertex_data(*it);

    for (pvector<GeomVertexData*>::iterator it = m_geoms.begin();
         it != m_geoms.end(); ++it)
          save_vertex_data(*it);
}

void SeaPatchNode::r_collect_geometry(PandaNode* node, bool flat)
{
    if (!flat)
        flat = node->get_tag("flat_sea") == "true";

    if (node->is_of_type(GeomNode::get_class_type()))
    {
        GeomNode* geom_node = DCAST(GeomNode, node);
        int num_geoms = geom_node->get_num_geoms();
        for (int i = 0; i < num_geoms; i++)
        {
            PT(Geom) geom = geom_node->modify_geom(i);
            PT(GeomVertexData) data = geom->modify_vertex_data();

            if (flat)
                m_flat_geoms.push_back(data);

            else
                m_geoms.push_back(data);
        }
    }

    int num_children = node->get_num_children();
    for (int i = 0; i < num_children; i++)
        r_collect_geometry(node->get_child(i), flat);
}

void SeaPatchNode::save_vertex_data(GeomVertexData* data)
{
    PT(GeomVertexFormat) new_format = new GeomVertexFormat();

    PT(GeomVertexArrayFormat) array;
    array = new GeomVertexArrayFormat();
    array->add_column(InternalName::get_color(), 4, Geom::NT_float32, Geom::C_color);
    array->add_column(InternalName::get_vertex(), 3, Geom::NT_float32, Geom::C_point);
    array->add_column(InternalName::get_normal(), 3, Geom::NT_float32, Geom::C_vector);
    array->add_column(InternalName::get_texcoord(), 2, Geom::NT_float32, Geom::C_texcoord);
    new_format->add_array(array);

    CPT(GeomVertexFormat) registered_format;
    registered_format = GeomVertexFormat::register_format(new_format);
    data->set_format(registered_format);
}

void SeaPatchNode::do_flat(GeomVertexData* geom, const LMatrix4f& anchor_mat,
                           const LMatrix4f& center_mat,
                           const LMatrix4f& anchor_rel_mat,
                           Thread* current_thread)
{
    GeomVertexWriter uv(geom, "texcoord", current_thread);
    GeomVertexWriter normal(geom, "normal", current_thread);
    GeomVertexWriter reflect(geom, "texcoord.reflect", current_thread);
    GeomVertexWriter color(geom, "color", current_thread);
    GeomVertexRewriter reader(geom, "vertex", current_thread);

    while (!reader.is_at_end())
    {
        LPoint3f v226, v283;
        v226 = reader.get_data3f();
        v283 = anchor_mat.xform_point(v226);

        if (m_want_z)
        {
            float z = m_z_offset + m_root->get_sea_level();
            v226[2] = z;
            reader.set_data3f(v226);
        }

        if (m_want_normal)
        {
            normal.set_data3f(LPoint3f(0, 0, 1));
        }

        if (m_want_reflect)
        {
            LPoint3f v98;
            LVecBase3f v99, v100;
            v98 = anchor_rel_mat.xform_point(v283);
            v99 = anchor_rel_mat.xform_vec(LVecBase3f(0, 0, 1));
            calc_reflection(v99, v98, v100);
            reflect.set_data2f(v100[0] * 0.5 + 0.5, 0.5 - v100[2] * 0.5);
        }

        if (m_want_color)
        {
            color.set_data4f(m_root->get_mid_color());
        }

        if (m_want_uv)
        {
            LPoint3f v294;
            v294 = center_mat.xform_point(v226);
            float v126 = v294.length_squared();

            LPoint2f v291;
            m_root->calc_uv(v291, v283[0], v283[1], v126);
            uv.set_data2f(v291);
        }
    }
}

void SeaPatchNode::do_wave(GeomVertexData* geom, const LMatrix4f& anchor_mat,
                           const LMatrix4f& center_mat,
                           const LMatrix4f& anchor_rel_mat,
                           Thread* current_thread)
{
    GeomVertexWriter uv(geom, "texcoord", current_thread);
    GeomVertexWriter normal(geom, "normal", current_thread);
    GeomVertexWriter reflect(geom, "texcoord.reflect", current_thread);
    GeomVertexWriter color(geom, "color", current_thread);
    GeomVertexRewriter reader(geom, "vertex", current_thread);

    while (!reader.is_at_end())
    {
        LPoint3f v244, v148, v310;
        v244 = reader.get_data3f();
        v148 = anchor_mat.xform_point(v244);
        v310 = center_mat.xform_point(v244);

        float v145, v108, v143;
        v145 = v310.length_squared();
        v108 = v143 = m_root->calc_height(v148[0], v148[1], v145);

        if (m_want_z)
        {
            float z = m_z_offset + v143;
            v244[2] = z;
            reader.set_data3f(v244);
        }

        if (m_want_normal || m_want_reflect)
        {
            LVecBase3f v267 = m_root->calc_normal(v143, v148[0], v148[1], v145);
            if (m_want_normal)
                normal.set_data3f(v267);

            if (m_want_reflect)
            {
                LPoint3f v113;
                LVecBase3f v114, v151;
                v113 = anchor_rel_mat.xform_point(v148);
                v114 = anchor_rel_mat.xform_vec(v267);
                calc_reflection(v114, v113, v151);
                reflect.set_data2f(v151[0] * 0.5 + 0.5, 0.5 - v151[2] * 0.5);
            }
        }

        if (m_want_color)
        {
            color.set_data4f(m_root->calc_color(v143, v148[0], v148[1]));
        }

        if (m_want_uv)
        {
            LPoint2f v312;
            m_root->calc_uv(v312, v148[0], v148[1], v145);
            uv.set_data2f(v312);
        }
    }
}
