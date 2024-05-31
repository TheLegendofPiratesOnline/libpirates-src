////////////////////////////////////////////////////////////////////
//
// Filename: seaPatchRoot.h
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

#ifndef __SEA_PATCH_ROOT_H__
#define __SEA_PATCH_ROOT_H__

#include "pandabase.h"

#include "nodePath.h"
#include "perlinNoise3.h"
#include "typedReferenceCount.h"

class EXPCL_PANDA_PIRATES SeaPatchRoot : public TypedReferenceCount {
    PUBLISHED:
        enum WaveTarget {
            WTZ,
            WTU,
            WTV
        };

        enum WaveFunction {
            WFSin,
            WFNoise
        };

        INLINE SeaPatchRoot();
        INLINE virtual ~SeaPatchRoot();

        INLINE void reset_environment();
        INLINE void reset_properties();
        INLINE void assign_environment_from(SeaPatchRoot* other);

        INLINE void enable();
        INLINE void disable();
        INLINE bool is_enabled();

        INLINE void set_anchor(NodePath anchor);
        INLINE NodePath& get_anchor();
        INLINE void set_center(NodePath center);
        INLINE NodePath& get_center();

        INLINE void set_sea_level(float sea_level);
        INLINE float get_sea_level();

        INLINE void animate_height(bool animate_height);
        INLINE bool get_animate_height();
        INLINE void animate_uv(bool animate_uv);
        INLINE bool get_animate_uv();

        void set_overall_speed(float speed);
        INLINE float get_overall_speed();

        void set_uv_speed(LVecBase2f uv_speed);
        INLINE LVecBase2f get_uv_speed();
        void set_uv_scale(LVecBase2f uv_scale);
        INLINE LVecBase2f get_uv_scale();
        LVecBase2f get_uv_offset();

        INLINE void set_radius(float radius);
        INLINE float get_radius();
        INLINE void set_threshold(float threshold);
        INLINE float get_threshold();

        INLINE void set_low_color(LVecBase4f low_color);
        INLINE LVecBase4f get_low_color();
        INLINE void set_high_color(LVecBase4f high_color);
        INLINE LVecBase4f get_high_color();
        INLINE LVecBase4f get_mid_color();

        INLINE int get_num_waves();
        INLINE void clear_waves();
        void allocate_wave(int index);
        void remove_wave(int index);
        INLINE void enable_wave(int index);
        INLINE void disable_wave(int index);
        INLINE bool is_wave_enabled(int index);
        INLINE void set_wave_target(int index, SeaPatchRoot::WaveTarget target);
        INLINE SeaPatchRoot::WaveTarget get_wave_target(int index);
        INLINE void set_wave_func(int index, SeaPatchRoot::WaveFunction function);
        INLINE SeaPatchRoot::WaveFunction get_wave_function(int index);
        INLINE void set_choppy_k(int index, int choppy_k);
        INLINE int get_choppy_k(int index);
        INLINE void set_wave_length(int index, float length);
        INLINE float get_wave_length(int index);
        INLINE void set_wave_direction(int index, LVecBase2f direction);
        INLINE LVecBase2f get_wave_direction(int index);
        INLINE void set_wave_speed(int index, float speed);
        INLINE float get_wave_speed(int index);
        INLINE void set_wave_amplitude(int index, float amplitude);
        INLINE float get_wave_amplitude(int index);

        INLINE void set_normal_damper(float normal_damper);
        INLINE float get_normal_damper();
        INLINE void set_height_damper(float height_damper);
        INLINE float get_height_damper();

        void add_flat_well(const std::string& a2, NodePath& a3, float a4,
                           float a5, float a6, float a7);
        void remove_flat_well(const std::string& name);
        float calc_flat_well_scale(float a2, float a3);

        float calc_height(float x, float y, float dist2);
        float calc_filtered_height(float x, float y, float min_length, float dist2);
        float calc_height_for_mass(float x, float y, float dist2, float mass, float area);

        LVecBase3f calc_normal(float height, float x, float y, float dist2);
        LVecBase3f calc_normal_for_mass(float height, float x, float y, float dist2, float mass, float area);

        void calc_uv(LPoint2f& a2, float a3, float a4, float a5);
        LVecBase4f calc_color(float a3, float a4, float a5);

    private:
        void compute_amplitude_scale();
        float get_root_t();

        bool m_enabled;
        bool m_animate_height;
        bool m_animate_uv;

        float m_sea_level;
        LVecBase2f m_speed;
        float m_radius;
        float m_radius2;
        float m_threshold;
        float m_threshold2;
        float m_normal_damper;
        float m_height_damper;
        float m_amplitude_scale;

        LVecBase2f m_uv_scale;
        LVecBase2f m_uv_speed;
        LVecBase2f m_uv_offset;
        LVecBase2f m_attr_23;

        NodePath m_anchor;
        NodePath m_center;

        LVecBase4f m_low_color;
        LVecBase4f m_mid_color;
        LVecBase4f m_high_color;

        PerlinNoise3 m_noise;

        typedef struct {
              bool m_enabled;
              WaveTarget m_target;
              WaveFunction m_function;
              int m_choppy_k;
              LVecBase2f m_direction;
              LVecBase2f m_direction_transformed;
              LVecBase2f m_speed;
              float m_amplitude;
              float m_length;
        } wave_t;
        std::vector<wave_t> m_waves;

        typedef struct {
              LPoint3f p;
              float a;
              float b;
              float c;
        } flat_well_t;
        std::map<std::string, flat_well_t> m_flat_wells;

    public:
        static TypeHandle get_class_type() {
            return _type_handle;
        }
        static void init_type() {
            TypedReferenceCount::init_type();
            register_type(_type_handle, "SeaPatchRoot", TypedReferenceCount::get_class_type());
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

#include "seaPatchRoot.I"

#endif /* __SEA_PATCH_ROOT_H__ */
