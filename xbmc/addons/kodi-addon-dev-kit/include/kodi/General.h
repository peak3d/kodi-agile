#pragma once
/*
 *      Copyright (C) 2005-2016 Team Kodi
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
 *  along with Kodi; see the file COPYING.  If not, see
 *  <http://www.gnu.org/licenses/>.
 *
 */

#include "addon/AddonBase.h"

extern "C"
{

typedef struct sAddonToKodiFuncTable_kodi
{
  void (*queue_notification)(void* kodiInstance, const int type, const char* message);
} sAddonToKodiFuncTable_kodi;

} /* extern "C" */


//==============================================================================
// \ingroup
/// @brief For kodi::QueueNotification used message types
///
typedef enum QueueMsg
{
  /// Show info notification message
  QUEUE_INFO,
  /// Show warning notification message
  QUEUE_WARNING,
  /// Show error notification message
  QUEUE_ERROR
} QueueMsg;
//------------------------------------------------------------------------------

namespace kodi
{
  ///
  // \ingroup
  /// @brief Writes the C string pointed by format in the GUI. If format includes
  /// format specifiers (subsequences beginning with %), the additional arguments
  /// following format are formatted and inserted in the resulting string replacing
  /// their respective specifiers.
  ///
  /// After the format parameter, the function expects at least as many additional
  /// arguments as specified by format.
  ///
  /// @param[in] type          The message type.
  ///  |  enum code:    | Description:                      |
  ///  |---------------:|-----------------------------------|
  ///  |  QUEUE_INFO    | Show info notification message    |
  ///  |  QUEUE_WARNING | Show warning notification message |
  ///  |  QUEUE_ERROR   | Show error notification message   |
  /// @param[in] format        The format of the message to pass to display in Kodi.
  ///                      C string that contains the text to be written to the stream.
  ///                      It can optionally contain embedded format specifiers that are
  ///                      replaced by the values specified in subsequent additional
  ///                      arguments and formatted as requested.
  ///  |  specifier | Output                                             | Example
  ///  |------------|----------------------------------------------------|------------
  ///  |  d or i    | Signed decimal integer                             | 392
  ///  |  u         | Unsigned decimal integer                           | 7235
  ///  |  o         | Unsigned octal                                     | 610
  ///  |  x         | Unsigned hexadecimal integer                       | 7fa
  ///  |  X         | Unsigned hexadecimal integer (uppercase)           | 7FA
  ///  |  f         | Decimal floating point, lowercase                  | 392.65
  ///  |  F         | Decimal floating point, uppercase                  | 392.65
  ///  |  e         | Scientific notation (mantissa/exponent), lowercase | 3.9265e+2
  ///  |  E         | Scientific notation (mantissa/exponent), uppercase | 3.9265E+2
  ///  |  g         | Use the shortest representation: %e or %f          | 392.65
  ///  |  G         | Use the shortest representation: %E or %F          | 392.65
  ///  |  a         | Hexadecimal floating point, lowercase              | -0xc.90fep-2
  ///  |  A         | Hexadecimal floating point, uppercase              | -0XC.90FEP-2
  ///  |  c         | Character                                          | a
  ///  |  s         | String of characters                               | sample
  ///  |  p         | Pointer address                                    | b8000000
  ///  |  %         | A % followed by another % character will write a single % to the stream. | %
  ///
  /// The length sub-specifier modifies the length of the data type. This is a chart
  /// showing the types used to interpret the corresponding arguments with and without
  /// length specifier (if a different type is used, the proper type promotion or
  /// conversion is performed, if allowed):
  ///  | length| d i           | u o x X               | f F e E g G a A | c     | s       | p       | n               |
  ///  |-------|---------------|-----------------------|-----------------|-------|---------|---------|-----------------|
  ///  | (none)| int           | unsigned int          | double          | int   | char*   | void*   | int*            |
  ///  | hh    | signed char   | unsigned char         |                 |       |         |         | signed char*    |
  ///  | h     | short int     | unsigned short int    |                 |       |         |         | short int*      |
  ///  | l     | long int      | unsigned long int     |                 | wint_t| wchar_t*|         | long int*       |
  ///  | ll    | long long int | unsigned long long int|                 |       |         |         | long long int*  |
  ///  | j     | intmax_t      | uintmax_t             |                 |       |         |         | intmax_t*       |
  ///  | z     | size_t        | size_t                |                 |       |         |         | size_t*         |
  ///  | t     | ptrdiff_t     | ptrdiff_t             |                 |       |         |         | ptrdiff_t*      |
  ///  | L     |               |                       | long double     |       |         |         |                 |
  ///  <b>Note:</b> that the c specifier takes an int (or wint_t) as argument, but performs the proper conversion to a char value
  ///  (or a wchar_t) before formatting it for output.
  /// @param[in] ... (additional arguments) Depending on the format string, the function
  ///            may expect a sequence of additional arguments, each containing a value
  ///            to be used to replace a format specifier in the format string (or a pointer
  ///            to a storage location, for n).
  ///            There should be at least as many of these arguments as the number of values specified
  ///            in the format specifiers. Additional arguments are ignored by the function.
  ///
  ///
  /// ------------------------------------------------------------------------
  ///
  /// **Example:**
  /// ~~~~~~~~~~~~~{.cpp}
  /// #include <kodi/General.h>
  /// ...
  /// kodi::QueueFormattedNotification(QUEUE_WARNING, "I'm want to inform you, here with a test call to show '%s'", "this");
  /// ...
  /// ~~~~~~~~~~~~~
  ///
  inline void QueueFormattedNotification(const QueueMsg type, const char* format, ... )
  {
    va_list args;
    char buffer[16384];
    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);
    ::kodi::addon::CAddonBase::m_instance->toKodi.kodi->queue_notification
    (
      ::kodi::addon::CAddonBase::m_instance->toKodi.kodiInstance,
      type,
      buffer
    );
  }
  //----------------------------------------------------------------------------

}
