/*
 *  Copyright (C) 2022 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#include <string>

class CFileItem;

namespace VIDEO_UTILS
{
/*! \brief Check whether auto play next item is set for the media type of the given item.
  \param item [in] the item to check
  \return True if auto play next item is active, false otherwise.
  */
bool IsAutoPlayNextItem(const CFileItem& item);

/*! \brief Check whether auto play next item is set for the given content type.
  \param item [in] the content to check
  \return True if auto play next item is active, false otherwise.
  */
bool IsAutoPlayNextItem(const std::string& content);

struct ResumeInformation
{
  bool isResumable{false}; // the playback of the item can be resumed
  int64_t startOffset{0}; // a start offset
  int partNumber{0}; // a part number
};

/*!
 \brief Check whether playback of the given item can be resumed, get detailed information.
 \param item The item to retrieve information for
 \return The resume information.
 */
ResumeInformation GetItemResumeInformation(const CFileItem& item);

/*!
 \brief Get resume information for a part of a stack item.
 \param item The stack item to retrieve information for
 \param partNumber The number of the part
 \return The resume information.
 */
ResumeInformation GetStackPartResumeInformation(const CFileItem& item, unsigned int partNumber);

} // namespace VIDEO_UTILS
