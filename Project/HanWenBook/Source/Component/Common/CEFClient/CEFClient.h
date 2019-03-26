#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <set>

#include "include/cef_version.h"
#include "include/cef_app.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_byte_read_handler.h"
#include "include/wrapper/cef_stream_resource_handler.h"

#ifndef ASSERT
#define ASSERT jassert
#endif

#define REQUIRE_UI_THREAD()   ASSERT(CefCurrentlyOn(TID_UI));
#define REQUIRE_IO_THREAD()   ASSERT(CefCurrentlyOn(TID_IO));
#define REQUIRE_FILE_THREAD() ASSERT(CefCurrentlyOn(TID_FILE));

#include "CEFClientApp.h"
#include "CEFClientHandler.h"
