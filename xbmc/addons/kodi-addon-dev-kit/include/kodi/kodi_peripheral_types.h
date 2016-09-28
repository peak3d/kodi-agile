#pragma once
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
#ifndef __PERIPHERAL_TYPES_H__
#define __PERIPHERAL_TYPES_H__

#include "xbmc_addon_types.h"

#ifdef TARGET_WINDOWS
  #include <windows.h>
#else
  #ifndef __cdecl
    #define __cdecl
  #endif
  #ifndef __declspec
    #define __declspec(X)
  #endif
#endif

#include <stdint.h>
#include <array> // Requires c++11
#include <cstring>
#include <map>
#include <string>
#include <vector>

#define PERIPHERAL_SAFE_DELETE(x)        do { delete   (x); (x) = NULL; } while (0)
#define PERIPHERAL_SAFE_DELETE_ARRAY(x)  do { delete[] (x); (x) = NULL; } while (0)

#undef ATTRIBUTE_PACKED
#undef PRAGMA_PACK_BEGIN
#undef PRAGMA_PACK_END

#if defined(__GNUC__)
  #if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
    #define ATTRIBUTE_PACKED __attribute__ ((packed))
    #define PRAGMA_PACK 0
  #endif
#endif

#if !defined(ATTRIBUTE_PACKED)
  #define ATTRIBUTE_PACKED
  #define PRAGMA_PACK 1
#endif

/* current Peripheral API version */
#define PERIPHERAL_API_VERSION "1.1.0"

/* min. Peripheral API version */
#define PERIPHERAL_MIN_API_VERSION "1.1.0"

/* indicates a joystick has no preference for port number */
#define NO_PORT_REQUESTED     (-1)

/* joystick's driver button/hat/axis index is unknown */
#define DRIVER_INDEX_UNKNOWN  (-1)

