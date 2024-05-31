////////////////////////////////////////////////////////////////////
//
// Filename: seaPatchRoot.I
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

#define _USE_MATH_DEFINES
#include <math.h>

SeaPatchRoot::SeaPatchRoot()
{
    reset_environment();
    reset_properties();

    m_high_color = LVecBase4f(0.2, 0.4, 0.6, 1);
    m_low_color = LVecBase4f(0.2, 0.4, 0.6, 1);
    m_mid_color = LVecBase4f(0.2, 0.4, 0.6, 1);
}

SeaPatchRoot::~SeaPatchRoot()
{
}

void SeaPatchRoot::reset_environment()
{
    m_anchor = NodePath();
    m_center = NodePath();
    set_sea_level(0.0);
    enable();
    animate_height(true);
    animate_uv(true);
}

void SeaPatchRoot::reset_properties()
{
    m_speed = LVecBase2f(0, 0);
    set_radius(100);
    set_threshold(80);
    m_uv_scale = LVecBase2f(36, 36);
    m_uv_speed = LVecBase2f(0, 0);
    m_uv_offset = LVecBase2f(0, 0);
    m_attr_23 = LVecBase2f(0, 0);
    set_normal_damper(1.0);
    set_height_damper(0.5);
    m_amplitude_scale = 0.0;
}

void SeaPatchRoot::assign_environment_from(SeaPatchRoot* other)
{
    set_overall_speed(other->get_overall_speed());
    set_anchor(other->get_anchor());
    set_center(other->get_center());
    set_normal_damper(other->get_normal_damper());
    set_height_damper(other->get_height_damper());
    set_uv_speed(other->get_uv_speed());
    set_uv_scale(other->get_uv_scale());
    set_sea_level(other->get_sea_level());
    animate_height(other->get_animate_height());
    animate_uv(other->get_animate_uv());
    set_radius(other->get_radius());
    set_threshold(other->get_threshold());
}

void SeaPatchRoot::enable()
{
    m_enabled = true;
}

void SeaPatchRoot::disable()
{
    m_enabled = false;
}

bool SeaPatchRoot::is_enabled()
{
    return m_enabled;
}

void SeaPatchRoot::set_anchor(NodePath anchor)
{
    m_anchor = anchor;
}

NodePath& SeaPatchRoot::get_anchor()
{
    return m_anchor;
}

void SeaPatchRoot::set_center(NodePath center)
{
    m_center = center;
}

NodePath& SeaPatchRoot::get_center()
{
    return m_center;
}

void SeaPatchRoot::set_sea_level(float sea_level)
{
    m_sea_level = sea_level;
}

float SeaPatchRoot::get_sea_level()
{
    return m_sea_level;
}

void SeaPatchRoot::animate_height(bool animate_height)
{
    m_animate_height = animate_height;
}

bool SeaPatchRoot::get_animate_height()
{
    return m_animate_height;
}

void SeaPatchRoot::animate_uv(bool animate_uv)
{
    m_animate_uv = animate_uv;
}

bool SeaPatchRoot::get_animate_uv()
{
    return m_animate_uv;
}

float SeaPatchRoot::get_overall_speed()
{
    return m_speed[0];
}

LVecBase2f SeaPatchRoot::get_uv_speed()
{
    return m_uv_speed;
}

LVecBase2f SeaPatchRoot::get_uv_scale()
{
    return m_uv_scale;
}

void SeaPatchRoot::set_radius(float radius)
{
    m_radius = radius;
    m_radius2 = radius * radius;
}

float SeaPatchRoot::get_radius()
{
    return m_radius;
}

void SeaPatchRoot::set_threshold(float threshold)
{
    m_threshold = threshold;
    m_threshold2 = threshold * threshold;
}

float SeaPatchRoot::get_threshold()
{
    return m_threshold;
}

void SeaPatchRoot::set_low_color(LVecBase4f low_color)
{
    m_low_color = low_color;
    m_mid_color = (m_high_color + m_low_color) / 2.0;
}

