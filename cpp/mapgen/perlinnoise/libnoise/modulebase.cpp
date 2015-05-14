// This file was taken from <http://libnoise.sourceforge.net/>,
// Copyright 2004-2005 libnoise Authors.
// It's licensed under the terms of the GPL 2.1 or later license.
// Modifications Copyright 2015-2015 the openage authors.
// See copying.md for further legal info.

#include "modulebase.h"

using namespace noise::module;

Module::Module (int sourceModuleCount)
{
  m_pSourceModule = NULL;

  // Create an array of pointers to all source modules required by this
  // noise module.  Set these pointers to NULL.
  if (sourceModuleCount > 0) {
    m_pSourceModule = new const Module*[sourceModuleCount];
    for (int i = 0; i < sourceModuleCount; i++) {
      m_pSourceModule[i] = NULL;
    }
  } else {
    m_pSourceModule = NULL;
  }
}

Module::~Module ()
{
  delete[] m_pSourceModule;
}
