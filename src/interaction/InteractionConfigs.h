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
  const Vector3 m_leapMin(-120.0f,340.0f,-30.0f);
  const Vector3 m_leapMax(120.0f,550.0f,30.0f);
}
namespace activationConfigs {
  // Pinch and Grab Constants
  const float MIN_GRAB_START = 0.7f;
  const float MIN_GRAB_CONTINUE = 0.6f;
  const float MIN_PINCH_START = 0.9f;
  const float MIN_PINCH_CONTINUE = 0.8f;
}

namespace loactionConfigs {
  // Hand Location Constants
  const float INDEX_DIRECTION_OFFSET_DISTANCE = 100.0f;
}

namespace pointingConfigs {
  const float MAX_BEND_FOR_START_POINTING = 1.6f;
  const float MAX_BEND_FOR_CONTINUE_POINTING = 1.8f;
  const float MAX_BEND_FOR_THUMB_START_POINTING = 1.5f;
  const float MAX_BEND_FOR_THUMB_END_POINTING = 1.7f;
}