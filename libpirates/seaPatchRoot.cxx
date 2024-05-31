////////////////////////////////////////////////////////////////////
//
// Filename: seaPatchRoot.cxx
// Created by:  TLOPO (21Apr17)
//
// The Legend of Pirates Online Software
// Copyright (c) The Legend of Pirates Online. All rights reserved.
//
// All use of this software is subject to the terms of the revised BSD
// license.  You should have received a copy of this license along
// with this source code in a file named "LICENSE."
//
////////////////////////////////////////////////////////////////////

#include "seaPatchRoot.h"
#include "clockObject.h"
#include "cullTraverser.h"
#include "cullTraverserData.h"

#include <math.h>

#define FRAME_TIME ((ClockObject::get_global_clock())->get_frame_time());
#define LOOP_WAVES for (auto& wave : m_waves)

TypeHandle SeaPatchRoot::_type_handle;

void SeaPatchRoot::set_overall_speed(float speed)
{
    float v3 = FRAME_TIME;
    float v4 = m_speed[0] * v3 + m_speed[1];
    m_speed = LPoint2f(speed, v4 - v3 * speed);
}

void SeaPatchRoot::set_uv_speed(LVecBase2f uv_speed)
{
    LVecBase2f v3, v4, v6;
    float v8 = FRAME_TIME;

    v3 = m_uv_speed;
    v4 = v3 * v8;
    v6 = v4 + m_uv_offset;

    m_uv_speed = uv_speed;
    m_uv_offset = v3 - m_uv_speed * v6[0];
}

void SeaPatchRoot::set_uv_scale(LVecBase2f uv_scale)
{
    LVecBase3f _scale = m_center.get_scale(m_anchor);
    LVecBase2f v6, v7, v9, v10, v11, v18, v20, v22;
    float v15 = FRAME_TIME;

    v18 = (_scale[0] / m_uv_scale[0], _scale[1] / m_uv_scale[1]);
    v6 = v18 * v15;
    v20 = v6 + m_uv_offset;

    v7 = v18 + v20;
    v22 = v7;

    v7 = v18 + v20;
    v22 = v7;

    m_uv_scale = uv_scale;

    v18 = LVecBase2f(_scale[0] / m_uv_scale[0], _scale[1] / m_uv_scale[1]);
    v9 = v22 - v18;
    v20 = v9;

    v10 = v20 * v15;
    v11 = v20 - v10;
    m_attr_23 = v11;
}

void SeaPatchRoot::allocate_wave(int index)
{
    while (get_num_waves() <= index)
    {
        wave_t wave;
        wave.m_enabled = false;
        wave.m_target = WTZ;
        wave.m_function = WFSin;
        wave.m_choppy_k = 1;
        wave.m_direction = LVecBase2f(0, 0);
        wave.m_direction_transformed = LVecBase2f(0, 0);
        wave.m_speed = LVecBase2f(0, 0);
        wave.m_amplitude = 1;
        wave.m_length = 0;
        m_waves.push_back(wave);
    }
}

void SeaPatchRoot::remove_wave(int index)
{
    m_waves.erase(m_waves.begin() + index);
    compute_amplitude_scale();
}

void SeaPatchRoot::compute_amplitude_scale()
{
    float v9 = 0;

    LOOP_WAVES
    {
        if (wave.m_enabled)
            v9 += wave.m_amplitude;
    }

    if (v9)
        m_amplitude_scale = 1.0 / (2 * v9);

    else
        m_amplitude_scale = 0.0;
}

LVecBase2f SeaPatchRoot::get_uv_offset()
{
    return m_uv_offset;
}

float SeaPatchRoot::get_root_t()
{
    float t = FRAME_TIME;
    return m_speed[0] * t + m_speed[1];
}

