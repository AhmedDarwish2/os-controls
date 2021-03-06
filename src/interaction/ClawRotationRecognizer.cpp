#include "stdafx.h"
#include "ClawRotationRecognizer.h"
#include <math.h>

ClawRotationRecognizer::ClawRotationRecognizer() :
  m_lastRot(0.0f,0.99f),
  m_deltaRot(0.0f,0.99f)
{
}

ClawRotationRecognizer::~ClawRotationRecognizer() {

}

void ClawRotationRecognizer::AutoFilter(const Leap::Hand& hand, const FrameTime& frameTime, ClawRotation& clawRotation) {
  Leap::Finger middle = Leap::Finger::invalid();
  Leap::Finger thumb = Leap::Finger::invalid();

  for( auto finger : hand.fingers()) {
    if ( finger.type() == Leap::Finger::TYPE_MIDDLE ) { middle = finger; }
    if ( finger.type() == Leap::Finger::TYPE_THUMB ) { thumb = finger; }
  }

  if(middle == Leap::Finger::invalid() ||
     thumb == Leap::Finger::invalid()) {
    return;
  }

  EigenTypes::Vector2 diff;
  if (hand.isRight()) {
    diff = ProjectVector(2, middle.tipPosition().toVector3<EigenTypes::Vector3>()) - ProjectVector(2, thumb.tipPosition().toVector3<EigenTypes::Vector3>());
  }
  else {
    diff = ProjectVector(2, thumb.tipPosition().toVector3<EigenTypes::Vector3>()) - ProjectVector(2, middle.tipPosition().toVector3<EigenTypes::Vector3>());
  }
  float angle = (float)atan2(diff.x(), diff.y());
  angle = (float)fmod(angle, 2*M_PI);
  //angle = 2*M_PI - angle;
  clawRotation.deltaTime = frameTime.deltaTime;
  m_deltaRot.SetGoal(angle - m_lastRot);

  // Need to keep track of what the last roll was, now
  m_lastRot.SetGoal(angle);

  m_deltaRot.Update(static_cast<float>(frameTime.deltaTime));
  m_lastRot.Update(static_cast<float>(frameTime.deltaTime));


  // Zeroize theta if we don't have a prior roll value
  if(!m_hasLast) {
    m_deltaRot.SetImmediate(0.0);
    m_hasLast = true;
  }

  clawRotation.deltaRotation = m_deltaRot.Value();
  clawRotation.absoluteRotation = m_lastRot;
}