#ifdef __cplusplus
extern "C" {
#endif

  /// @name Peripheral types
  ///{
  typedef enum PERIPHERAL_ERROR
  {
    PERIPHERAL_NO_ERROR                      =  0, // no error occurred
    PERIPHERAL_ERROR_UNKNOWN                 = -1, // an unknown error occurred
    PERIPHERAL_ERROR_FAILED                  = -2, // the command failed
    PERIPHERAL_ERROR_INVALID_PARAMETERS      = -3, // the parameters of the method are invalid for this operation
    PERIPHERAL_ERROR_NOT_IMPLEMENTED         = -4, // the method that the frontend called is not implemented
    PERIPHERAL_ERROR_NOT_CONNECTED           = -5, // no peripherals are connected
    PERIPHERAL_ERROR_CONNECTION_FAILED       = -6, // peripherals are connected, but command was interrupted
  } PERIPHERAL_ERROR;

  typedef enum PERIPHERAL_TYPE
  {
    PERIPHERAL_TYPE_UNKNOWN,
    PERIPHERAL_TYPE_JOYSTICK,
  } PERIPHERAL_TYPE;

  typedef struct PERIPHERAL_INFO
  {
    PERIPHERAL_TYPE type;           /*!< @brief type of peripheral */
    char*           name;           /*!< @brief name of peripheral */
    uint16_t        vendor_id;      /*!< @brief vendor ID of peripheral, 0x0000 if unknown */
    uint16_t        product_id;     /*!< @brief product ID of peripheral, 0x0000 if unknown */
    unsigned int    index;          /*!< @brief the order in which the add-on identified this peripheral */
  } ATTRIBUTE_PACKED PERIPHERAL_INFO;

  /*!
   * @brief Properties passed to the Create() method of an add-on.
   */
  typedef struct PERIPHERAL_PROPS
  {
    const char* user_path;              /*!< @brief path to the user profile */
    const char* addon_path;             /*!< @brief path to this add-on */
  } ATTRIBUTE_PACKED PERIPHERAL_PROPS;

  /*!
   * @brief Peripheral add-on capabilities.
   */
  typedef struct PERIPHERAL_CAPABILITIES
  {
    bool provides_joysticks;            /*!< @brief true if the add-on provides joysticks */
    bool provides_buttonmaps;           /*!< @brief true if the add-on provides button maps */
  } ATTRIBUTE_PACKED PERIPHERAL_CAPABILITIES;
  ///}

  /// @name Event types
  ///{
  typedef enum PERIPHERAL_EVENT_TYPE
  {
    PERIPHERAL_EVENT_TYPE_NONE,           /*!< @brief unknown event */
    PERIPHERAL_EVENT_TYPE_DRIVER_BUTTON,  /*!< @brief state changed for joystick driver button */
    PERIPHERAL_EVENT_TYPE_DRIVER_HAT,     /*!< @brief state changed for joystick driver hat */
    PERIPHERAL_EVENT_TYPE_DRIVER_AXIS,    /*!< @brief state changed for joystick driver axis */
    PERIPHERAL_EVENT_TYPE_SET_MOTOR,      /*!< @brief set the state for joystick rumble motor */
  } PERIPHERAL_EVENT_TYPE;

  typedef enum JOYSTICK_STATE_BUTTON
  {
    JOYSTICK_STATE_BUTTON_UNPRESSED = 0x0,    /*!< @brief button is released */
    JOYSTICK_STATE_BUTTON_PRESSED   = 0x1,    /*!< @brief button is pressed */
  } JOYSTICK_STATE_BUTTON;

  typedef enum JOYSTICK_STATE_HAT
  {
    JOYSTICK_STATE_HAT_UNPRESSED  = 0x0,    /*!< @brief no directions are pressed */
    JOYSTICK_STATE_HAT_LEFT       = 0x1,    /*!< @brief only left is pressed */
    JOYSTICK_STATE_HAT_RIGHT      = 0x2,    /*!< @brief only right is pressed */
    JOYSTICK_STATE_HAT_UP         = 0x4,    /*!< @brief only up is pressed */
    JOYSTICK_STATE_HAT_DOWN       = 0x8,    /*!< @brief only down is pressed */
    JOYSTICK_STATE_HAT_LEFT_UP    = JOYSTICK_STATE_HAT_LEFT  | JOYSTICK_STATE_HAT_UP,
    JOYSTICK_STATE_HAT_LEFT_DOWN  = JOYSTICK_STATE_HAT_LEFT  | JOYSTICK_STATE_HAT_DOWN,
    JOYSTICK_STATE_HAT_RIGHT_UP   = JOYSTICK_STATE_HAT_RIGHT | JOYSTICK_STATE_HAT_UP,
    JOYSTICK_STATE_HAT_RIGHT_DOWN = JOYSTICK_STATE_HAT_RIGHT | JOYSTICK_STATE_HAT_DOWN,
  } JOYSTICK_STATE_HAT;

  /*!
   * @brief value in the closed interval [-1.0, 1.0]
   *
   * The axis state uses the XInput coordinate system:
   *   - Negative values signify down or to the left
   *   - Positive values signify up or to the right
   */
  typedef float JOYSTICK_STATE_AXIS;

  typedef float JOYSTICK_STATE_MOTOR;

  typedef struct PERIPHERAL_EVENT
  {
    unsigned int             peripheral_index;
    PERIPHERAL_EVENT_TYPE    type;
    unsigned int             driver_index;
    JOYSTICK_STATE_BUTTON    driver_button_state;
    JOYSTICK_STATE_HAT       driver_hat_state;
    JOYSTICK_STATE_AXIS      driver_axis_state;
    JOYSTICK_STATE_MOTOR     motor_state;
  } ATTRIBUTE_PACKED PERIPHERAL_EVENT;
  ///}

  /// @name Joystick types
  ///{
  typedef struct JOYSTICK_INFO
  {
    PERIPHERAL_INFO peripheral;         /*!< @brief peripheral info for this joystick */
    char*           provider;           /*!< @brief name of the driver or interface providing the joystick */
    int             requested_port;     /*!< @brief requested port number (such as for 360 controllers), or NO_PORT_REQUESTED */
    unsigned int    button_count;       /*!< @brief number of buttons reported by the driver */
    unsigned int    hat_count;          /*!< @brief number of hats reported by the driver */
    unsigned int    axis_count;         /*!< @brief number of axes reported by the driver */
    unsigned int    motor_count;        /*!< @brief number of motors reported by the driver */
    bool            supports_poweroff;  /*!< @brief whether the joystick supports being powered off */
  } ATTRIBUTE_PACKED JOYSTICK_INFO;

  typedef enum JOYSTICK_DRIVER_PRIMITIVE_TYPE
  {
    JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN,
    JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON,
    JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION,
    JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS,
    JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR,
  } JOYSTICK_DRIVER_PRIMITIVE_TYPE;

  typedef struct JOYSTICK_DRIVER_BUTTON
  {
    int              index;
  } ATTRIBUTE_PACKED JOYSTICK_DRIVER_BUTTON;

  typedef enum JOYSTICK_DRIVER_HAT_DIRECTION
  {
    JOYSTICK_DRIVER_HAT_UNKNOWN,
    JOYSTICK_DRIVER_HAT_LEFT,
    JOYSTICK_DRIVER_HAT_RIGHT,
    JOYSTICK_DRIVER_HAT_UP,
    JOYSTICK_DRIVER_HAT_DOWN,
  } JOYSTICK_DRIVER_HAT_DIRECTION;

  typedef struct JOYSTICK_DRIVER_HAT
  {
    int                           index;
    JOYSTICK_DRIVER_HAT_DIRECTION direction;
  } ATTRIBUTE_PACKED JOYSTICK_DRIVER_HAT;

  typedef enum JOYSTICK_DRIVER_SEMIAXIS_DIRECTION
  {
    JOYSTICK_DRIVER_SEMIAXIS_NEGATIVE = -1, /*!< @brief negative half of the axis */
    JOYSTICK_DRIVER_SEMIAXIS_UNKNOWN  =  0, /*!< @brief unknown direction */
    JOYSTICK_DRIVER_SEMIAXIS_POSITIVE =  1, /*!< @brief positive half of the axis */
  } JOYSTICK_DRIVER_SEMIAXIS_DIRECTION;

  typedef struct JOYSTICK_DRIVER_SEMIAXIS
  {
    int                                index;
    JOYSTICK_DRIVER_SEMIAXIS_DIRECTION direction;
  } ATTRIBUTE_PACKED JOYSTICK_DRIVER_SEMIAXIS;

  typedef struct JOYSTICK_DRIVER_MOTOR
  {
    int              index;
  } ATTRIBUTE_PACKED JOYSTICK_DRIVER_MOTOR;

  typedef struct JOYSTICK_DRIVER_PRIMITIVE
  {
    JOYSTICK_DRIVER_PRIMITIVE_TYPE    type;
    union
    {
      struct JOYSTICK_DRIVER_BUTTON   button;
      struct JOYSTICK_DRIVER_HAT      hat;
      struct JOYSTICK_DRIVER_SEMIAXIS semiaxis;
      struct JOYSTICK_DRIVER_MOTOR    motor;
    };
  } ATTRIBUTE_PACKED JOYSTICK_DRIVER_PRIMITIVE;

  typedef enum JOYSTICK_FEATURE_TYPE
  {
    JOYSTICK_FEATURE_TYPE_UNKNOWN,
    JOYSTICK_FEATURE_TYPE_SCALAR,
    JOYSTICK_FEATURE_TYPE_ANALOG_STICK,
    JOYSTICK_FEATURE_TYPE_ACCELEROMETER,
    JOYSTICK_FEATURE_TYPE_MOTOR,
  } JOYSTICK_FEATURE_TYPE;

  typedef enum JOYSTICK_FEATURE_PRIMITIVE
  {
    // Scalar feature
    JOYSTICK_SCALAR_PRIMITIVE = 0,

    // Analog stick
    JOYSTICK_ANALOG_STICK_UP = 0,
    JOYSTICK_ANALOG_STICK_DOWN = 1,
    JOYSTICK_ANALOG_STICK_RIGHT = 2,
    JOYSTICK_ANALOG_STICK_LEFT = 3,

    // Accelerometer
    JOYSTICK_ACCELEROMETER_POSITIVE_X = 0,
    JOYSTICK_ACCELEROMETER_POSITIVE_Y = 1,
    JOYSTICK_ACCELEROMETER_POSITIVE_Z = 2,

    // Motor
    JOYSTICK_MOTOR_PRIMITIVE = 0,

    // Maximum number of primitives
    JOYSTICK_PRIMITIVE_MAX = 4,
  } JOYSTICK_FEATURE_PRIMITIVE;

  typedef struct JOYSTICK_FEATURE
  {
    char*                                   name;
    JOYSTICK_FEATURE_TYPE                   type;
    struct JOYSTICK_DRIVER_PRIMITIVE        primitives[JOYSTICK_PRIMITIVE_MAX];
  } ATTRIBUTE_PACKED JOYSTICK_FEATURE;
  ///}

  //! @todo Mouse, light gun, multitouch
typedef struct sAddonToKodiFuncTable_Peripheral
{
  void* kodiInstance;
  void (*TriggerScan)(void* kodiInstance);
  void (*RefreshButtonMaps)(void* kodiInstance, const char* deviceName, const char* controllerId);
  unsigned int (*FeatureCount)(void* kodiInstance, const char* controllerId, JOYSTICK_FEATURE_TYPE type);
} sAddonToKodiFuncTable_Peripheral;

typedef struct sKodiToAddonFuncTable_Peripheral
{
  void (__cdecl* GetCapabilities)(void* addonInstance, PERIPHERAL_CAPABILITIES* capabilities);
  PERIPHERAL_ERROR (__cdecl* PerformDeviceScan)(void* addonInstance, unsigned int*, PERIPHERAL_INFO**);
  void (__cdecl* FreeScanResults)(void* addonInstance, unsigned int, PERIPHERAL_INFO*);
  PERIPHERAL_ERROR (__cdecl* GetEvents)(void* addonInstance, unsigned int*, PERIPHERAL_EVENT**);
  void (__cdecl* FreeEvents)(void* addonInstance, unsigned int, PERIPHERAL_EVENT*);
  bool (__cdecl* SendEvent)(void* addonInstance, const PERIPHERAL_EVENT*);

  /// @name Joystick operations
  ///{
  PERIPHERAL_ERROR (__cdecl* GetJoystickInfo)(void* addonInstance, unsigned int, JOYSTICK_INFO*);
  void (__cdecl* FreeJoystickInfo)(void* addonInstance, JOYSTICK_INFO*);
  PERIPHERAL_ERROR (__cdecl* GetFeatures)(void* addonInstance, const JOYSTICK_INFO*, const char*, unsigned int*, JOYSTICK_FEATURE**);
  void (__cdecl* FreeFeatures)(void* addonInstance, unsigned int, JOYSTICK_FEATURE*);
  PERIPHERAL_ERROR (__cdecl* MapFeatures)(void* addonInstance, const JOYSTICK_INFO*, const char*, unsigned int, JOYSTICK_FEATURE*);
  void (__cdecl* SaveButtonMap)(void* addonInstance, const JOYSTICK_INFO*);
  void (__cdecl* ResetButtonMap)(void* addonInstance, const JOYSTICK_INFO*, const char*);
  void (__cdecl* PowerOffJoystick)(void* addonInstance, unsigned int);
  ///}
} sKodiToAddonFuncTable_Peripheral;

typedef struct sAddonInstance_Peripheral
{
  PERIPHERAL_PROPS props;
  sAddonToKodiFuncTable_Peripheral toKodi;
  sKodiToAddonFuncTable_Peripheral toAddon;
} sAddonInstance_Peripheral;

#ifdef __cplusplus
} /* extern "C" */

