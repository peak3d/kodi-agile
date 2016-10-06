set(SOURCES InputStream.cpp)

set(HEADERS InputStream.h)

include_directories(${CORE_SOURCE_DIR}/xbmc/addons/kodi-addon-dev-kit/include)

core_add_library(addoninterface_kodi_inputstream)