void SeaPatchRoot::add_flat_well(const std::string& a2, NodePath& a3, float a4,
                                 float a5, float a6, float a7)
{
    flat_well_t v8;
    float v9, v18, v19, v21, v23;

    LPoint3f v11(a4, a5, 0);
    v8.p = m_anchor.get_relative_point(a3, v11);
    v18 = m_anchor.get_sx(a3);

    v9 = std::max((float)0.0, a7);
    a6 = std::max((float)0.0, a6);

    v19 = a6 * v18;
    v23 = v18 * v9;
    v21 = v23 * v23;

    v8.a = v19 * v19;
    v8.b = v21;
    v8.c = v21 - v8.a;

    m_flat_wells[a2] = v8;
}

void SeaPatchRoot::remove_flat_well(const std::string& name)
{
    auto it = m_flat_wells.find(name);
    if (it != m_flat_wells.end())
        m_flat_wells.erase(it);
}

float SeaPatchRoot::calc_flat_well_scale(float a2, float a3)
{
    float v13 = 1.0;
    for (auto& it : m_flat_wells)
    {
        flat_well_t fw = it.second;
        LVecBase2f v7(a3 - fw.p[1], a2 - fw.p[0]);
        float v9 = v7.length_squared();

        if (fw.b < v9)
            continue;

        if (fw.a > v9)
            return 0;

        float v11 = (v9 - fw.a) / fw.c;
        v13 = std::min(v13, v11);
    }

    return v13;
}

float SeaPatchRoot::calc_height(float x, float y, float dist2)
{
    float result = get_sea_level();
    float v28 = calc_flat_well_scale(x, y);
    if (m_animate_height && m_radius2 > dist2 && v28)
    {
        float v27, v25;
        v27 = get_root_t();
        v25 = 0.0;

        LOOP_WAVES
        {
            if (!(wave.m_enabled && wave.m_target == WTZ))
                continue;

            float v26;
            if (wave.m_function == WFNoise)
            {
                LVecBase3f v30;
                v30[0] = x / wave.m_length;
                v30[1] = y / wave.m_length;
                v30[2] = wave.m_speed[0] * v27 + wave.m_speed[1];
                v26 = m_noise.noise(v30);
            }

            else
            {
                float v10, v12;
                v10 = wave.m_direction_transformed[0] * x + wave.m_direction_transformed[1] * y;
                v12 = wave.m_speed[0] * v27 + wave.m_speed[1];
                v26 = sin(v10 + v12) * 0.5 + 0.5;
            }

            if (wave.m_choppy_k > 1)
                v26 = pow(v26, wave.m_choppy_k);

            v25 += (v26 - 0.5) * wave.m_amplitude;
        }

        if (m_threshold2 < dist2)
        {
            float v22, v23;
            v22 = sqrt(dist2 + 0.001);
            v23 = v22 - m_threshold;
            v25 -= v23 * v25 / (m_radius - m_threshold);
        }

        result += v25 * v28;
    }

    return result;
}

float SeaPatchRoot::calc_filtered_height(float x, float y, float min_length, float dist2)
{
    float result = get_sea_level();
    float v28 = calc_flat_well_scale(x, y);
    if (m_animate_height && m_radius2 > dist2 && v28)
    {
        float v27, v25;
        v27 = get_root_t();
        v25 = 0.0;

        LOOP_WAVES
        {
            if (!(wave.m_enabled &&
                  wave.m_target == WTZ &&
                  wave.m_function == WFSin &&
                  wave.m_length > min_length))
                continue;

            float v26, v10, v12;
            v10 = wave.m_direction_transformed[0] * x + wave.m_direction_transformed[1] * y;
            v12 = wave.m_speed[0] * v27 + wave.m_speed[1];
            v26 = sin(v10 + v12) * 0.5 + 0.5;

            if (wave.m_choppy_k > 1)
                v26 = pow(v26, wave.m_choppy_k);

            v25 += (v26 - 0.5) * wave.m_amplitude;
        }

        if (m_threshold2 < dist2)
        {
            float v22, v23;
            v22 = sqrt(dist2 + 0.001);
            v23 = v22 - m_threshold;
            v25 -= v23 * v25 / (m_radius - m_threshold);
        }

        result += v25 * v28;
    }

    return result;
}

