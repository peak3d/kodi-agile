set(SOURCES AudioDSP.cpp)

set(HEADERS AudioDSP.h)

include_directories(${CORE_SOURCE_DIR}/xbmc/addons/kodi-addon-dev-kit/include)

core_add_library(addoninterface_kodi_adsp)
