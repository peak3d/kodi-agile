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

#include "AddonInterfaces.h"

#include "addons/Addon.h"

#include "filesystem/SpecialProtocol.h"
#include "messaging/ApplicationMessenger.h"
#include "utils/log.h"

using namespace KODI::MESSAGING;

namespace ADDON
{

void CAddonInterfaces::OnApplicationMessage(ThreadMessage* pMsg)
{
//   switch (pMsg->dwMessage)
//   {
//   case TMSG_GUI_ADDON_DIALOG:
//   {
//     if (pMsg->lpVoid)
//     { //! @todo This is ugly - really these binary add-on dialogs should just be normal Kodi dialogs
//       switch (pMsg->param1)
//       {
//       case 1:
//         static_cast<KodiAPI::GUI::CGUIAddonWindowDialog*>(pMsg->lpVoid)->Show_Internal(pMsg->param2 > 0);
//         break;
//       };
//     }
//   }
//   break;
//   }
}

} /* namespace ADDON */