namespace kodi {
namespace addon {
namespace peripheral {

  /*!
   * Utility class to manipulate arrays of peripheral types.
   */
  template <class THE_CLASS, typename THE_STRUCT>
  class PeripheralVector
  {
  public:
    static void ToStructs(const std::vector<THE_CLASS>& vecObjects, THE_STRUCT** pStructs)
    {
      if (!pStructs)
        return;

      if (vecObjects.empty())
      {
        *pStructs = NULL;
      }
      else
      {
        (*pStructs) = new THE_STRUCT[vecObjects.size()];
        for (unsigned int i = 0; i < vecObjects.size(); i++)
          vecObjects.at(i).ToStruct((*pStructs)[i]);
      }
    }

    static void ToStructs(const std::vector<THE_CLASS*>& vecObjects, THE_STRUCT** pStructs)
    {
      if (!pStructs)
        return;

      if (vecObjects.empty())
      {
        *pStructs = NULL;
      }
      else
      {
        *pStructs = new THE_STRUCT[vecObjects.size()];
        for (unsigned int i = 0; i < vecObjects.size(); i++)
          vecObjects.at(i)->ToStruct((*pStructs)[i]);
      }
    }

    static void FreeStructs(unsigned int structCount, THE_STRUCT* structs)
    {
      if (structs)
      {
        for (unsigned int i = 0; i < structCount; i++)
          THE_CLASS::FreeStruct(structs[i]);
      }
      PERIPHERAL_SAFE_DELETE_ARRAY(structs);
    }
  };

  /*!
   * Peripheral
   *
   * Wrapper class providing peripheral information. Classes can extend
   * Peripheral to inherit peripheral properties.
   */
  class Peripheral
  {
  public:
    Peripheral(PERIPHERAL_TYPE type = PERIPHERAL_TYPE_UNKNOWN, const std::string& strName = "") :
      m_type(type),
      m_strName(strName),
      m_vendorId(0),
      m_productId(0),
      m_index(0)
    {
    }

    Peripheral(const PERIPHERAL_INFO& info) :
      m_type(info.type),
      m_strName(info.name ? info.name : ""),
      m_vendorId(info.vendor_id),
      m_productId(info.product_id),
      m_index(info.index)
    {
    }

    virtual ~Peripheral(void) { }

    PERIPHERAL_TYPE    Type(void) const      { return m_type; }
    const std::string& Name(void) const      { return m_strName; }
    uint16_t           VendorID(void) const  { return m_vendorId; }
    uint16_t           ProductID(void) const { return m_productId; }
    unsigned int       Index(void) const     { return m_index; }

    // Derived property: VID and PID are 0x0000 if unknown
    bool IsVidPidKnown(void) const { return m_vendorId != 0 || m_productId != 0; }

    void SetType(PERIPHERAL_TYPE type)       { m_type      = type; }
    void SetName(const std::string& strName) { m_strName   = strName; }
    void SetVendorID(uint16_t vendorId)      { m_vendorId  = vendorId; }
    void SetProductID(uint16_t productId)    { m_productId = productId; }
    void SetIndex(unsigned int index)        { m_index     = index; }

    void ToStruct(PERIPHERAL_INFO& info) const
    {
      info.type       = m_type;
      info.name       = new char[m_strName.size() + 1];
      info.vendor_id  = m_vendorId;
      info.product_id = m_productId;
      info.index      = m_index;

      std::strcpy(info.name, m_strName.c_str());
    }

