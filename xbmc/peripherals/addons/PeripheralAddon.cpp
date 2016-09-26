/*
 *      Copyright (C) 2014-2016 Team Kodi
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
 *  along with this Program; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "PeripheralAddon.h"
#include "AddonButtonMap.h"
#include "PeripheralAddonTranslator.h"
#include "addons/AddonManager.h"
#include "addons/interfaces/ExceptionHandling.h"
#include "filesystem/Directory.h"
#include "filesystem/SpecialProtocol.h"
#include "games/controllers/Controller.h"
#include "games/controllers/ControllerLayout.h"
#include "input/joysticks/DriverPrimitive.h"
#include "input/joysticks/IButtonMap.h"
#include "input/joysticks/IDriverHandler.h"
#include "input/joysticks/JoystickTranslator.h"
#include "input/joysticks/JoystickUtils.h"
#include "peripherals/Peripherals.h"
#include "peripherals/bus/virtual/PeripheralBusAddon.h"
#include "peripherals/devices/PeripheralJoystick.h"
#include "settings/Settings.h"
#include "utils/log.h"
#include "utils/StringUtils.h"

#include <algorithm>
#include <string.h>
#include <utility>

using namespace JOYSTICK;
using namespace PERIPHERALS;
using namespace XFILE;

std::unique_ptr<CPeripheralAddon> CPeripheralAddon::FromExtension(ADDON::AddonProps props, const cp_extension_t* ext)
{
  std::string strProvidesJoysticks = ADDON::CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@provides_joysticks");
  std::string strProvidesButtonMaps = ADDON::CAddonMgr::GetInstance().GetExtValue(ext->configuration, "@provides_buttonmaps");

  bool bProvidesJoysticks = StringUtils::EqualsNoCase(strProvidesJoysticks, "true");
  bool bProvidesButtonMaps = StringUtils::EqualsNoCase(strProvidesButtonMaps, "true");

  return std::unique_ptr<CPeripheralAddon>(new CPeripheralAddon(std::move(props), bProvidesJoysticks, bProvidesButtonMaps));
}

CPeripheralAddon::CPeripheralAddon(ADDON::AddonProps props, bool bProvidesJoysticks, bool bProvidesButtonMaps) :
  CAddonDll(std::move(props)),
  m_apiVersion("0.0.0"),
  m_bProvidesJoysticks(bProvidesJoysticks),
  m_bProvidesButtonMaps(bProvidesButtonMaps),
  m_addonInstance(nullptr)
{
  ResetProperties();
}

CPeripheralAddon::~CPeripheralAddon(void)
{
  // delete all peripherals provided by this addon
  for (const auto& peripheral : m_peripherals)
  {
    if (CSettings::GetInstance().GetBool(CSettings::SETTING_INPUT_CONTROLLERPOWEROFF))
    {
      // shutdown the joystick if it is supported
      if (peripheral.second->Type() == PERIPHERAL_JOYSTICK)
      {
        CPeripheralJoystick* joystick = static_cast<CPeripheralJoystick*>(peripheral.second);
        if (joystick->SupportsPowerOff())
          PowerOffJoystick(peripheral.first);
      }
    }

    delete peripheral.second;
  }
  m_peripherals.clear();

  // only clear buttonMaps but don't delete them as they are owned by a CAddonJoystickInputHandling instance
  m_buttonMaps.clear();

  Destroy();
}

void CPeripheralAddon::ResetProperties(void)
{
  // Initialise members
  m_strUserPath        = CSpecialProtocol::TranslatePath(Profile());
  m_strClientPath      = CSpecialProtocol::TranslatePath(Path());

  m_apiVersion = ADDON::AddonVersion("0.0.0");
  
  memset(&m_struct, 0, sizeof(m_struct));
}

ADDON::AddonPtr CPeripheralAddon::GetRunningInstance(void) const
{
  PeripheralBusAddonPtr addonBus = std::static_pointer_cast<CPeripheralBusAddon>(g_peripherals.GetBusByType(PERIPHERAL_BUS_ADDON));
  if (addonBus)
  {
    ADDON::AddonPtr peripheralAddon;
    if (addonBus->GetAddon(ID(), peripheralAddon))
      return peripheralAddon;
  }
  return CAddon::GetRunningInstance();
}

ADDON_STATUS CPeripheralAddon::CreateAddon(void)
{
  ADDON_STATUS status(ADDON_STATUS_UNKNOWN);

  // Reset all properties to defaults
  ResetProperties();

  // Create directory for user data
  if (!CDirectory::Exists(m_strUserPath))
    CDirectory::Create(m_strUserPath);

  // Initialise the add-on
  CLog::Log(LOGDEBUG, "PERIPHERAL - %s - creating peripheral add-on instance '%s'", __FUNCTION__, Name().c_str());
  status = CAddonDll::Create();

  if (status == ADDON_STATUS_OK)
  {
    m_struct.props.user_path = m_strUserPath.c_str();
    m_struct.props.addon_path = m_strClientPath.c_str();
    m_struct.toKodi.kodiInstance = this;
    m_struct.toKodi.TriggerScan = TriggerScan;
    m_struct.toKodi.RefreshButtonMaps = RefreshButtonMaps;
    m_struct.toKodi.FeatureCount = FeatureCount;
    status = CAddonDll::CreateInstance(ADDON_INSTANCE_PERIPHERAL, ID().c_str(), &m_struct, &m_addonInstance);
  }

  if (status == ADDON_STATUS_OK)
  {
    if (!GetAddonProperties())
    {
      Destroy();
      status = ADDON_STATUS_PERMANENT_FAILURE;
    }
  }

  return status;
}

void CPeripheralAddon::Destroy()
{
  CAddonDll::DestroyInstance(ADDON_INSTANCE_PERIPHERAL, m_addonInstance);
  CAddonDll::Destroy();
  m_addonInstance = nullptr;
}


bool CPeripheralAddon::GetAddonProperties(void)
{
  PERIPHERAL_CAPABILITIES addonCapabilities = { };

  // Get the capabilities
  try
  {
    if (m_struct.toAddon.GetCapabilities)
      m_struct.toAddon.GetCapabilities(m_addonInstance, &addonCapabilities);
  }
  catch (std::exception& e) { ExceptionHandle(e, "GetCapabilities()"); return false; }

  // Verify capabilities against addon.xml
  if (m_bProvidesJoysticks != addonCapabilities.provides_joysticks)
  {
    CLog::Log(LOGERROR, "PERIPHERAL - Add-on '%s': provides_joysticks'(%s) in add-on DLL  doesn't match 'provides_joysticks'(%s) in addon.xml. Please contact the developer of this add-on: %s",
        Name().c_str(), addonCapabilities.provides_joysticks ? "true" : "false",
        m_bProvidesJoysticks ? "true" : "false", Author().c_str());
    return false;
  }
  if (m_bProvidesButtonMaps != addonCapabilities.provides_buttonmaps)
  {
    CLog::Log(LOGERROR, "PERIPHERAL - Add-on '%s': provides_buttonmaps' (%s) in add-on DLL  doesn't match 'provides_buttonmaps' (%s) in addon.xml. Please contact the developer of this add-on: %s",
        Name().c_str(), addonCapabilities.provides_buttonmaps ? "true" : "false",
        m_bProvidesButtonMaps ? "true" : "false", Author().c_str());
    return false;
  }

  return true;
}

bool CPeripheralAddon::Register(unsigned int peripheralIndex, CPeripheral *peripheral)
{
  if (!peripheral)
    return false;

  CSingleLock lock(m_critSection);

  if (m_peripherals.find(peripheralIndex) == m_peripherals.end())
  {
    if (peripheral->Type() == PERIPHERAL_JOYSTICK)
    {
      m_peripherals[peripheralIndex] = static_cast<CPeripheralJoystick*>(peripheral);

      CLog::Log(LOGNOTICE, "%s - new %s device registered on %s->%s: %s",
          __FUNCTION__, PeripheralTypeTranslator::TypeToString(peripheral->Type()),
          PeripheralTypeTranslator::BusTypeToString(PERIPHERAL_BUS_ADDON),
          peripheral->Location().c_str(), peripheral->DeviceName().c_str());

      return true;
    }
  }
  return false;
}

void CPeripheralAddon::UnregisterRemovedDevices(const PeripheralScanResults &results, std::vector<CPeripheral*>& removedPeripherals)
{
  CSingleLock lock(m_critSection);
  std::vector<unsigned int> removedIndexes;
  for (std::map<unsigned int, CPeripheral*>::const_iterator it = m_peripherals.begin(); it != m_peripherals.end(); ++it)
  {
    CPeripheral *peripheral = it->second;
    PeripheralScanResult updatedDevice(PERIPHERAL_BUS_ADDON);
    if (!results.GetDeviceOnLocation(peripheral->Location(), &updatedDevice) ||
      *peripheral != updatedDevice)
    {
      // Device removed
      removedIndexes.push_back(it->first);
    }
  }
  lock.Leave();

  for (std::vector<unsigned int>::const_iterator it = removedIndexes.begin(); it != removedIndexes.end(); ++it)
  {
    CPeripheral *peripheral = m_peripherals[*it];
    CLog::Log(LOGNOTICE, "%s - device removed from %s/%s: %s (%s:%s)", __FUNCTION__, PeripheralTypeTranslator::TypeToString(peripheral->Type()), peripheral->Location().c_str(), peripheral->DeviceName().c_str(), peripheral->VendorIdAsString(), peripheral->ProductIdAsString());
    peripheral->OnDeviceRemoved();
    removedPeripherals.push_back(peripheral);
    m_peripherals.erase(*it);
  }
}

bool CPeripheralAddon::HasFeature(const PeripheralFeature feature) const
{
  if (feature == FEATURE_JOYSTICK)
    return m_bProvidesJoysticks;

  return false;
}

void CPeripheralAddon::GetFeatures(std::vector<PeripheralFeature> &features) const
{
  if (m_bProvidesJoysticks && std::find(features.begin(), features.end(), FEATURE_JOYSTICK) == features.end())
    features.push_back(FEATURE_JOYSTICK);
}

CPeripheral* CPeripheralAddon::GetPeripheral(unsigned int index) const
{
  CPeripheral* peripheral(NULL);
  CSingleLock lock(m_critSection);
  std::map<unsigned int, CPeripheral*>::const_iterator it = m_peripherals.find(index);
  if (it != m_peripherals.end())
    peripheral = it->second;
  return peripheral;
}

CPeripheral *CPeripheralAddon::GetByPath(const std::string &strPath) const
{
  CSingleLock lock(m_critSection);
  for (std::map<unsigned int, CPeripheral*>::const_iterator it = m_peripherals.begin(); it != m_peripherals.end(); ++it)
  {
    if (StringUtils::EqualsNoCase(strPath, it->second->FileLocation()))
      return it->second;
  }

  return NULL;
}

int CPeripheralAddon::GetPeripheralsWithFeature(std::vector<CPeripheral*> &results, const PeripheralFeature feature) const
{
  int iReturn(0);
  CSingleLock lock(m_critSection);
  for (std::map<unsigned int, CPeripheral*>::const_iterator it = m_peripherals.begin(); it != m_peripherals.end(); ++it)
  {
    if (it->second->HasFeature(feature))
    {
      results.push_back(it->second);
      ++iReturn;
    }
  }
  return iReturn;
}

size_t CPeripheralAddon::GetNumberOfPeripherals(void) const
{
  CSingleLock lock(m_critSection);
  return m_peripherals.size();
}

size_t CPeripheralAddon::GetNumberOfPeripheralsWithId(const int iVendorId, const int iProductId) const
{
  int iReturn(0);
  CSingleLock lock(m_critSection);
  for (std::map<unsigned int, CPeripheral*>::const_iterator it = m_peripherals.begin(); it != m_peripherals.end(); ++it)
  {
    if (it->second->VendorId() == iVendorId &&
        it->second->ProductId() == iProductId)
      iReturn++;
  }

  return iReturn;
}

void CPeripheralAddon::GetDirectory(const std::string &strPath, CFileItemList &items) const
{
  CSingleLock lock(m_critSection);
  for (std::map<unsigned int, CPeripheral*>::const_iterator it = m_peripherals.begin(); it != m_peripherals.end(); ++it)
  {
    const CPeripheral *peripheral = it->second;
    if (peripheral->IsHidden())
      continue;

    CFileItemPtr peripheralFile(new CFileItem(peripheral->DeviceName()));
    peripheralFile->SetPath(peripheral->FileLocation());
    peripheralFile->SetProperty("vendor", peripheral->VendorIdAsString());
    peripheralFile->SetProperty("product", peripheral->ProductIdAsString());
    peripheralFile->SetProperty("bus", PeripheralTypeTranslator::BusTypeToString(peripheral->GetBusType()));
    peripheralFile->SetProperty("location", peripheral->Location());
    peripheralFile->SetProperty("class", PeripheralTypeTranslator::TypeToString(peripheral->Type()));
    peripheralFile->SetProperty("version", peripheral->GetVersionInfo());
    peripheralFile->SetIconImage(peripheral->GetIcon());
    items.Add(peripheralFile);
  }
}

bool CPeripheralAddon::PerformDeviceScan(PeripheralScanResults &results)
{
  unsigned int      peripheralCount;
  PERIPHERAL_INFO*  pScanResults;
  PERIPHERAL_ERROR retVal = PERIPHERAL_ERROR_UNKNOWN;

  try
  {
    if (m_struct.toAddon.PerformDeviceScan)
      retVal = m_struct.toAddon.PerformDeviceScan(m_addonInstance, &peripheralCount, &pScanResults);
    LogError(retVal, "PerformDeviceScan()");
  }
  catch (std::exception &e) { ExceptionHandle(e, "PerformDeviceScan()"); return false;  }

  if (retVal == PERIPHERAL_NO_ERROR)
  {
    for (unsigned int i = 0; i < peripheralCount; i++)
    {
      ADDON::Peripheral peripheral(pScanResults[i]);

      PeripheralScanResult result(PERIPHERAL_BUS_ADDON);
      switch (peripheral.Type())
      {
      case PERIPHERAL_TYPE_JOYSTICK:
        result.m_type = PERIPHERAL_JOYSTICK;
        break;
      default:
        continue;
      }

      result.m_strDeviceName = peripheral.Name();
      result.m_strLocation   = StringUtils::Format("%s/%d", ID().c_str(), peripheral.Index());
      result.m_iVendorId     = peripheral.VendorID();
      result.m_iProductId    = peripheral.ProductID();
      result.m_mappedType    = PERIPHERAL_JOYSTICK;
      result.m_mappedBusType = PERIPHERAL_BUS_ADDON;
      result.m_iSequence     = 0;

      if (!results.ContainsResult(result))
        results.m_results.push_back(result);
    }

    try
    {
      if (m_struct.toAddon.FreeScanResults)
        m_struct.toAddon.FreeScanResults(m_addonInstance, peripheralCount, pScanResults);
      else
        throw std::logic_error("PerformDeviceScan set, but FreeScanResults function address empty!");
    }
    catch (std::exception &e) { ExceptionHandle(e, "FreeScanResults()"); }

    return true;
  }

  return false;
}

bool CPeripheralAddon::ProcessEvents(void)
{
  if (!m_bProvidesJoysticks)
    return false;

  PERIPHERAL_ERROR retVal = PERIPHERAL_ERROR_UNKNOWN;
  unsigned int eventCount = 0;
  PERIPHERAL_EVENT* pEvents = NULL;

  try
  {
    if (m_struct.toAddon.GetEvents)
      retVal = m_struct.toAddon.GetEvents(m_addonInstance, &eventCount, &pEvents);
    LogError(retVal, "GetEvents()");
  }
  catch (std::exception &e) { ExceptionHandle(e, "GetEvents()"); return false;  }

  if (retVal == PERIPHERAL_NO_ERROR)
  {
    for (unsigned int i = 0; i < eventCount; i++)
    {
      ADDON::PeripheralEvent event(pEvents[i]);
      CPeripheral* device = GetPeripheral(event.PeripheralIndex());
      if (!device)
        continue;

      switch (device->Type())
      {
      case PERIPHERAL_JOYSTICK:
      {
        CPeripheralJoystick* joystickDevice = static_cast<CPeripheralJoystick*>(device);

        switch (event.Type())
        {
          case PERIPHERAL_EVENT_TYPE_DRIVER_BUTTON:
          {
            const bool bPressed = (event.ButtonState() == JOYSTICK_STATE_BUTTON_PRESSED);
            CLog::Log(LOGDEBUG, "Button [ %u ] on %s %s", event.DriverIndex(),
                      joystickDevice->DeviceName().c_str(), bPressed ? "pressed" : "released");
            if (joystickDevice->OnButtonMotion(event.DriverIndex(), bPressed))
              CLog::Log(LOGDEBUG, "Joystick button event handled");
            break;
          }
          case PERIPHERAL_EVENT_TYPE_DRIVER_HAT:
          {
            const HAT_STATE state = CPeripheralAddonTranslator::TranslateHatState(event.HatState());
            CLog::Log(LOGDEBUG, "Hat [ %u ] on %s %s", event.DriverIndex(),
                      joystickDevice->DeviceName().c_str(), CJoystickTranslator::HatStateToString(state));
            if (joystickDevice->OnHatMotion(event.DriverIndex(), state))
              CLog::Log(LOGDEBUG, "Joystick hat event handled");
            break;
          }
          case PERIPHERAL_EVENT_TYPE_DRIVER_AXIS:
          {
            joystickDevice->OnAxisMotion(event.DriverIndex(), event.AxisState());
            break;
          }
          default:
            break;
        }
        break;
      }
      default:
        break;
      }
    }

    for (std::map<unsigned int, CPeripheral*>::const_iterator it = m_peripherals.begin(); it != m_peripherals.end(); ++it)
    {
      if (it->second->Type() == PERIPHERAL_JOYSTICK)
        static_cast<CPeripheralJoystick*>(it->second)->ProcessAxisMotions();
    }

    try
    {
      if (m_struct.toAddon.FreeEvents)
        m_struct.toAddon.FreeEvents(m_addonInstance, eventCount, pEvents);
      else
        throw std::logic_error("GetEvents set, but FreeEvents function address empty!");
    }
    catch (std::exception &e) { ExceptionHandle(e, "FreeEvents()"); }

    return true;
  }

  return false;
}

bool CPeripheralAddon::SendRumbleEvent(unsigned int peripheralIndex, unsigned int driverIndex, float magnitude)
{
  bool bHandled = false;

  PERIPHERAL_EVENT eventStruct = { };

  eventStruct.peripheral_index = peripheralIndex;
  eventStruct.type             = PERIPHERAL_EVENT_TYPE_SET_MOTOR;
  eventStruct.driver_index     = driverIndex;
  eventStruct.motor_state      = magnitude;

  try
  {
    if (m_struct.toAddon.SendEvent)
      bHandled = m_struct.toAddon.SendEvent(m_addonInstance, &eventStruct); 
  }
  catch (std::exception &e) { ExceptionHandle(e, "SendEvent()"); }

  return bHandled;
}

bool CPeripheralAddon::GetJoystickProperties(unsigned int index, CPeripheralJoystick& joystick)
{
  if (!m_bProvidesJoysticks)
    return false;

  PERIPHERAL_ERROR retVal = PERIPHERAL_ERROR_UNKNOWN;

  JOYSTICK_INFO joystickStruct;

  try
  {
    if (m_struct.toAddon.GetJoystickInfo)
      retVal = m_struct.toAddon.GetJoystickInfo(m_addonInstance, index, &joystickStruct);
    LogError(retVal, "GetJoystickInfo()");
  }
  catch (std::exception &e) { ExceptionHandle(e, "GetJoystickInfo()"); return false;  }

  if (retVal == PERIPHERAL_NO_ERROR)
  {
    ADDON::Joystick addonJoystick(joystickStruct);
    SetJoystickInfo(joystick, addonJoystick);

    try
    {
      if (m_struct.toAddon.FreeJoystickInfo)
        m_struct.toAddon.FreeJoystickInfo(m_addonInstance, &joystickStruct);
      else
        throw std::logic_error("GetJoystickInfo set, but FreeJoystickInfo function address empty!");
    }
    catch (std::exception &e) { ExceptionHandle(e, "FreeJoystickInfo()"); }

    return true;
  }

  return false;
}

bool CPeripheralAddon::GetFeatures(const CPeripheral* device,
                                   const std::string& strControllerId,
                                   FeatureMap& features)
{
  if (!m_bProvidesButtonMaps)
    return false;

  PERIPHERAL_ERROR retVal = PERIPHERAL_ERROR_UNKNOWN;

  ADDON::Joystick joystickInfo;
  GetJoystickInfo(device, joystickInfo);

  JOYSTICK_INFO joystickStruct;
  joystickInfo.ToStruct(joystickStruct);

  unsigned int      featureCount = 0;
  JOYSTICK_FEATURE* pFeatures = NULL;

  try
  {
    if (m_struct.toAddon.GetFeatures)
      retVal = m_struct.toAddon.GetFeatures(m_addonInstance, &joystickStruct, strControllerId.c_str(),
                                            &featureCount, &pFeatures);
    LogError(retVal, "GetFeatures()");
  }
  catch (std::exception &e) { ExceptionHandle(e, "GetFeatures()"); return false;  }

  if (retVal == PERIPHERAL_NO_ERROR)
  {
    for (unsigned int i = 0; i < featureCount; i++)
    {
      ADDON::JoystickFeature feature(pFeatures[i]);
      if (feature.Type() != JOYSTICK_FEATURE_TYPE_UNKNOWN)
        features[feature.Name()] = std::move(feature);
    }

    try
    {
      if (m_struct.toAddon.FreeFeatures)
        m_struct.toAddon.FreeFeatures(m_addonInstance, featureCount, pFeatures);
      else
        throw std::logic_error("GetFeatures set, but FreeFeatures function address empty!");
    }
    catch (std::exception &e) { ExceptionHandle(e, "FreeFeatures()"); }

    return true;
  }

  return false;
}

bool CPeripheralAddon::MapFeature(const CPeripheral* device,
                                  const std::string& strControllerId,
                                  const ADDON::JoystickFeature& feature)
{
  if (!m_bProvidesButtonMaps)
    return false;

  PERIPHERAL_ERROR retVal = PERIPHERAL_ERROR_UNKNOWN;

  ADDON::Joystick joystickInfo;
  GetJoystickInfo(device, joystickInfo);

  JOYSTICK_INFO joystickStruct;
  joystickInfo.ToStruct(joystickStruct);

  JOYSTICK_FEATURE addonFeature;
  feature.ToStruct(addonFeature);

  try
  {
    if (m_struct.toAddon.MapFeatures)
      retVal = m_struct.toAddon.MapFeatures(m_addonInstance, &joystickStruct, 
                                            strControllerId.c_str(),
                                            1, &addonFeature);
    LogError(retVal, "MapFeatures()");
  }
  catch (std::exception &e) { ExceptionHandle(e, "MapFeatures()"); return false; }

  if (retVal == PERIPHERAL_NO_ERROR)
  {
    // Notify observing button maps
    RefreshButtonMaps(device->DeviceName());
  }

  return retVal == PERIPHERAL_NO_ERROR;
}

void CPeripheralAddon::SaveButtonMap(const CPeripheral* device)
{
  if (!m_bProvidesButtonMaps)
    return;

  ADDON::Joystick joystickInfo;
  GetJoystickInfo(device, joystickInfo);

  JOYSTICK_INFO joystickStruct;
  joystickInfo.ToStruct(joystickStruct);

  try
  {
    if (m_struct.toAddon.SaveButtonMap)
      m_struct.toAddon.SaveButtonMap(m_addonInstance, &joystickStruct);
  }
  catch (std::exception &e) { ExceptionHandle(e, "SaveButtonMap()"); return; }
}

void CPeripheralAddon::ResetButtonMap(const CPeripheral* device, const std::string& strControllerId)
{
  if (!m_bProvidesButtonMaps)
    return;

  ADDON::Joystick joystickInfo;
  GetJoystickInfo(device, joystickInfo);

  JOYSTICK_INFO joystickStruct;
  joystickInfo.ToStruct(joystickStruct);

  try
  {
    if (m_struct.toAddon.ResetButtonMap)
      m_struct.toAddon.ResetButtonMap(m_addonInstance, &joystickStruct, strControllerId.c_str());
  }
  catch (std::exception &e) { ExceptionHandle(e, "ResetButtonMap()"); return; }
}

void CPeripheralAddon::PowerOffJoystick(unsigned int index)
{
  if (!HasFeature(FEATURE_JOYSTICK))
    return;

  try
  {
    if (m_struct.toAddon.PowerOffJoystick)
      m_struct.toAddon.PowerOffJoystick(m_addonInstance, index);
  }
  catch (std::exception &e) { ExceptionHandle(e, "PowerOffJoystick()"); return; }
}

void CPeripheralAddon::RegisterButtonMap(CPeripheral* device, IButtonMap* buttonMap)
{
  UnregisterButtonMap(buttonMap);
  m_buttonMaps.push_back(std::make_pair(device, buttonMap));
}

void CPeripheralAddon::UnregisterButtonMap(IButtonMap* buttonMap)
{
  for (auto it = m_buttonMaps.begin(); it != m_buttonMaps.end(); ++it)
  {
    if (it->second == buttonMap)
    {
      m_buttonMaps.erase(it);
      break;
    }
  }
}

void CPeripheralAddon::RefreshButtonMaps(const std::string& strDeviceName /* = "" */)
{
  for (auto it = m_buttonMaps.begin(); it != m_buttonMaps.end(); ++it)
  {
    if (strDeviceName.empty() || strDeviceName == it->first->DeviceName())
      it->second->Load();
  }
}

