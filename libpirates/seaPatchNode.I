////////////////////////////////////////////////////////////////////
//
// Filename: seaPatchNode.I
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

SeaPatchNode::SeaPatchNode(const std::string& name, PT(SeaPatchRoot) root) : PandaNode(name),
  m_root(root),
  m_want_z(true),
  m_want_reflect(true),
  m_want_color(true),
  m_want_uv(true),
  m_want_normal(true),
  m_z_offset(0.0)
{
    set_cull_callback();
}

SeaPatchNode::~SeaPatchNode()
{
    m_root = NULL;
}

void SeaPatchNode::set_want_z(bool want_z)
{
    m_want_z = want_z;
}

bool SeaPatchNode::get_want_z()
{
    return m_want_z;
}

void SeaPatchNode::set_want_reflect(bool want_reflect)
{
    m_want_reflect = want_reflect;
}

bool SeaPatchNode::get_want_reflect()
{
    return m_want_reflect;
}

void SeaPatchNode::set_want_color(bool want_color)
{
    m_want_color = want_color;
}

bool SeaPatchNode::get_want_color()
{
    return m_want_color;
}

void SeaPatchNode::set_want_uv(bool want_uv)
{
    m_want_uv = want_uv;
}

bool SeaPatchNode::get_want_uv()
{
    return m_want_uv;
}

void SeaPatchNode::set_want_normal(bool want_normal)
{
    m_want_normal = want_normal;
}

bool SeaPatchNode::get_want_normal()
{
    return m_want_normal;
}

void SeaPatchNode::set_z_offset(float offset)
{
    m_z_offset = offset;
}

float SeaPatchNode::get_z_offset()
{
    return m_z_offset;
}

void SeaPatchNode::calc_reflection(LVecBase3f& normal, LPoint3f& d, LVecBase3f& result)
{
    float v12 = normal.dot(d);

    LPoint3f v3;
    LVecBase3f v4;

    v3 = normal * 2 * v12;
    v4 = d - v3;
    v4.normalize();

    result[0] = v4[0];
    result[1] = v4[1];
    result[2] = v4[2];
}
