/*
 *  Copyright (C) 2021 Team CoreELEC
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include "AddonClass.h"
#include "LanguageHook.h"
#include "filesystem/File.h"
#include "utils/StringUtils.h"

namespace XBMCAddon
{
  namespace xbmcvfs
  {
    //
    /// \defgroup python_stat Stat
    /// \ingroup python_xbmcvfs
    /// @{
    /// @brief **Get file or file system status.**
    ///
    /// \python_class{ xbmcvfs.Stat(path) }
    ///
    /// These class return information about a file. Execute (search) permission
    /// is required on all of the directories in path that lead to the file.
    ///
    /// @param path                  [string] file or folder
    ///
    ///
    /// ------------------------------------------------------------------------
    /// @python_v12 New function added
    ///
    /// **Example:**
    /// ~~~~~~~~~~~~~{.py}
    /// ..
    ///   st = xbmcvfs.Stat(path)
    ///   modified = st.st_mtime()
    /// ..
    /// ~~~~~~~~~~~~~
    //
    class configini : public AddonClass
    {
      const std::string configini_path = "/flash/config.ini";
      XFILE::CFile* configini_file;
    public:
#ifndef DOXYGEN_SHOULD_USE_THIS
      inline std::string get(std::string var)
      {
        if (XFILE::CFile::Exists(configini_path.c_str(), true))
        {
          char szLine[1024];
          std::string strLine;
          std::string ret = std::string();
          std::smatch match;

          if (!configini_file.Open(configini_path.c_str()))
          {
            configini_file.Close();
            return std::string();
          }

          while (configini_file.ReadString(szLine, 1024))
          {
            strLine = szLine;
            StringUtils::RemoveCRLF(strLine);

            if (std::regex_match(strLine.c_str(), match, std::regex(StringUtils::Format("^[^#]*\b%s=([^#]*)#*", var.c_str()))))
            {
              if (match.size() == 2)
              {
                std::ssub_match value = match[1];
                ret = value.str();
                // Remove all double-quote characters
                ret.erase(remove(ret.begin(), ret.end(), '\"' ), ret.end());
                // Remove all single-quote characters
                ret.erase(remove(ret.begin(), ret.end(), '\'' ), ret.end());
                break;
              }
            }
          }

          configini_file.Close();
        }

        return ret;
      };
#endif
  }
}