void CPeripheralAddon::GetPeripheralInfo(const CPeripheral* device, ADDON::Peripheral& peripheralInfo)
{
  peripheralInfo.SetName(device->DeviceName());
  peripheralInfo.SetVendorID(device->VendorId());
  peripheralInfo.SetProductID(device->ProductId());
}

void CPeripheralAddon::GetJoystickInfo(const CPeripheral* device, ADDON::Joystick& joystickInfo)
{
  GetPeripheralInfo(device, joystickInfo);

  if (device->Type() == PERIPHERAL_JOYSTICK)
  {
    const CPeripheralJoystick* joystick = static_cast<const CPeripheralJoystick*>(device);
    joystickInfo.SetProvider(joystick->Provider());
    joystickInfo.SetButtonCount(joystick->ButtonCount());
    joystickInfo.SetHatCount(joystick->HatCount());
    joystickInfo.SetAxisCount(joystick->AxisCount());
    joystickInfo.SetMotorCount(joystick->MotorCount());
    joystickInfo.SetSupportsPowerOff(joystick->SupportsPowerOff());
  }
}

void CPeripheralAddon::SetJoystickInfo(CPeripheralJoystick& joystick, const ADDON::Joystick& joystickInfo)
{
  joystick.SetProvider(joystickInfo.Provider());
  joystick.SetRequestedPort(joystickInfo.RequestedPort());
  joystick.SetButtonCount(joystickInfo.ButtonCount());
  joystick.SetHatCount(joystickInfo.HatCount());
  joystick.SetAxisCount(joystickInfo.AxisCount());
  joystick.SetMotorCount(joystickInfo.MotorCount());
  joystick.SetSupportsPowerOff(joystickInfo.SupportsPowerOff());
}

