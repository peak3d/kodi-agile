/*
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

#include "DialogContextMenu.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/DialogContextMenu.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "dialogs/GUIDialogContextMenu.h"
#include "guilib/GUIWindowManager.h"
#include "utils/Variant.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIDialogContextMenu::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->dialogContextMenu.Open = Open;
}

int Interface_GUIDialogContextMenu::Open(void* kodiInstance, const char *heading, const char *entries[], unsigned int size)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIControlTextBox::%s - invalid data", __FUNCTION__);
    return -1;
  }

  try
  {
    CGUIDialogContextMenu* dialog = dynamic_cast<CGUIDialogContextMenu*>(g_windowManager.GetWindow(WINDOW_DIALOG_CONTEXT_MENU));

    if (!heading || !entries || !dialog)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIControlTextBox::%s - invalid handler data (heading='%p', entries='%p', dialog='%p')", __FUNCTION__, heading, entries, dialog).c_str());

    CContextButtons choices;
    for (unsigned int i = 0; i < size; ++i)
      choices.Add(i, entries[i]);

    return dialog->Show(choices);
  }
  HANDLE_ADDON_EXCEPTION(addon);

  return -1;
}

} /* extern "C" */
} /* namespace ADDON */