float SeaPatchRoot::calc_height_for_mass(float x, float y,
                                         float dist2, float mass,
                                         float area)
{
    return calc_height(x, y, dist2) * m_height_damper - (mass / area);
}

LVecBase3f SeaPatchRoot::calc_normal(float height, float x, float y, float dist2)
{
    float v7, v8, v9, v10, v11, v38;
    LVecBase3f ret;

    v7 = y + .2;
    v8 = x - .3;
    v10 = y - .1;
    v11 = x - .3;

    v9 = calc_height(v8, v7, dist2);
    v38 = calc_height(v11, v10, dist2);

    LVecBase3f v21, v24, v18, v12, v13;
    v21 = LVecBase3f(v8, v7, v9);
    v24 = LVecBase3f(v11, v10, v38);
    v18 = LVecBase3f(x, y, height);

    v12 = v21 - v18;
    v13 = v24 - v18;
    ret = v12.cross(v13);
    ret.normalize();
    return ret;
}

LVecBase3f SeaPatchRoot::calc_normal_for_mass(float height, float x, float y, float dist2, float mass, float area)
{
    float v7, v8, v9, v10, v11, v38;
    LVecBase3f ret;

    v7 = y + .2;
    v8 = x - .3;
    v10 = y - .1;
    v11 = x - .3;

    v9 = calc_height_for_mass(v8, v7, dist2, mass, area);
    v38 = calc_height_for_mass(v11, v10, dist2, mass, area);

    LVecBase3f v21, v24, v18, v12, v13;
    v21 = LVecBase3f(v8, v7, v9 - (v9 - height) * m_normal_damper);
    v24 = LVecBase3f(v11, v10, v38 - (v38 - height) * m_normal_damper);
    v18 = LVecBase3f(x, y, height);

    v12 = v21 - v18;
    v13 = v24 - v18;
    ret = v13.cross(v12);
    ret.normalize();
    return ret;
}

void SeaPatchRoot::calc_uv(LPoint2f& a2, float a3, float a4, float a5)
{
    LPoint2f result, v25;
    v25 = LPoint2f(a3 / m_uv_scale[0], a4 / m_uv_scale[1]);
    result = v25 + get_uv_offset();

    if (m_animate_uv && m_radius2 > a5)
    {
        float v24 = get_root_t();

        LOOP_WAVES
        {
            if (wave.m_enabled && (wave.m_target == WTU || wave.m_target == WTV))
            {
                float v23;
                if (wave.m_function == WFNoise)
                {
                    LPoint3f v31;
                    v31[0] = a3 / wave.m_length;
                    v31[1] = a4 / wave.m_length;
                    v31[2] = wave.m_speed[0] * v24 + wave.m_speed[1];
                    v23 = m_noise.noise(v31);
                }

                else
                {
                    float v10, v12;
                    v10 = wave.m_direction_transformed[0] * a3 + wave.m_direction_transformed[1] * a4;
                    v12 = wave.m_speed[0] * v24 + wave.m_speed[1];
                    v23 = sin(v10 + v12) * 0.5 + 0.5;
                }

                float v21 = wave.m_amplitude * v23;
                if (wave.m_target == WTU)
                    result[0] += v21;

                else
                    result[1] += v21;
            }
        }
    }

    a2[0] = result[0];
    a2[1] = result[1];
}

LVecBase4f SeaPatchRoot::calc_color(float a3, float a4, float a5)
{
    float v5, v9;

    v9 = (a3 - m_sea_level) * m_amplitude_scale + 0.5;
    if (v9 <= 0.0)
        v9 = 0.0;

    else if (v9 >= 1.0)
        v9 = 1.0;

    return m_low_color + (m_high_color - m_low_color) * (1.0 - v9);
}