    static void FreeStruct(PERIPHERAL_INFO& info)
    {
      PERIPHERAL_SAFE_DELETE_ARRAY(info.name);
    }

  private:
    PERIPHERAL_TYPE  m_type;
    std::string      m_strName;
    uint16_t         m_vendorId;
    uint16_t         m_productId;
    unsigned int     m_index;
  };

  typedef PeripheralVector<Peripheral, PERIPHERAL_INFO> Peripherals;

  /*!
   * PeripheralEvent
   *
   * Wrapper class for peripheral events.
   */
  class PeripheralEvent
  {
  public:
    PeripheralEvent(void) :
      m_event()
    {
    }

    PeripheralEvent(unsigned int peripheralIndex, unsigned int buttonIndex, JOYSTICK_STATE_BUTTON state) :
      m_event()
    {
      SetType(PERIPHERAL_EVENT_TYPE_DRIVER_BUTTON);
      SetPeripheralIndex(peripheralIndex);
      SetDriverIndex(buttonIndex);
      SetButtonState(state);
    }

    PeripheralEvent(unsigned int peripheralIndex, unsigned int hatIndex, JOYSTICK_STATE_HAT state) :
      m_event()
    {
      SetType(PERIPHERAL_EVENT_TYPE_DRIVER_HAT);
      SetPeripheralIndex(peripheralIndex);
      SetDriverIndex(hatIndex);
      SetHatState(state);
    }

    PeripheralEvent(unsigned int peripheralIndex, unsigned int axisIndex, JOYSTICK_STATE_AXIS state) :
      m_event()
    {
      SetType(PERIPHERAL_EVENT_TYPE_DRIVER_AXIS);
      SetPeripheralIndex(peripheralIndex);
      SetDriverIndex(axisIndex);
      SetAxisState(state);
    }

    PeripheralEvent(const PERIPHERAL_EVENT& event) :
      m_event(event)
    {
    }

    PERIPHERAL_EVENT_TYPE Type(void) const            { return m_event.type; }
    unsigned int          PeripheralIndex(void) const { return m_event.peripheral_index; }
    unsigned int          DriverIndex(void) const     { return m_event.driver_index; }
    JOYSTICK_STATE_BUTTON ButtonState(void) const     { return m_event.driver_button_state; }
    JOYSTICK_STATE_HAT    HatState(void) const        { return m_event.driver_hat_state; }
    JOYSTICK_STATE_AXIS   AxisState(void) const       { return m_event.driver_axis_state; }
    JOYSTICK_STATE_MOTOR  MotorState(void) const      { return m_event.motor_state; }

    void SetType(PERIPHERAL_EVENT_TYPE type)         { m_event.type                = type; }
    void SetPeripheralIndex(unsigned int index)      { m_event.peripheral_index    = index; }
    void SetDriverIndex(unsigned int index)          { m_event.driver_index        = index; }
    void SetButtonState(JOYSTICK_STATE_BUTTON state) { m_event.driver_button_state = state; }
    void SetHatState(JOYSTICK_STATE_HAT state)       { m_event.driver_hat_state    = state; }
    void SetAxisState(JOYSTICK_STATE_AXIS state)     { m_event.driver_axis_state   = state; }
    void SetMotorState(JOYSTICK_STATE_MOTOR state)   { m_event.motor_state         = state; }

    void ToStruct(PERIPHERAL_EVENT& event) const
    {
      event = m_event;
    }

    static void FreeStruct(PERIPHERAL_EVENT& event)
    {
      (void)event;
    }

  private:
    PERIPHERAL_EVENT m_event;
  };

  typedef PeripheralVector<PeripheralEvent, PERIPHERAL_EVENT> PeripheralEvents;

  /*!
   * Joystick
   *
   * Wrapper class providing additional joystick information not provided by
   * Peripheral.
   */
  class Joystick : public Peripheral
  {
  public:
    Joystick(const std::string& provider = "", const std::string& strName = "") :
      Peripheral(PERIPHERAL_TYPE_JOYSTICK, strName),
      m_provider(provider),
      m_requestedPort(NO_PORT_REQUESTED),
      m_buttonCount(0),
      m_hatCount(0),
      m_axisCount(0),
      m_motorCount(0),
      m_supportsPowerOff(false)
    {
    }

    Joystick(const Joystick& other)
    {
      *this = other;
    }

    Joystick(const JOYSTICK_INFO& info) :
      Peripheral(info.peripheral),
      m_provider(info.provider ? info.provider : ""),
      m_requestedPort(info.requested_port),
      m_buttonCount(info.button_count),
      m_hatCount(info.hat_count),
      m_axisCount(info.axis_count),
      m_motorCount(info.motor_count),
      m_supportsPowerOff(info.supports_poweroff)
    {
    }

    virtual ~Joystick(void) { }

    Joystick& operator=(const Joystick& rhs)
    {
      if (this != &rhs)
      {
        Peripheral::operator=(rhs);

        m_provider      = rhs.m_provider;
        m_requestedPort = rhs.m_requestedPort;
        m_buttonCount   = rhs.m_buttonCount;
        m_hatCount      = rhs.m_hatCount;
        m_axisCount     = rhs.m_axisCount;
        m_motorCount    = rhs.m_motorCount;
        m_supportsPowerOff = rhs.m_supportsPowerOff;
      }
      return *this;
    }

    const std::string& Provider(void) const      { return m_provider; }
    int                RequestedPort(void) const { return m_requestedPort; }
    unsigned int       ButtonCount(void) const   { return m_buttonCount; }
    unsigned int       HatCount(void) const      { return m_hatCount; }
    unsigned int       AxisCount(void) const     { return m_axisCount; }
    unsigned int       MotorCount(void) const    { return m_motorCount; }
    bool               SupportsPowerOff(void) const { return m_supportsPowerOff; }

    // Derived property: Counts are unknown if all are zero
    bool AreElementCountsKnown(void) const { return m_buttonCount != 0 || m_hatCount != 0 || m_axisCount != 0; }

