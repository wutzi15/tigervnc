/* Copyright (C) 2010 TightVNC Team.  All Rights Reserved.
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <os/os.h>

#include <cassert>

#ifndef WIN32
#include <pwd.h>
#include <cstdlib>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>
#else
#include <windows.h>
#include <wininet.h> /* MinGW needs it */
#include <shlobj.h>
#endif

int getvnchomedir(char **dirp)
{
#ifndef WIN32
	char *homedir, *dir;
	size_t len;
	uid_t uid;
	struct passwd *passwd;
#else
	TCHAR *dir;
	BOOL ret;
#endif

	assert(dirp != NULL && *dirp == NULL);

#ifndef WIN32
	homedir = getenv("HOME");
	if (homedir == nullptr) {
		uid = getuid();
		passwd = getpwuid(uid);
		if (passwd == nullptr) {
			/* Do we want emit error msg here? */
			return -1;
		}
		homedir = passwd->pw_dir;
	}

	len = strlen(homedir);
	dir = new char[len+7];
	if (dir == nullptr)
		return -1;

	memcpy(dir, homedir, len);
	memcpy(dir + len, "/.vnc/\0", 7);
#else
	dir = new TCHAR[MAX_PATH];
	if (dir == NULL)
		return -1;

	ret = SHGetSpecialFolderPath(NULL, dir, CSIDL_APPDATA, FALSE);
	if (ret == FALSE) {
		delete [] dir;
		return -1;
	}
	memcpy(dir+strlen(dir), (TCHAR *)"\\vnc\\\0", 6);
#endif
	*dirp = dir;
	return 0;
}

int fileexists(char *file)
{
#ifdef WIN32
  return (GetFileAttributes(file) == INVALID_FILE_ATTRIBUTES) ? -1 : 0;
#else
  return access(file, R_OK);
#endif
}


