set(SOURCES AudioDecoder.cpp)

set(HEADERS AudioDecoder.h)

include_directories(${CORE_SOURCE_DIR}/xbmc/addons/kodi-addon-dev-kit/include)

core_add_library(addoninterface_kodi_audiodecoder)
