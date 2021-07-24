/***************************************************************************
 * Copyright 1998-2020 by authors (see AUTHORS.txt)                        *
 *                                                                         *
 *   This file is part of LuxCoreRender.                                   *
 *                                                                         *
 * Licensed under the Apache License, Version 2.0 (the "License");         *
 * you may not use this file except in compliance with the License.        *
 * You may obtain a copy of the License at                                 *
 *                                                                         *
 *     http://www.apache.org/licenses/LICENSE-2.0                          *
 *                                                                         *
 * Unless required by applicable law or agreed to in writing, software     *
 * distributed under the License is distributed on an "AS IS" BASIS,       *
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
 * See the License for the specific language governing permissions and     *
 * limitations under the License.                                          *
 ***************************************************************************/

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/filesystem.hpp>

#include "luxrays/utils/safesave.h"
#include "luxrays/utils/fileext.h"
#include "luxrays/utils/strutils.h"

using namespace std;
using namespace luxrays;

//------------------------------------------------------------------------------
// SafeSave
//------------------------------------------------------------------------------

SafeSave::SafeSave(const string &name) {
	fileName = name;
	// Keep the file extension, it can be useful for some case (like
	// OpenImageIO, in order to keep the file format)
	// Note: GetFileNamePath() already return a "/" at the end if needed
	fileNameTmp = GetFileNamePath(name) + ToString(boost::uuids::random_generator()()) + GetFileNameExt(name);
}

SafeSave::~SafeSave() {
}

void SafeSave::Process() const {
	const string fileNameCopy = fileName + ".bak";
	bool bCopyVacant = true;
	bool bFileVacant = true;
	bool bMustRemoveTmp = false;

	// Check if the file already exists
	if (boost::filesystem::exists(fileName)) {
		// Check if there is an old copy
		if (boost::filesystem::exists(fileNameCopy)) {
			// Erase the old copy
			try 
			{
				boost::filesystem::remove(fileNameCopy);
			}
			catch (boost::filesystem::filesystem_error& e)
			{
				printf("WARNING: failed remove fileNameCopy.");
				bCopyVacant = false;
			}

		}

		// Rename the new copy
		if (bCopyVacant)
		{
			// rename if oldcopy removed
			try
			{
				boost::filesystem::rename(fileName, fileNameCopy);
			}
			catch (boost::filesystem::filesystem_error& e)
			{
				printf("WARNING: failed rename fileNameCopy.");
				bFileVacant = false;
			}
		}
		else
		{
			// otherwise delete
			try
			{
				boost::filesystem::remove(fileName);
			}
			catch (boost::filesystem::filesystem_error& e)
			{
				printf("\nWARNING: failed remove %s.\n", fileName);
				bFileVacant = false;
			}
		}

	}

	// Rename the temporary file to file name
	if (bFileVacant)
	{
		// rename if fileName is vacant
		try
		{
			boost::filesystem::rename(fileNameTmp, fileName);
		}
		catch (boost::filesystem::filesystem_error& e)
		{
			printf("\nWARNING: failed rename %s.\n", fileNameTmp);
			bMustRemoveTmp = true;
		}
	}

	if (bFileVacant == false || bMustRemoveTmp == true)
	{
		// remove temp 
		try
		{
			boost::filesystem::remove(fileNameTmp);
		}
		catch (boost::filesystem::filesystem_error& e)
		{
			printf("\nERROR: failed to remove %s.\n", fileNameTmp);
			exit(-1);
		}

	}


}
