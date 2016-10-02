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

#include "ControlRendering.h"
#include "General.h"
#include "addons/kodi-addon-dev-kit/include/kodi/gui/ControlRendering.h"

#include "addons/AddonDll.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "guilib/GUIRenderingControl.h"

namespace ADDON
{
extern "C"
{

void Interface_GUIControlAddonRendering::Init(sFuncTable_Addon* funcTable)
{
  funcTable->toKodi.kodi_gui->controlRendering.SetCallbacks = SetCallbacks;
  funcTable->toKodi.kodi_gui->controlRendering.Delete = Delete;
}

void Interface_GUIControlAddonRendering::SetCallbacks(
                            void* kodiInstance, void* handle, void* clienthandle,
                            bool (*createCB)(void*,int,int,int,int,void*),
                            void (*renderCB)(void*), void (*stopCB)(void*), bool (*dirtyCB)(void*))
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIControlAddonRendering::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("Interface_GUIControlAddonRendering - %s - invalid handler data", __FUNCTION__).c_str());

    CGUIAddonRenderingControl* pAddonControl = static_cast<CGUIAddonRenderingControl*>(handle);

    Interface_GUIGeneral::Lock();
    pAddonControl->m_clientHandle = clienthandle;
    pAddonControl->CBCreate = createCB;
    pAddonControl->CBRender = renderCB;
    pAddonControl->CBStop = stopCB;
    pAddonControl->CBDirty = dirtyCB;
    pAddonControl->m_addon = addon;
    Interface_GUIGeneral::Unlock();

    pAddonControl->m_pControl->InitCallback(pAddonControl);
  }
  HANDLE_ADDON_EXCEPTION(addon);
}

void Interface_GUIControlAddonRendering::Delete(void* kodiInstance, void* handle)
{
  CAddonDll* addon = static_cast<CAddonDll*>(kodiInstance);
  if (!addon)
  {
    CLog::Log(LOGERROR, "Interface_GUIControlAddonRendering::%s - invalid data", __FUNCTION__);
    return;
  }

  try
  {
    if (!handle)
      throw std::logic_error(StringUtils::Format("Interface_GUIControlAddonRendering - %s - invalid handler data", __FUNCTION__).c_str());

    Interface_GUIGeneral::Lock();
    static_cast<CGUIAddonRenderingControl*>(handle)->Delete();
    Interface_GUIGeneral::Unlock();
  }
  HANDLE_ADDON_EXCEPTION(addon);
}


CGUIAddonRenderingControl::CGUIAddonRenderingControl(CGUIRenderingControl *pControl)
  : CBCreate{nullptr},
    CBRender{nullptr},
    CBStop{nullptr},
    CBDirty{nullptr},
    m_clientHandle{nullptr},
    m_addon{nullptr},
    m_pControl{pControl},
    m_refCount{1}
{
}

bool CGUIAddonRenderingControl::Create(int x, int y, int w, int h, void *device)
{
  try
  {
    if (CBCreate)
    {
      if (CBCreate(m_clientHandle, x, y, w, h, device))
      {
        ++m_refCount;
        return true;
      }
    }
  }
  HANDLE_ADDON_EXCEPTION(m_addon);

  return false;
}

void CGUIAddonRenderingControl::Render()
{
  try
  {
    if (CBRender)
    {
      CBRender(m_clientHandle);
    }
  }
  HANDLE_ADDON_EXCEPTION(m_addon);
}

void CGUIAddonRenderingControl::Stop()
{
  try
  {
    if (CBStop)
    {
      CBStop(m_clientHandle);
    }
  }
  HANDLE_ADDON_EXCEPTION(m_addon);

  --m_refCount;
  if (m_refCount <= 0)
    delete this;
}

void CGUIAddonRenderingControl::Delete()
{
  --m_refCount;
  if (m_refCount <= 0)
    delete this;
}

bool CGUIAddonRenderingControl::IsDirty()
{
  bool ret = true;
  try
  {
    if (CBDirty)
    {
      ret = CBDirty(m_clientHandle);
    }
  }
  HANDLE_ADDON_EXCEPTION(m_addon);

  return ret;
}

} /* extern "C" */
} /* namespace ADDON */
