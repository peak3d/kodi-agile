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

#include "DialogTextViewer.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/DialogTextViewer.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "dialogs/GUIDialogTextViewer.h"
#include "guilib/GUIWindowManager.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIDialogTextViewer::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->dialogTextViewer.Open = Open;
}

void Interface_GUIDialogTextViewer::Open(void* kodiInstance, const char *heading, const char *text)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "ADDON::Interface_GUIDialogTextViewer::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    CGUIDialogTextViewer* dialog = dynamic_cast<CGUIDialogTextViewer*>(g_windowManager.GetWindow(WINDOW_DIALOG_TEXT_VIEWER));
    if (!heading || !text || !dialog)
      throw std::logic_error(StringUtils::Format("ADDON::Interface_GUIDialogTextViewer::%s - invalid handler data (heading='%p', text='%p', dialog='%p')", __FUNCTION__, heading, text, dialog).c_str());

    dialog->SetHeading(heading);
    dialog->SetText(text);
    dialog->Open();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

} /* extern "C" */
} /* namespace ADDON */