    void SetProvider(const std::string& provider)     { m_provider      = provider; }
    void SetRequestedPort(int requestedPort)          { m_requestedPort = requestedPort; }
    void SetButtonCount(unsigned int buttonCount)     { m_buttonCount   = buttonCount; }
    void SetHatCount(unsigned int hatCount)           { m_hatCount      = hatCount; }
    void SetAxisCount(unsigned int axisCount)         { m_axisCount     = axisCount; }
    void SetMotorCount(unsigned int motorCount)       { m_motorCount    = motorCount; }
    void SetSupportsPowerOff(bool supportsPowerOff)   { m_supportsPowerOff = supportsPowerOff; }

    void ToStruct(JOYSTICK_INFO& info) const
    {
      Peripheral::ToStruct(info.peripheral);

      info.provider       = new char[m_provider.size() + 1];
      info.requested_port = m_requestedPort;
      info.button_count   = m_buttonCount;
      info.hat_count      = m_hatCount;
      info.axis_count     = m_axisCount;
      info.motor_count    = m_motorCount;
      info.supports_poweroff = m_supportsPowerOff;

      std::strcpy(info.provider, m_provider.c_str());
    }

    static void FreeStruct(JOYSTICK_INFO& info)
    {
      Peripheral::FreeStruct(info.peripheral);

      PERIPHERAL_SAFE_DELETE_ARRAY(info.provider);
    }

  private:
    std::string                   m_provider;
    int                           m_requestedPort;
    unsigned int                  m_buttonCount;
    unsigned int                  m_hatCount;
    unsigned int                  m_axisCount;
    unsigned int                  m_motorCount;
    bool                          m_supportsPowerOff;
  };

  typedef PeripheralVector<Joystick, JOYSTICK_INFO> Joysticks;

  /*!
   * DriverPrimitive
   *
   * Base class for joystick driver primitives. A driver primitive can be:
   *
   *   1) a button
   *   2) a hat direction
   *   3) a semiaxis (either the positive or negative half of an axis)
   *   4) a motor
   *
   * The type determines the fields in use:
   *
   *    Button:
   *       - driver index
   *
   *    Hat direction:
   *       - driver index
   *       - hat direction
   *
   *    Semiaxis:
   *       - driver index
   *       - semiaxis direction
   *
   *    Motor:
   *       - driver index
   */
  struct DriverPrimitive
  {
  protected:
    /*!
     * \brief Construct a driver primitive of the specified type
     */
    DriverPrimitive(JOYSTICK_DRIVER_PRIMITIVE_TYPE type, unsigned int driverIndex) :
      m_type(type),
      m_driverIndex(driverIndex),
      m_hatDirection(JOYSTICK_DRIVER_HAT_UNKNOWN),
      m_semiAxisDirection(JOYSTICK_DRIVER_SEMIAXIS_UNKNOWN)
    {
    }

  public:
    /*!
     * \brief Construct an invalid driver primitive
     */
    DriverPrimitive(void) :
      m_type(JOYSTICK_DRIVER_PRIMITIVE_TYPE_UNKNOWN),
      m_driverIndex(0),
      m_hatDirection(JOYSTICK_DRIVER_HAT_UNKNOWN),
      m_semiAxisDirection(JOYSTICK_DRIVER_SEMIAXIS_UNKNOWN)
    {
    }

    /*!
     * \brief Construct a driver primitive representing a button
     */
    static DriverPrimitive CreateButton(unsigned int buttonIndex)
    {
      return DriverPrimitive(JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON, buttonIndex);
    }

    /*!
     * \brief Construct a driver primitive representing one of the four direction
     *        arrows on a dpad
     */
    DriverPrimitive(unsigned int hatIndex, JOYSTICK_DRIVER_HAT_DIRECTION direction) :
      m_type(JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION),
      m_driverIndex(hatIndex),
      m_hatDirection(direction),
      m_semiAxisDirection(JOYSTICK_DRIVER_SEMIAXIS_UNKNOWN)
    {
    }

    /*!
     * \brief Construct a driver primitive representing the positive or negative
     *        half of an axis
     */
    DriverPrimitive(unsigned int axisIndex, JOYSTICK_DRIVER_SEMIAXIS_DIRECTION direction) :
      m_type(JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS),
      m_driverIndex(axisIndex),
      m_hatDirection(JOYSTICK_DRIVER_HAT_UNKNOWN),
      m_semiAxisDirection(direction)
    {
    }

    /*!
     * \brief Construct a driver primitive representing a motor
     */
    static DriverPrimitive CreateMotor(unsigned int motorIndex)
    {
      return DriverPrimitive(JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR, motorIndex);
    }

    DriverPrimitive(const JOYSTICK_DRIVER_PRIMITIVE& primitive) :
      m_type(primitive.type),
      m_driverIndex(0),
      m_hatDirection(JOYSTICK_DRIVER_HAT_UNKNOWN),
      m_semiAxisDirection(JOYSTICK_DRIVER_SEMIAXIS_UNKNOWN)
    {
      switch (m_type)
      {
        case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
        {
          m_driverIndex = primitive.button.index;
          break;
        }
        case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
        {
          m_driverIndex  = primitive.hat.index;
          m_hatDirection = primitive.hat.direction;
          break;
        }
        case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
        {
          m_driverIndex       = primitive.semiaxis.index;
          m_semiAxisDirection = primitive.semiaxis.direction;
          break;
        }
        case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR:
        {
          m_driverIndex = primitive.motor.index;
          break;
        }
        default:
          break;
      }
    }

    JOYSTICK_DRIVER_PRIMITIVE_TYPE     Type(void) const { return m_type; }
    unsigned int                       DriverIndex(void) const { return m_driverIndex; }
    JOYSTICK_DRIVER_HAT_DIRECTION      HatDirection(void) const { return m_hatDirection; }
    JOYSTICK_DRIVER_SEMIAXIS_DIRECTION SemiAxisDirection(void) const { return m_semiAxisDirection; }

    bool operator==(const DriverPrimitive& other) const
    {
      if (m_type == other.m_type)
      {
        switch (m_type)
        {
          case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
          case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR:
          {
            return m_driverIndex == other.m_driverIndex;
          }
          case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
          {
            return m_driverIndex  == other.m_driverIndex &&
                   m_hatDirection == other.m_hatDirection;
          }
          case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
          {
            return m_driverIndex       == other.m_driverIndex &&
                   m_semiAxisDirection == other.m_semiAxisDirection;
          }
          default:
            break;
        }
      }
      return false;
    }

