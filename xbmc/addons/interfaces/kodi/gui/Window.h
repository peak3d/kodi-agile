#pragma once
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

#include "threads/Event.h"
#include "windows/GUIMediaWindow.h"

struct sFuncTable_Addon;

namespace ADDON
{
extern "C"
{
  class CAddonDll;

  struct Interface_GUIWindow
  {
    static void Init(sFuncTable_Addon* funcTable);

    static void*    New(void* kodiInstance, const char *xmlFilename, const char *defaultSkin, bool forceFallback, bool asDialog);
    static void     Delete(void* kodiInstance, void* handle);
    static void     SetCallbacks(void* kodiInstance,
                                 void* handle,
                                 void* clienthandle,
                                 bool (*initCB)(void*),
                                 bool (*clickCB)(void*, int),
                                 bool (*focusCB)(void*, int),
                                 bool (*onActionCB)(void* handle, int));
    static bool     Show(void* kodiInstance, void* handle);
    static bool     Close(void* kodiInstance, void* handle);
    static bool     DoModal(void* kodiInstance, void* handle);
    static bool     SetFocusId(void* kodiInstance, void* handle, int iControlId);
    static int      GetFocusId(void* kodiInstance, void* handle);
    static void     SetProperty(void* kodiInstance, void* handle, const char *key, const char *value);
    static void     SetPropertyInt(void* kodiInstance, void* handle, const char *key, int value);
    static void     SetPropertyBool(void* kodiInstance, void* handle, const char *key, bool value);
    static void     SetPropertyDouble(void* kodiInstance, void* handle, const char *key, double value);
    static void     GetProperty(void* kodiInstance, void* handle, const char *key,char &property, unsigned int &iMaxStringSize);
    static int      GetPropertyInt(void* kodiInstance, void* handle, const char *key);
    static bool     GetPropertyBool(void* kodiInstance, void* handle, const char *key);
    static double   GetPropertyDouble(void* kodiInstance, void* handle, const char *key);
    static void     ClearProperties(void* kodiInstance, void* handle);
    static void     ClearProperty(void* kodiInstance, void* handle, const char *key);
    static int      GetListSize(void* kodiInstance, void* handle);
    static void     ClearList(void* kodiInstance, void* handle);
    static void*    AddItem(void* kodiInstance, void* handle, void* item, int itemPosition);
    static void*    AddStringItem(void* kodiInstance, void* handle, const char *itemName, int itemPosition);
    static void     RemoveItem(void* kodiInstance, void* handle, int itemPosition);
    static void     RemoveItemFile(void* kodiInstance, void* handle, void* fileItem);
    static void*    GetListItem(void* kodiInstance, void* handle, int listPos);
    static void     SetCurrentListPosition(void* kodiInstance, void* handle, int listPos);
    static int      GetCurrentListPosition(void* kodiInstance, void* handle);
    static void     SetControlLabel(void* kodiInstance, void* handle, int controlId, const char *label);
    static void     MarkDirtyRegion(void* kodiInstance, void* handle);

    static void*    GetControl_Button(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_Edit(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_FadeLabel(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_Image(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_Label(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_Spin(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_RadioButton(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_Progress(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_RenderAddon(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_Slider(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_SettingsSlider(void* kodiInstance, void* handle, int controlId);
    static void*    GetControl_TextBox(void* kodiInstance, void* handle, int controlId);
  };

  class CGUIAddonWindow : public CGUIMediaWindow
  {
  friend struct Interface_GUIWindow;

  public:
    CGUIAddonWindow(int id, const std::string& strXML, ADDON::CAddonDll* addon);
    virtual ~CGUIAddonWindow(void);

    bool OnMessage(CGUIMessage& message) override;
    bool OnAction(const CAction &action) override;
    void AllocResources(bool forceLoad = false) override;
    void FreeResources(bool forceUnLoad = false) override;
    void Render() override;
    void WaitForActionEvent(unsigned int timeout);
    void PulseActionEvent();
    void AddItem(CFileItemPtr fileItem, int itemPosition);
    void RemoveItem(int itemPosition);
    void RemoveItem(CFileItem* fileItem);
    void ClearList();
    CFileItemPtr GetListItem(int position);
    int GetListSize();
    int GetCurrentListPosition();
    void SetCurrentListPosition(int item);
    bool OnClick(int iItem, const std::string &player) override;

    CGUIControl* GetAddonControl(int controlId, CGUIControl::GUICONTROLTYPES type, std::string typeName);

  protected:
    void Update();
    void GetContextButtons(int itemNumber, CContextButtons &buttons) override;
    void SetupShares() override;

    bool (*CBOnInit)(void* cbhdl);
    bool (*CBOnFocus)(void* cbhdl, int controlId);
    bool (*CBOnClick)(void* cbhdl, int controlId);
    bool (*CBOnAction)(void* cbhdl, int);

    void* m_clientHandle;
    const int m_iWindowId;
    int m_iOldWindowId;
    bool m_bModal;
    bool m_bIsDialog;

  private:
    CEvent m_actionEvent;
    ADDON::CAddonDll* m_addon;
    std::string m_mediaDir;
  };

  class CGUIAddonWindowDialog : public CGUIAddonWindow
  {
  public:
    CGUIAddonWindowDialog(int id, const std::string& strXML, ADDON::CAddonDll* addon);
    virtual ~CGUIAddonWindowDialog(void);

    void  Show(bool show = true);
    virtual bool OnMessage(CGUIMessage &message);
    virtual bool IsDialogRunning() const { return m_bRunning; }
    virtual bool IsDialog() const { return true;};
    virtual bool IsModalDialog() const { return true; };
    virtual bool IsMediaWindow() const { return false; };

    void Show_Internal(bool show = true);

  private:
    bool m_bRunning;
  };

} /* extern "C" */
} /* namespace ADDON */
