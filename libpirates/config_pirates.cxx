////////////////////////////////////////////////////////////////////
//
// Filename: config_pirates.cxx
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
#include "seaPatchRoot.h"

#include "config_pirates.h"

NotifyCategoryDef(pirates, "");

ConfigureDef(config_pirates)
ConfigureFn(config_pirates) {
  SeaPatchNode::init_type();
  SeaPatchRoot::init_type();
}
