#import "ApplicationDelegate.h"
#import "utility/Config.h"

#import <autowiring/autowiring.h>
#import <ServiceManagement/ServiceManagement.h>

@implementation ApplicationDelegate

@synthesize menubarController = _menubarController;

- (id)init
{
  self = [super init];
  if (self != nil) {
    // Defer initialization until applicationDidFinishLaunching:
    _menubarController = nil;
    _isInitialized = NO;
    _wasHidden = NO;
  }
  return self;
}

- (void)dealloc
{
  [super dealloc];
}

- (void)applicationDidFinishLaunching:(NSNotification*)aNotification
{
  if (_isInitialized) {
    return;
  }
  _isInitialized = YES;

  // Check and possibly update the link to the Shortcuts preference pane
  @autoreleasepool {
    NSArray* paths = NSSearchPathForDirectoriesInDomains(NSPreferencePanesDirectory, NSUserDomainMask, YES);
    NSString* userPrefPanePath = [paths objectAtIndex:0];
    // If we can't get the directory to the user's preference pane directory, give up
    if (userPrefPanePath == nil) {
      return;
    }
    userPrefPanePath = [userPrefPanePath stringByAppendingString:@"/Shortcuts.prefPane"];

    NSBundle* bundle = [NSBundle mainBundle];
    NSString* shortcutsPrefPanePath = [bundle bundlePath];
    // If we can't get the path to this app bundle, give up
    if (shortcutsPrefPanePath == nil) {
      return;
    }
    shortcutsPrefPanePath = [shortcutsPrefPanePath stringByAppendingString:@"/Contents/MacOS/Shortcuts.prefPane"];
    NSString* shortcutsPrefPanePathExec =
      [shortcutsPrefPanePath stringByAppendingString:@"/Contents/MacOS/ShortcutsPreferences"];

    NSFileManager* fileManager = [NSFileManager defaultManager];
    // If we can't find the Shortcuts preferences executable, give up
    if (![fileManager fileExistsAtPath:shortcutsPrefPanePathExec]) {
      return;
    }

    // See whether or not the Shortcuts preference pane is pointing to the expected instance
    NSString* symbolicLinkPath = [fileManager destinationOfSymbolicLinkAtPath:userPrefPanePath error:nil];
    if (symbolicLinkPath && [shortcutsPrefPanePath compare:symbolicLinkPath] == NSOrderedSame) {
      return;
    }
    // Try to remove the old link (if it exists at all)
    [fileManager removeItemAtPath:userPrefPanePath error:nil];
    // Create a symbolic link to our Shortcuts preference pane app in the user's preference pane directory
    [fileManager createSymbolicLinkAtPath:userPrefPanePath withDestinationPath:shortcutsPrefPanePath error:nil];
  }
}

- (void)validateAccessibility
{
  if (!AXIsProcessTrustedWithOptions) {
    return; // Don't force on Accessibilty pre-OS X 10.9
  }
  AutowiredFast<Config> cfg;
  int showAuthCount = cfg->GetOrCreate("showAuthCount", 0);

  if (AXIsProcessTrusted()) {
    if (showAuthCount > 0) {
      cfg->Set("showAuthCount", 0);
    }
  } else if (showAuthCount < 3) {
    @autoreleasepool {
      NSBundle* bundle = [NSBundle mainBundle];
      NSString* shortcutsHelperPath = [bundle bundlePath];
      if (shortcutsHelperPath == nil) {
        return;
      }
      shortcutsHelperPath = [shortcutsHelperPath stringByAppendingString:@"/Contents/MacOS/ShortcutsHelper"];
      AuthorizationRef authorizationRef;
      OSStatus status = AuthorizationCreate(nullptr, kAuthorizationEmptyEnvironment,
                                            kAuthorizationFlagDefaults, &authorizationRef);
      if (status == errAuthorizationSuccess) {
        AuthorizationItem right = {kAuthorizationRightExecute, 0, nullptr, 0};
        AuthorizationRights rights = {1, &right};
        AuthorizationFlags flags = kAuthorizationFlagDefaults |
                                   kAuthorizationFlagInteractionAllowed |
                                   kAuthorizationFlagPreAuthorize |
                                   kAuthorizationFlagExtendRights;

        // Show our application icon in the Authorization popup
        const char* prompt = "Shortcuts would like to control this computer using accessibilty features.";
        NSString* src =
          [[[NSBundle mainBundle] bundlePath] stringByAppendingString:@"/Contents/MacOS/Shortcuts.prefPane/Contents/Resources/ShortcutsPreferences.tiff"];
        NSString *dst = [NSString stringWithFormat:@"/tmp/ShortcutsAuth-%@.tiff", [[NSProcessInfo processInfo] globallyUniqueString]];
        [[NSFileManager defaultManager] copyItemAtPath:src toPath:dst error:nil];
        const char* iconPath = [dst UTF8String];
        AuthorizationItem authItems[2];
        UInt32 numAuthItems = 0;

        authItems[numAuthItems++] = {kAuthorizationEnvironmentPrompt, std::strlen(prompt), (void*)prompt, 0};
        if (iconPath) {
          authItems[numAuthItems++] = {kAuthorizationEnvironmentIcon, std::strlen(iconPath), (void*)iconPath, 0};
        };
        AuthorizationEnvironment authEnvironment = {numAuthItems, authItems};

        // Bring application to front so that the Authorization window has focus
        [[NSApplication sharedApplication] activateIgnoringOtherApps:YES];

        status = AuthorizationCopyRights(authorizationRef, &rights, &authEnvironment, flags, nullptr);
        if (status == errAuthorizationSuccess) {
          NSString* label = @"com.leapmotion.ShortcutsHelper";
          NSDictionary* job = @{@"Label":label, @"Program":shortcutsHelperPath, @"RunAtLoad":@YES};

          SMJobRemove(kSMDomainSystemLaunchd, (CFStringRef)label, authorizationRef, false, nullptr);
          SMJobSubmit(kSMDomainSystemLaunchd, (CFDictionaryRef)job, authorizationRef, nullptr);
        } else {
          // Tell the user that they need to update the System Preferences themselves -- FIXME
        }
        AuthorizationFree(authorizationRef, kAuthorizationFlagDestroyRights);
        [[NSFileManager defaultManager] removeItemAtPath:dst error:nil];
      }
    }
    cfg->Set("showAuthCount", ++showAuthCount);
  }
}

- (void)addStatusItem
{
  if (_menubarController) {
    return;
  }

  // Validate that Accessibility is enabled for this application
  [self validateAccessibility];

  // Create the status item
  _menubarController = [[MenubarController alloc] init];

  // Determine if we need to show help or not
  AutowiredFast<Config> cfg;
  if (cfg->Get<bool>("showHelpOnStart")) {
    [_menubarController onHelp:nil];
    cfg->Set("showHelpOnStart", false);
  }
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication*)sender
{
  [_menubarController release];
  _menubarController = nil;
  return NSTerminateNow;
}

@end
