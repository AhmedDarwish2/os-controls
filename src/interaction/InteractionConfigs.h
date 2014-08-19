//
//  InteractionConfigs.h
//  oscontrols
//
//  Created by Daniel Plemmons on 8/18/14.
//
//

#pragma once
#include "EigenTypes.h"

namespace config {
  const Vector3 m_leapMin(-120.0f,200.0f,-30.0f);
  const Vector3 m_leapMax(120.0f,600.0f,30.0f);
  const float MIN_DOT_FOR_POINTING = 0.75f;
  const float MAX_DOT_FOR_CURLED = 0.1f;
  const float FINGER_OFFSET_DISTANCE = 3.0f;
}