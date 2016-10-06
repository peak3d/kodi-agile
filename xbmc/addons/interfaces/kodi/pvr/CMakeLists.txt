set(SOURCES PVRClient.cpp)

set(HEADERS PVRClient.h)

include_directories(${CORE_SOURCE_DIR}/xbmc/addons/kodi-addon-dev-kit/include)

core_add_library(addoninterface_kodi_pvrclient)