    void ToStruct(JOYSTICK_DRIVER_PRIMITIVE& driver_primitive) const
    {
      driver_primitive.type = m_type;
      switch (m_type)
      {
        case JOYSTICK_DRIVER_PRIMITIVE_TYPE_BUTTON:
        {
          driver_primitive.button.index = m_driverIndex;
          break;
        }
        case JOYSTICK_DRIVER_PRIMITIVE_TYPE_HAT_DIRECTION:
        {
          driver_primitive.hat.index     = m_driverIndex;
          driver_primitive.hat.direction = m_hatDirection;
          break;
        }
        case JOYSTICK_DRIVER_PRIMITIVE_TYPE_SEMIAXIS:
        {
          driver_primitive.semiaxis.index     = m_driverIndex;
          driver_primitive.semiaxis.direction = m_semiAxisDirection;
          break;
        }
        case JOYSTICK_DRIVER_PRIMITIVE_TYPE_MOTOR:
        {
          driver_primitive.motor.index = m_driverIndex;
          break;
        }
        default:
          break;
      }
    }

  private:
    JOYSTICK_DRIVER_PRIMITIVE_TYPE     m_type;
    unsigned int                       m_driverIndex;
    JOYSTICK_DRIVER_HAT_DIRECTION      m_hatDirection;
    JOYSTICK_DRIVER_SEMIAXIS_DIRECTION m_semiAxisDirection;
  };

  /*!
   * JoystickFeature
   *
   * Class for joystick features. A feature can be:
   *
   *   1) scalar[1]
   *   2) analog stick
   *   3) accelerometer
   *   4) motor
   *
   * [1] All three driver primitives (buttons, hats and axes) have a state that
   *     can be represented using a single scalar value. For this reason,
   *     features that map to a single primitive are called "scalar features".
   */
  class JoystickFeature
  {
  public:
    JoystickFeature(const std::string& name = "", JOYSTICK_FEATURE_TYPE type = JOYSTICK_FEATURE_TYPE_UNKNOWN) :
      m_name(name),
      m_type(type)
    {
    }

    JoystickFeature(const JoystickFeature& other)
    {
      *this = other;
    }

    JoystickFeature(const JOYSTICK_FEATURE& feature) :
      m_name(feature.name ? feature.name : ""),
      m_type(feature.type)
    {
      for (unsigned int i = 0; i < JOYSTICK_PRIMITIVE_MAX; i++)
        m_primitives[i] = feature.primitives[i];
    }

    JoystickFeature& operator=(const JoystickFeature& rhs)
    {
      if (this != &rhs)
      {
        m_name = rhs.m_name;
        m_type = rhs.m_type;
        m_primitives = rhs.m_primitives;
      }
      return *this;
    }

    bool operator==(const JoystickFeature& other) const
    {
      return m_name == other.m_name &&
             m_type == other.m_type &&
             m_primitives == other.m_primitives;
    }

    const std::string& Name(void) const { return m_name; }
    JOYSTICK_FEATURE_TYPE Type(void) const { return m_type; }
    bool IsValid() const { return m_type != JOYSTICK_FEATURE_TYPE_UNKNOWN; }

    void SetName(const std::string& name) { m_name = name; }
    void SetType(JOYSTICK_FEATURE_TYPE type) { m_type = type; }
    void SetInvalid(void) { m_type = JOYSTICK_FEATURE_TYPE_UNKNOWN; }

    const DriverPrimitive& Primitive(JOYSTICK_FEATURE_PRIMITIVE which) const { return m_primitives[which]; }
    void SetPrimitive(JOYSTICK_FEATURE_PRIMITIVE which, const DriverPrimitive& primitive) { m_primitives[which] = primitive; }

    std::array<DriverPrimitive, JOYSTICK_PRIMITIVE_MAX>& Primitives() { return m_primitives; }
    const std::array<DriverPrimitive, JOYSTICK_PRIMITIVE_MAX>& Primitives() const { return m_primitives; }

    void ToStruct(JOYSTICK_FEATURE& feature) const
    {
      feature.name = new char[m_name.length() + 1];
      feature.type = m_type;
      for (unsigned int i = 0; i < JOYSTICK_PRIMITIVE_MAX; i++)
        m_primitives[i].ToStruct(feature.primitives[i]);

      std::strcpy(feature.name, m_name.c_str());
    }

    static void FreeStruct(JOYSTICK_FEATURE& feature)
    {
      PERIPHERAL_SAFE_DELETE_ARRAY(feature.name);
    }

  private:
    std::string                  m_name;
    JOYSTICK_FEATURE_TYPE        m_type;
    std::array<DriverPrimitive, JOYSTICK_PRIMITIVE_MAX> m_primitives;
  };