LVecBase4f SeaPatchRoot::get_low_color()
{
    return m_low_color;
}

void SeaPatchRoot::set_high_color(LVecBase4f high_color)
{
    m_high_color = high_color;
    m_mid_color = (m_high_color + m_low_color) / 2.0;
}

LVecBase4f SeaPatchRoot::get_high_color()
{
    return m_high_color;
}

LVecBase4f SeaPatchRoot::get_mid_color()
{
    return m_mid_color;
}

int SeaPatchRoot::get_num_waves()
{
    return m_waves.size();
}

void SeaPatchRoot::clear_waves()
{
    m_waves.clear();
}

void SeaPatchRoot::enable_wave(int index)
{
    allocate_wave(index);
    m_waves[index].m_enabled = true;
    compute_amplitude_scale();
}

void SeaPatchRoot::disable_wave(int index)
{
    allocate_wave(index);
    m_waves[index].m_enabled = false;
    compute_amplitude_scale();
}

bool SeaPatchRoot::is_wave_enabled(int index)
{
    allocate_wave(index);
    return m_waves[index].m_enabled;
}

void SeaPatchRoot::set_wave_target(int index, SeaPatchRoot::WaveTarget target)
{
    allocate_wave(index);
    m_waves[index].m_target = target;
}

SeaPatchRoot::WaveTarget SeaPatchRoot::get_wave_target(int index)
{
    return m_waves[index].m_target;
}

void SeaPatchRoot::set_wave_func(int index, SeaPatchRoot::WaveFunction function)
{
    allocate_wave(index);
    m_waves[index].m_function = function;
}

SeaPatchRoot::WaveFunction SeaPatchRoot::get_wave_function(int index)
{
    return m_waves[index].m_function;
}

void SeaPatchRoot::set_choppy_k(int index, int choppy_k)
{
    allocate_wave(index);
    m_waves[index].m_choppy_k = choppy_k;
}

int SeaPatchRoot::get_choppy_k(int index)
{
    return m_waves[index].m_choppy_k;
}

void SeaPatchRoot::set_wave_length(int index, float length)
{
    allocate_wave(index);
    wave_t& wave = m_waves[index];
    wave.m_length = length;
    wave.m_direction_transformed = wave.m_direction * 2 * M_PI / wave.m_length;
}

float SeaPatchRoot::get_wave_length(int index)
{
    return m_waves[index].m_length;
}

void SeaPatchRoot::set_wave_direction(int index, LVecBase2f direction)
{
    allocate_wave(index);
    wave_t& wave = m_waves[index];
    wave.m_direction = direction;
    wave.m_direction_transformed = wave.m_direction * 2 * M_PI / wave.m_length;
}

LVecBase2f SeaPatchRoot::get_wave_direction(int index)
{
    return m_waves[index].m_direction;
}

void SeaPatchRoot::set_wave_speed(int index, float speed)
{
    allocate_wave(index);
    wave_t& wave = m_waves[index];
    float v6 = get_root_t();
    float v8 = wave.m_speed[0] * v6 + wave.m_speed[1];
    wave.m_speed = LVecBase2f(speed, v8 - v6 * speed);
}

float SeaPatchRoot::get_wave_speed(int index)
{
    return m_waves[index].m_speed[0];
}

void SeaPatchRoot::set_wave_amplitude(int index, float amplitude)
{
    allocate_wave(index);
    m_waves[index].m_amplitude = amplitude;
    compute_amplitude_scale();
}

float SeaPatchRoot::get_wave_amplitude(int index)
{
    return m_waves[index].m_amplitude;
}

void SeaPatchRoot::set_normal_damper(float normal_damper)
{
    m_normal_damper = normal_damper;
}

float SeaPatchRoot::get_normal_damper()
{
    return m_normal_damper;
}

void SeaPatchRoot::set_height_damper(float height_damper)
{
    m_height_damper = height_damper;
}

float SeaPatchRoot::get_height_damper()
{
    return m_height_damper;
}
