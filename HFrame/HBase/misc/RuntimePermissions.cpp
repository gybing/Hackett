

#if ! HANDROID  // We currently don't request runtime permissions on any other platform
                    // than Android, so this file contains a dummy implementation for those.
                    // This may change in the future.

void RuntimePermissions::request (PermissionID, Callback callback)   { callback (true); }
bool RuntimePermissions::isRequired (PermissionID) { return false; }
bool RuntimePermissions::isGranted (PermissionID) { return true; }

#endif


