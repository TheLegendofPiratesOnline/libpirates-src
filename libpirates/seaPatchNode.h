////////////////////////////////////////////////////////////////////
//
// Filename: seaPatchNode.h
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

#ifndef __SEA_PATCH_NODE_H__
#define __SEA_PATCH_NODE_H__

#include "pandabase.h"

#include "seaPatchRoot.h"

#include "pandaNode.h"
#include "nodePath.h"
#include "geomVertexData.h"
#include "pmutex.h"
#include "geom.h"

class EXPCL_PANDA_PIRATES SeaPatchNode : public PandaNode {
    PUBLISHED:
        INLINE SeaPatchNode(const std::string& name, PT(SeaPatchRoot) root);
        INLINE virtual ~SeaPatchNode();

        INLINE void set_want_z(bool want_z);
        INLINE bool get_want_z();
        INLINE void set_want_reflect(bool want_reflect);
        INLINE bool get_want_reflect();
        INLINE void set_want_color(bool want_color);
        INLINE bool get_want_color();
        INLINE void set_want_uv(bool want_uv);
        INLINE bool get_want_uv();
        INLINE void set_want_normal(bool want_normal);
        INLINE bool get_want_normal();

        INLINE void set_z_offset(float offset);
        INLINE float get_z_offset();

        void collect_geometry();

    public:
        virtual bool has_cull_callback() const;
        virtual bool safe_to_transform() const;
        virtual bool cull_callback(CullTraverser *trav, CullTraverserData &data);

        INLINE void calc_reflection(LVecBase3f& normal, LPoint3f& d, LVecBase3f& result);

    private:
          void r_collect_geometry(PandaNode* node, bool flat);
          void save_vertex_data(GeomVertexData* data);
          void do_flat(GeomVertexData* geom, const LMatrix4f& anchor_mat,
                       const LMatrix4f& center_mat,
                       const LMatrix4f& anchor_rel_mat,
                       Thread* current_thread);
          void do_wave(GeomVertexData* geom, const LMatrix4f& anchor_mat,
                       const LMatrix4f& center_mat,
                       const LMatrix4f& anchor_rel_mat,
                       Thread* current_thread);

          PT(SeaPatchRoot) m_root;

          bool m_want_z;
          bool m_want_reflect;
          bool m_want_color;
          bool m_want_uv;
          bool m_want_normal;

          float m_z_offset;
          float m_last_update;

          Mutex m_lock;

          pvector<GeomVertexData*> m_geoms;
          pvector<GeomVertexData*> m_flat_geoms;

    public:
        static TypeHandle get_class_type() {
            return _type_handle;
        }
        static void init_type() {
            PandaNode::init_type();
            register_type(_type_handle, "SeaPatchNode", PandaNode::get_class_type());
        }
        virtual TypeHandle get_type() const {
            return get_class_type();
        }
        virtual TypeHandle force_init_type() {
            init_type();
            return get_class_type();
        }

    private:
        static TypeHandle _type_handle;
};

#include "seaPatchNode.I"

#endif /* __SEA_PATCH_NODE_H__ */
