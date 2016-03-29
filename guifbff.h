/* $Id: */
/*
 * Copyright 1996 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.    John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose. It is provided "as is" without express or implied warranty.
 *
 * John L. Cwikla and Wolfram Research, Inc disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness, in no event shall John L. Cwikla or
 * Wolfram Research, Inc be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data or profits, whether in an action of contract, negligence or
 * other tortious action, arising out of or in connection with the use or
 * performance of this software.
 *
 * Author:
 *  John L. Cwikla
 *  X Programmer
 *  Wolfram Research Inc.
 *
 *  cwikla@wri.com
*/


#ifndef _guifbff_h
#define _guifbff_h

extern int GUIFBMakeDirHier(const char *_path);

extern int GUIFBFindMatchedFiles(const char *_currentPath, const char *_completion, const char *_path, int _includeFiles,
                        int _includeDirs, int _fullName, char **_bestMatch, const char ***_dirList,
                        const char ***_fileList, int *_dirListSize, int *_fileListSize);

extern int GUIFBFindFiles(const char *_path, const char *_completion, const char *_pattern, int _includeFiles,
                    int _includeDirs, int _fullName, const char ***_dirList, const char ***_fileList,
                    int *_dirListSize, int *_fileListSize);

extern void GUIFBFreeFileLists(const char **_dirList, const char **_fileList, int _dirListCount, int _fileListCount);

extern const char *GUIFBCompleteSelection(const char *_path, const char *_pattern, int *_count);

#define GUIFB_ERROR 0
#define GUIFB_SUCCESS 1
#define GUIFB_PARTIAL_MATCH -1

#define MAX_PATH_NAME_LENGTH 1024
#define MAX_FILE_NAME_LENGTH 256
#define PATH_NAME_SEPARATOR '/'

#endif /* _guifbff_h */
