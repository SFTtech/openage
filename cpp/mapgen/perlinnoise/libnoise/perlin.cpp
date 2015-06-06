// This file was taken from <http://libnoise.sourceforge.net/>,
// Copyright 2004-2005 libnoise Authors.
// It's licensed under the terms of the GPL 2.1 or later license.
// Modifications Copyright 2015-2015 the openage authors.
// See copying.md for further legal info.

#include "perlin.h"

using namespace noise::module;

Perlin::Perlin ():
  Module (GetSourceModuleCount ()),
  m_frequency    (DEFAULT_PERLIN_FREQUENCY   ),
  m_lacunarity   (DEFAULT_PERLIN_LACUNARITY  ),
  m_noiseQuality (DEFAULT_PERLIN_QUALITY     ),
  m_octaveCount  (DEFAULT_PERLIN_OCTAVE_COUNT),
  m_persistence  (DEFAULT_PERLIN_PERSISTENCE ),
  m_seed         (DEFAULT_PERLIN_SEED)
{
}

double Perlin::GetValue (double x, double y, double z) const
{
  double value = 0.0;
  double signal = 0.0;
  double curPersistence = 1.0;
  double nx, ny, nz;
  int seed;

  x *= m_frequency;
  y *= m_frequency;
  z *= m_frequency;

  for (int curOctave = 0; curOctave < m_octaveCount; curOctave++) {

    // Make sure that these floating-point values have the same range as a 32-
    // bit integer so that we can pass them to the coherent-noise functions.
    nx = MakeInt32Range (x);
    ny = MakeInt32Range (y);
    nz = MakeInt32Range (z);

    // Get the coherent-noise value from the input value and add it to the
    // final result.
    seed = (m_seed + curOctave) & 0xffffffff;
    signal = GradientCoherentNoise3D (nx, ny, nz, seed, m_noiseQuality);
    value += signal * curPersistence;

    // Prepare the next octave.
    x *= m_lacunarity;
    y *= m_lacunarity;
    z *= m_lacunarity;
    curPersistence *= m_persistence;
  }

  return value;
}