  typedef PeripheralVector<JoystickFeature, JOYSTICK_FEATURE> JoystickFeatures;


#ifdef __cplusplus
extern "C" {
#endif

  class CAddon
  {
  public:
    //==========================================================================
    /// @brief Class constructor
    ///
    /// @param[in] instance             The from Kodi given instance given be
    ///                                 add-on CreateInstance call with instance
    ///                                 id ADDON_INSTANCE_PERIPHERAL.
    ///
    CAddon(void* instance)
      : m_instance(static_cast<sAddonInstance_Peripheral*>(instance))
    {
      m_instance->toAddon.GetCapabilities = ADDON_GetCapabilities;
      m_instance->toAddon.PerformDeviceScan = ADDON_PerformDeviceScan;
      m_instance->toAddon.FreeScanResults = ADDON_FreeScanResults;
      m_instance->toAddon.GetEvents = ADDON_GetEvents;
      m_instance->toAddon.FreeEvents = ADDON_FreeEvents;
      m_instance->toAddon.SendEvent = ADDON_SendEvent;

      m_instance->toAddon.GetJoystickInfo = ADDON_GetJoystickInfo;
      m_instance->toAddon.FreeJoystickInfo = ADDON_FreeJoystickInfo;
      m_instance->toAddon.GetFeatures = ADDON_GetFeatures;
      m_instance->toAddon.FreeFeatures = ADDON_FreeFeatures;
      m_instance->toAddon.MapFeatures = ADDON_MapFeatures;
      m_instance->toAddon.SaveButtonMap = ADDON_SaveButtonMap;
      m_instance->toAddon.ResetButtonMap = ADDON_ResetButtonMap;
      m_instance->toAddon.PowerOffJoystick = ADDON_PowerOffJoystick;
    }
    //--------------------------------------------------------------------------

    /// @name Peripheral operations
    ///{

    //==========================================================================
    ///
    /// @brief Get the list of features that this add-on provides
    ///
    /// @param[out] capabilities        The add-on's capabilities.
    /// @remarks                        Valid implementation required.
    ///
    /// Called by the frontend to query the add-on's capabilities and supported
    /// peripherals. All capabilities that the add-on supports should be set to
    /// true.
    ///
    ///
    virtual void GetCapabilities(PERIPHERAL_CAPABILITIES& capabilities)=0;
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @brief Perform a scan for joysticks
    ///
    /// @param[in] peripheral_count     Assigned to the number of peripherals allocated
    /// @param[in] scan_results         Assigned to allocated memory
    /// @return                         PERIPHERAL_NO_ERROR if successful
    ///
    /// The frontend calls this when a hardware change is detected. If an add-on
    /// detects a hardware change, it can trigger this function using the
    /// TriggerScan() callback.
    ///
    virtual PERIPHERAL_ERROR PerformDeviceScan(std::vector<Peripheral*>& scan_results) { return PERIPHERAL_ERROR_NOT_IMPLEMENTED; }
    //--------------------------------------------------------------------------

    ///
    /// @brief Get all events that have occurred since the last call to GetEvents()
    ///
    /// @return                         PERIPHERAL_NO_ERROR if successful
    ///
    virtual PERIPHERAL_ERROR GetEvents(std::vector<PeripheralEvent>& events) { return  PERIPHERAL_ERROR_NOT_IMPLEMENTED; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @brief Send an input event to the specified peripheral
    ///
    /// @param[in] event                The input event
    /// @return                         true if the event was handled, false
    ///                                 otherwise
    ///
    virtual bool SendEvent(const PERIPHERAL_EVENT& event) { return false; }
    //--------------------------------------------------------------------------
    ///}

    /// @name Joystick operations
    ///
    /// @note add provides_joysticks="true" to the kodi.peripheral extension
    /// point node in addon.xml.
    ///
    ///{

    //==========================================================================
    ///
    /// @brief Get extended info about an attached joystick
    /// @param[in] index        The joystick's driver index
    /// @param[out] info        The container for the allocated joystick info
    /// @return                 PERIPHERAL_NO_ERROR if successful
    ///
    virtual PERIPHERAL_ERROR GetJoystickInfo(unsigned int index, Joystick& info) { return PERIPHERAL_ERROR_NOT_IMPLEMENTED; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @brief Get the features that allow translating the joystick into the controller profile
    /// @param[in] joystick             The device's joystick properties; unknown values may be left at their default
    /// @param[in] controller_id        The controller profile being requested, e.g. game.controller.default
    /// @param[out] features            The array of allocated features
    /// @return                         PERIPHERAL_NO_ERROR if successful
    ///
    virtual PERIPHERAL_ERROR GetFeatures(const Joystick& joystick,
                                         const std::string& controller_id,
                                         std::vector<JoystickFeature>& features) { return PERIPHERAL_ERROR_NOT_IMPLEMENTED; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @brief Add or update joystick features
    ///
    /// @param[in] joystick             The device's joystick properties;
    ///                                 unknown values may be left at their
    ///                                 default
    /// @param[in] controller_id        The game controller profile being updated
    /// @param[in] features             The array of features
    /// @return                         PERIPHERAL_NO_ERROR if successful
    ///
    virtual PERIPHERAL_ERROR MapFeatures(const Joystick& joystick,
                                         const std::string& controller_id,
                                         const std::vector<JoystickFeature>& features) { return PERIPHERAL_ERROR_NOT_IMPLEMENTED; }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @brief Save the button map for the given joystick
    ///
    /// @param[in] joystick             The device's joystick properties
    ///
    virtual void SaveButtonMap(const Joystick& joystick) { }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @brief Reset the button map for the given joystick and controller profile ID
    ///
    /// @param[in] joystick             The device's joystick properties
    /// @param[in] controller_id        The game controller profile being reset
    ///
    virtual void ResetButtonMap(const Joystick& joystick, std::string controller_id) { }
    //--------------------------------------------------------------------------

    //==========================================================================
    ///
    /// @brief Powers off the given joystick if supported
    ///
    /// @param[in] index                The joystick's driver index
    ///
    virtual void PowerOffJoystick(unsigned int index) { }
    //--------------------------------------------------------------------------
    ///}

    //==========================================================================
    ///
    /// @brief Trigger a scan for peripherals
    ///
    /// The add-on calls this if a change in hardware is detected.
    ///
    void TriggerScan(void)
    {
      return m_instance->toKodi.TriggerScan(m_instance->toKodi.kodiInstance);
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Notify the frontend that button maps have changed
    ///
    /// @param[in] deviceName           [optional] The name of the device to
    ///                                 refresh, or empty/null for all devices
    /// @param[in] controllerId         [optional] The controller ID to refresh,
    ///                                 or empty/null for all controllers
    ///
    void RefreshButtonMaps(const std::string& strDeviceName = "", const std::string& strControllerId = "")
    {
      return m_instance->toKodi.RefreshButtonMaps(m_instance->toKodi.kodiInstance, strDeviceName.c_str(), strControllerId.c_str());
    }
    //--------------------------------------------------------------------------

    //==========================================================================
    /// @brief Return the number of features belonging to the specified
    /// controller
    ///
    /// @param[in] controllerId         The controller ID to enumerate
    /// @param[in] type                 [optional]  Type to filter by, or 
    ///                                 JOYSTICK_FEATURE_TYPE_UNKNOWN for all
    ///                                 features
    ///
    /// @return The number of features matching the request parameters
    ///
    unsigned int FeatureCount(const std::string& strControllerId, JOYSTICK_FEATURE_TYPE type = JOYSTICK_FEATURE_TYPE_UNKNOWN)
    {
      return m_instance->toKodi.FeatureCount(m_instance->toKodi.kodiInstance, strControllerId.c_str(), type);
    }
    //--------------------------------------------------------------------------
  
  private:
    static inline void ADDON_GetCapabilities(void* addonInstance, PERIPHERAL_CAPABILITIES* capabilities)
    {
      if (!capabilities || !addonInstance)
        throw std::logic_error("GetCapabilities() called from Kodi with empty pointer");

      static_cast<CAddon*>(addonInstance)->GetCapabilities(*capabilities);
    }

    static inline PERIPHERAL_ERROR ADDON_PerformDeviceScan(void* addonInstance, 
                                                     unsigned int* peripheral_count, 
                                                     PERIPHERAL_INFO** scan_results)
    {
      if (!scan_results || !peripheral_count || !addonInstance)
        throw std::logic_error("PerformDeviceScan() called from Kodi with empty pointer");
  
      std::vector<Peripheral*> peripherals;
      PERIPHERAL_ERROR error = static_cast<CAddon*>(addonInstance)->PerformDeviceScan(peripherals);
      if (error == PERIPHERAL_NO_ERROR)
      {
        *peripheral_count = peripherals.size();
        Peripherals::ToStructs(peripherals, scan_results);
      }
      return error;
    }

    static inline void ADDON_FreeScanResults(void* addonInstance, unsigned int peripheral_count, PERIPHERAL_INFO* scan_results)
    {
      if (!scan_results || !addonInstance)
        throw std::logic_error("FreeScanResults() called from Kodi with empty pointer");

      Peripherals::FreeStructs(peripheral_count, scan_results);
    }

    static inline PERIPHERAL_ERROR ADDON_GetEvents(void* addonInstance, unsigned int* event_count, PERIPHERAL_EVENT** events)
    {
      if (!event_count || !events || !addonInstance)
        throw std::logic_error("GetEvents() called from Kodi with empty pointer");

      std::vector<PeripheralEvent> peripheralEvents;
      PERIPHERAL_ERROR error = static_cast<CAddon*>(addonInstance)->GetEvents(peripheralEvents);
      if (error == PERIPHERAL_NO_ERROR)
      {
        *event_count = peripheralEvents.size();
        PeripheralEvents::ToStructs(peripheralEvents, events);
      }
      return error;
    }

    static inline void ADDON_FreeEvents(void* addonInstance, unsigned int event_count, PERIPHERAL_EVENT* events)
    {
      if (!events || !addonInstance)
        throw std::logic_error("FreeEvents() called from Kodi with empty pointer");

      PeripheralEvents::FreeStructs(event_count, events);
    }

    static inline bool ADDON_SendEvent(void* addonInstance, const PERIPHERAL_EVENT* event)
    {
      if (!event || !addonInstance)
        throw std::logic_error("SendEvent() called from Kodi with empty pointer");

      return static_cast<CAddon*>(addonInstance)->SendEvent(*event);
    } 

    static inline PERIPHERAL_ERROR ADDON_GetJoystickInfo(void* addonInstance, unsigned int index, JOYSTICK_INFO* info)
    {
      if (!info || !addonInstance)
        throw std::logic_error("GetJoystickInfo() called from Kodi with empty pointer");

      Joystick* joystick = nullptr;
      PERIPHERAL_ERROR error = static_cast<CAddon*>(addonInstance)->GetJoystickInfo(index, *joystick);
      if (error == PERIPHERAL_NO_ERROR && joystick)
      {
        joystick->ToStruct(*info);
      }
    }

    static inline void ADDON_FreeJoystickInfo(void* addonInstance, JOYSTICK_INFO* info)
    {
      if (!info || !addonInstance)
        throw std::logic_error("FreeJoystickInfo() called from Kodi with empty pointer");

      Joystick::FreeStruct(*info);
    }

    static inline PERIPHERAL_ERROR ADDON_GetFeatures(void* addonInstance,
                                               const JOYSTICK_INFO* joystick,
                                               const char* controller_id,
                                               unsigned int* feature_count,
                                               JOYSTICK_FEATURE** features)
    {
      if (!joystick || !controller_id || !feature_count || !features)
        throw std::logic_error("GetFeatures() called from Kodi with empty pointer");

      std::vector<JoystickFeature> featureVector;
      PERIPHERAL_ERROR error = static_cast<CAddon*>(addonInstance)->GetFeatures(Joystick(*joystick), controller_id, featureVector);
      if (error == PERIPHERAL_NO_ERROR)
      {
        *feature_count = featureVector.size();
        JoystickFeatures::ToStructs(featureVector, features);
      }
      return error;
    }

    static inline void ADDON_FreeFeatures(void* addonInstance,
                                    unsigned int feature_count,
                                    JOYSTICK_FEATURE* features)
    {
      if (!addonInstance || !feature_count || !features)
        throw std::logic_error("FreeFeatures() called from Kodi with empty pointer");

      JoystickFeatures::FreeStructs(feature_count, features);
    }

    static inline PERIPHERAL_ERROR ADDON_MapFeatures(void* addonInstance,
                                               const JOYSTICK_INFO* joystick,
                                               const char* controller_id,
                                               unsigned int feature_count,
                                               JOYSTICK_FEATURE* features)
    {
      if (!joystick || !controller_id || (feature_count > 0 && !features))
        throw std::logic_error("MapFeatures() called from Kodi with empty pointer or invalid parameters");

      std::vector<JoystickFeature> featureVector(features, features + feature_count);
      return static_cast<CAddon*>(addonInstance)->MapFeatures(Joystick(*joystick), controller_id, featureVector);
    }

    static inline void ADDON_SaveButtonMap(void* addonInstance,
                                     const JOYSTICK_INFO* joystick)
    {
      if (!addonInstance || !joystick)
        throw std::logic_error("SaveButtonMap() called from Kodi with empty pointer");

      Joystick addonJoystick(*joystick);
      static_cast<CAddon*>(addonInstance)->SaveButtonMap(addonJoystick);
    }

    static inline void ADDON_ResetButtonMap(void* addonInstance,
                                      const JOYSTICK_INFO* joystick,
                                      const char* controller_id)
    {
      if (!addonInstance || !joystick || !controller_id)
        throw std::logic_error("ResetButtonMap() called from Kodi with empty pointer");

      Joystick addonJoystick(*joystick);
      static_cast<CAddon*>(addonInstance)->ResetButtonMap(addonJoystick, controller_id);
    }

    static inline void ADDON_PowerOffJoystick(void* addonInstance,
                                        unsigned int index)
    {
      if (!addonInstance)
        throw std::logic_error("PowerOffJoystick() called from Kodi with empty pointer");

      static_cast<CAddon*>(addonInstance)->PowerOffJoystick(index);
    }
  
    sAddonInstance_Peripheral* m_instance;
  };

} /* namespace peripheral */
} /* namespace addon */
} /* namespace kodi */
} /* extern "C" */
#endif

#endif // __PERIPHERAL_TYPES_H__
