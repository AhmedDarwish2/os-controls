#pragma once
#include "interaction/HandLocationRecognizer.h"
#include "interaction/HandRollRecognizer.h"
#include "interaction/FrameDeltaTimeRecognizer.h"
#include "interaction/MediaViewController.h"
#include "uievents/MediaViewEventListener.h"
#include "uievents/OSCDomain.h"
#include "autowiring/Autowiring.h"
#include "RenderEngine.h"
#include "RenderEngineNode.h"
#include <RadialMenu.h>

class MediaViewStateMachine :
public RenderEngineNode
{
public:
  MediaViewStateMachine();
  virtual ~MediaViewStateMachine() {};
  
  void AutoInit();
  
  //All user and state machine driven changes to the view are dealt with from here.
  void AutoFilter(OSCState appState, const HandLocation& handLocation, const DeltaRollAmount& dHandRoll, const FrameTime& frameTime);
  
  void AnimationUpdate(const RenderFrame& renderFrame) override;
  void Render(const RenderFrame& renderFrame) const override;
  
  
private:
  void resolveSelection(int selectedID);
  //Adjust the view for the volume control
  float calculateVolumeDelta(float deltaHandRoll);
  
  
  /// <summary>
  /// State for the wedge network on the media view control
  /// </summary>
  
  enum class State {
    
    /*                        |----------V
     *    --> Inactive --> Active --> SelectionMade
     *           ^-----------|-----------|
     */
    
    //Media View is created but not focused.
    INACTIVE,
    
    //Taking user input, fading in, etc
    ACTIVE,
    
    //Done taking input, has sent its event up the chain. Mostly for finished animations.
    SELECTION_MADE,
    
    //Wait for animation to fade out
    FADE_OUT,
    
    //Tear everything down.
    FINAL
  };
  
  RadialMenu m_radialMenu;
  
  State m_state;
  
  //Autowired<MediaView> m_mediaView;
  
  // Events fired by this MediaView
  AutoFired<MediaViewEventListener> m_mediaViewEventListener;
  
  AutoRequired<MediaViewController> m_mediaViewController;
  //std::shared_ptr<Wedge> m_lastActiveWedge;
  Autowired<RootRenderEngineNode> m_rootNode;
  RectanglePrim prim;
};