/*
 *      Copyright (C) 2012-2013 Team XBMC
 *      Copyright (C) 2015-2016 Team KODI
 *      http://kodi.tv
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with KODI; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "General.h"
#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "addons/kodi-addon-dev-kit/include/kodi/General.h"
#include "dialogs/GUIDialogKaiToast.h"

using namespace ADDON;

namespace kodi
{

void ADDON_General::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi = (sAddonToKodiFuncTable_kodi*)malloc(sizeof(sAddonToKodiFuncTable_kodi));
  funcTable->toKodi.kodi->queue_notification = queue_notification;
}

void ADDON_General::DeInit(sFuncTable_Addon* funcTable)
{
  if (funcTable->toKodi.kodi)
  {
    free(funcTable->toKodi.kodi);
    funcTable->toKodi.kodi = nullptr;
  }
}

void ADDON_General::queue_notification(void* kodiInstance, const int type, const char* message)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (addon == nullptr || message == nullptr)
  {
    CLog::Log(LOGERROR, "kodi::General::%s - called with a null pointer", __FUNCTION__);
    return;
  }

  try
  {
    switch (type)
    {
      case QUEUE_WARNING:
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Warning, addon->Name(), message, 3000, true);
        CLog::Log(LOGDEBUG, "CAddOnGeneral - %s - %s - Warning Message: '%s'", __FUNCTION__, addon->Name().c_str(), message);
        break;
    
      case QUEUE_ERROR:
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Error, addon->Name(), message, 3000, true);
        CLog::Log(LOGDEBUG, "CAddOnGeneral - %s - %s - Error Message : '%s'", __FUNCTION__, addon->Name().c_str(), message);
        break;
    
      case QUEUE_INFO:
      default:
        CGUIDialogKaiToast::QueueNotification(CGUIDialogKaiToast::Info, addon->Name(), message, 3000, false);
        CLog::Log(LOGDEBUG, "CAddOnGeneral - %s - %s - Info Message : '%s'", __FUNCTION__, addon->Name().c_str(), message);
        break;
    }
  }
  catch (std::exception &e)
  {
    ADDON::LogException(addon, e, "queue_notification");
  }
}

} /* namespace kodi */