bool CPeripheralAddon::LogError(const PERIPHERAL_ERROR error, const char *strMethod) const
{
  if (error != PERIPHERAL_NO_ERROR)
  {
    CLog::Log(LOGERROR, "PERIPHERAL - %s - addon '%s' returned an error: %s",
        strMethod, Name().c_str(), CPeripheralAddonTranslator::TranslateError(error));
    return false;
  }
  return true;
}

void CPeripheralAddon::ExceptionHandle(std::exception& ex, const char* function)
{
  ADDON::LogException(this, ex, function); // Handle exception
  memset(&m_struct.toAddon, 0, sizeof(m_struct.toAddon)); // reset function table to prevent further exception call  
}

void CPeripheralAddon::TriggerScan(void* kodiInstance)
{
  g_peripherals.TriggerDeviceScan(PERIPHERAL_BUS_ADDON);
}

void CPeripheralAddon::RefreshButtonMaps(void* kodiInstance, const char* deviceName, const char* controllerId)
{
  if (!kodiInstance)
    return;

  static_cast<CPeripheralAddon*>(kodiInstance)->RefreshButtonMaps(deviceName ? deviceName : "");
}

unsigned int CPeripheralAddon::FeatureCount(void* kodiInstance, const char* controllerId, JOYSTICK_FEATURE_TYPE type)
{
  using namespace ADDON;
  using namespace GAME;

  unsigned int count = 0;

  AddonPtr addon;
  if (CAddonMgr::GetInstance().GetAddon(controllerId, addon, ADDON_GAME_CONTROLLER))
  {
    ControllerPtr controller = std::static_pointer_cast<CController>(addon);
    if (controller->LoadLayout())
      count = controller->Layout().FeatureCount(CPeripheralAddonTranslator::TranslateFeatureType(type));
  }

  return count;
}
