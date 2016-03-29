/* $Id: guifbff.c,v 1.8 1996/02/29 23:12:42 cwikla Exp $ */
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
 * prior permission.	John L. Cwikla and Wolfram Research, Inc make no
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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include <string.h>
#include <memory.h>

#include "guifbff.h"
#include "xreg.h"

#ifndef NewString
#define NewString(str) \
	((str) != NULL ? ((char *)strcpy((char *)malloc((unsigned)strlen(str) + 1), str)) : (char *)NULL)
#endif /* NewString */

#define NEW_MEMU(a,b,c) (a *)malloc(sizeof(a)*(b)+c)
#define REALLOCU(a,b,c,d) (b *)realloc((a), sizeof(b)*(c)+d)
#define FREE_MEMU(a) free((void *)(a))
#define CHUNK 25

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#define ADD_TO_LIST(l, a)		   \
		if (l.curSize == l.realSize)	\
		{						   \
			l.realSize += CHUNK;	  \
			(l.list) = REALLOCU((l.list), const char *, l.realSize, 0); \
		}						   \
		(l.list)[l.curSize++] = (a)

static int mystrcmp(const void *arg1, const void *arg2)
{
	return strcmp(*(char **)arg1, *(char **)arg2);
}

static int _patternToPatternList(const char *_pattern, char *_buffer, int *_offsets, int _maxCount)
{
	int next, done;
	char *ptr, *last;

	while(*_pattern == ' ')
		_pattern++;

	strcpy(_buffer, _pattern);

	ptr = _buffer;
	last = _buffer;
	next = 0;
	done = 0;
	while(!done)
	{
		ptr = strchr(ptr, ' ');
		if (ptr == NULL) 
		{
			if ((*last) != 0)
			{
				_offsets[next++] = last-_buffer;
			}
			done = 1;
			continue;
		}

		if (*(ptr-1) == '\\')
		{
			ptr++;
			continue;
		}

		*ptr = 0;
		if (ptr-last)
		{
			_offsets[next++] = last-_buffer;
		}

		ptr++;
		last = ptr;

		done = (next == _maxCount);
	}

	return next;
}


typedef struct _ListChunk
{
	int curSize;
	int realSize;
	const char **list;
} ListChunk;

#define MAX_LIST 25

static void shrinkMatch(char *_buffer, const char *_match)
{
	int i;
	i = 0;
	while(_buffer[i] == _match[i])
		i++;
	_buffer[i] = 0;
}

const char *GUIFBCompleteSelection(const char *_path, const char *_pattern, int *_count)
{
	char holder[MAX_PATH_NAME_LENGTH];
	int matchCount;
	DIR *dir;
	struct dirent *de;
	char fileName[MAX_PATH_NAME_LENGTH];
	char fpName [MAX_PATH_NAME_LENGTH];
	char *ptr, *filePtr, *patPtr;
	int fileLen, i, match, done;
	struct stat statBuf;
	char patList[MAX_PATH_NAME_LENGTH];
	int patOffsets[MAX_LIST], regCount;
	void *regContext[MAX_LIST];
	int regCompileEachTime;


	strcpy(fileName, _path);

	filePtr = strrchr(fileName, '/');
	ptr = strrchr(_path, '/');

	fileLen = 0;
	if (filePtr)
	{
		ptr++;
		filePtr++;
		fileLen = strlen(filePtr);
		*filePtr = 0;
	}

	if (fileLen == 0)
		return (const char *)NewString(_path);

	strcpy(holder, ptr);

	if ((dir = opendir(fileName)) == (DIR *)NULL)
		return (const char *)NULL;

	regCount = 0;
	if (_pattern)
	{
		regCompileEachTime = REGneedsRecompile();
		regCount = _patternToPatternList(_pattern, patList, patOffsets, MAX_LIST);
		if (regCount && !regCompileEachTime)
		{
			patPtr = patList;
			for(i=0;i<regCount;i++)
			{
				patPtr = patList + patOffsets[i];
				regContext[i] = REGcompile(patPtr);
			}
		}
	}
	else
		regCount = 0;

	rewinddir(dir);

	matchCount = 0;
	done = FALSE;
	while (!done && ((de = readdir(dir)) != NULL))
	{
		if (strncmp(ptr, de->d_name, fileLen) == 0)
		{
			strcpy(fpName, fileName);
			if (fpName[strlen(fpName)-1] != '/')
				strcat(fpName, "/");
			strcat(fpName, de->d_name);

			stat(fpName, &statBuf);
			if (!regCount || S_ISDIR(statBuf.st_mode))
			{
				if (matchCount == 0)
					strcpy(holder, de->d_name);
				else
					shrinkMatch(holder, de->d_name);
				matchCount++;
			}
			else
			{
				match = FALSE;
				if (regCompileEachTime)
				{
					for(i=0;!match && (i<regCount);i++)
					{
						regContext[0] = REGcompile(patList+patOffsets[i]);
						match = REGmatch(regContext[0], de->d_name);
					}
				}
				else
				{
					for(i=0;!match && (i<regCount);i++)
						match = REGmatch(regContext[i], de->d_name);
				}
				if (match)
				{
					if (matchCount == 0)
						strcpy(holder, de->d_name);
					else
						shrinkMatch(holder, de->d_name);
					matchCount++;
				}
			}
			done = (matchCount != 0) && (holder[fileLen] == 0);
		}
	}

	if (regCount)
	{
		if (regCompileEachTime)
			regCount = 1;
		for(i=0;i<regCount;i++)
			REGfree(regContext[i]);
	}

	closedir(dir);
	*_count = matchCount;
	
	if (matchCount == 0)
		return (const char *)NewString(_path);

	strcpy(filePtr, holder);

	if ((matchCount == 1) && (stat(fileName, &statBuf) != -1))
	{
		if (S_ISDIR(statBuf.st_mode))
		{
			if (fileName[strlen(fileName)-1] != '/')
				strcat(fileName, "/");
		}
	}

	return (const char *)NewString(fileName);
}

int GUIFBFindFiles(const char *_path, const char *_completion, const char *_pattern, int _includeFiles, 
					int _includeDirs, int _fullName, const char ***_dirList, const char ***_fileList,
					int *_dirListSize, int *_fileListSize)
{
	DIR *dir;
	struct dirent *de;
	char *ptr;
	char fpName[MAX_PATH_NAME_LENGTH];
	char patList[MAX_PATH_NAME_LENGTH];
	int patOffsets[MAX_LIST], regCount;
	struct stat statBuf;
	const char *returnPath;
	void *regContext[MAX_LIST];
	ListChunk dirChunk, fileChunk;
	int match, i, regCompileEachTime;
	int completionLength;

	if (!_path)
		return GUIFB_ERROR;

	returnPath = NewString(_path);

	if ((dir = opendir(returnPath)) == NULL)
		return GUIFB_ERROR;

	completionLength = _completion ? strlen(_completion) : 0;

	regCount = 0;
	if (_pattern)
	{
		regCompileEachTime = REGneedsRecompile();
		regCount = _patternToPatternList(_pattern, patList, patOffsets, MAX_LIST);
		if (regCount && !regCompileEachTime)
		{
			ptr = patList;
			for(i=0;i<regCount;i++)
			{
				ptr = patList + patOffsets[i];
				regContext[i] = REGcompile(ptr);
			}
		}
	}
	else
		regCount = 0;

	rewinddir(dir);

	if (_includeDirs)
	{
		dirChunk.realSize = CHUNK;
		dirChunk.curSize = 0;
		dirChunk.list = NEW_MEMU(const char *, CHUNK, 0);
	}

	if (_includeFiles)
	{
		fileChunk.realSize = CHUNK;
		fileChunk.curSize = 0;
		fileChunk.list = NEW_MEMU(const char *, CHUNK, 0);
	}

	while((de = readdir(dir)) != NULL)
	{
		if (strcmp(de->d_name, ".") == 0)
			continue;

		strcpy(fpName, returnPath);
		if (returnPath[strlen(returnPath)] != '/')
			strcat(fpName, "/");
		strcat(fpName, de->d_name);

		if (strcmp(de->d_name, "..") == 0)
		{
			if (_fullName)
			{
				ADD_TO_LIST(dirChunk, NewString(fpName));
			}
			else
			{
				ADD_TO_LIST(dirChunk, NewString(de->d_name));
			}
			continue;
		}


		stat(fpName, &statBuf);

		if (!_includeFiles)
		{
			if (S_ISREG(statBuf.st_mode))
				continue;
		}

		if (!_includeDirs)
		{
			if (S_ISDIR(statBuf.st_mode))
				continue;
		}

		if (_completion)
		{
			if (strncmp(de->d_name, _completion, completionLength) != 0)
				continue;
		}

		match = FALSE;

		if (S_ISDIR(statBuf.st_mode))
			match = TRUE;
		else
		if (regCount)
		{
			if (!_completion || (strncmp(de->d_name, _completion, completionLength) == 0))
			{
				if (regCompileEachTime)
				{
					for(i=0;!match && (i<regCount);i++)
					{
						regContext[0] = REGcompile(patList+patOffsets[i]);
						match = REGmatch(regContext[0], de->d_name);
					}
				}
				else
				{
					for(i=0;!match && (i<regCount);i++)
						match = REGmatch(regContext[i], de->d_name);
				}
			}
		}
		else
			match = TRUE;

		if (match)
		{
			if (_fullName)
			{
				if (S_ISDIR(statBuf.st_mode))
				{
					ADD_TO_LIST(dirChunk, NewString(fpName));
				}
				else
				{
					ADD_TO_LIST(fileChunk, NewString(fpName));
				}
			}
			else
			{
				if (S_ISDIR(statBuf.st_mode))
				{
					ADD_TO_LIST(dirChunk, NewString(de->d_name));
				}
				else
				{
					ADD_TO_LIST(fileChunk, NewString(de->d_name));
				}
			}
		}
	}

	FREE_MEMU(returnPath);

	closedir(dir);

	*_dirListSize = 0;
	*_fileListSize = 0;
	*_dirList = 0;
	*_fileList = 0;

	if (_includeDirs)
	{
		if (dirChunk.curSize)
		{
			dirChunk.list = (const char **)realloc(dirChunk.list, (sizeof(char *) * dirChunk.curSize));
			qsort(dirChunk.list, dirChunk.curSize, sizeof(char *), mystrcmp);
		}
		else
		{
			free((void *)dirChunk.list);
			dirChunk.list = (const char **)NULL;
		}
		*_dirList = dirChunk.list;
		*_dirListSize = dirChunk.curSize;
	}
	if (_includeFiles)
	{
		if (fileChunk.curSize)
		{
			fileChunk.list = (const char **)realloc(fileChunk.list, (sizeof(char *) * fileChunk.curSize));
			qsort(fileChunk.list, fileChunk.curSize, sizeof(char *), mystrcmp);
		}
		else
		{
			free((void *)fileChunk.list);
			fileChunk.list = (const char **)NULL;
		}
		*_fileList = fileChunk.list;
		*_fileListSize = fileChunk.curSize;
	}

	if (regCount)
	{
		if (regCompileEachTime)
			regCount = 1;
		for(i=0;i<regCount;i++)
			REGfree(regContext[i]);
	}

	return ((*_dirListSize) + (*_fileListSize));
}

void GUIFBFreeFileLists(const char **_dirList, const char **_fileList, int _dirListCount, int _fileListCount)
{
	int i;

	for(i=0;i<_dirListCount;i++)
		FREE_MEMU(_dirList[i]);

	for(i=0;i<_fileListCount;i++)
		FREE_MEMU(_fileList[i]);

	if (_dirListCount)
		FREE_MEMU(_dirList);
	if (_fileListCount)
		FREE_MEMU(_fileList);
}

static int _dirExists(const char *_pathName)
{
	struct stat statBuf;
	return ((stat(_pathName, &statBuf) != -1) && S_ISDIR(statBuf.st_mode));
}

static int _createDirectory(const char *_pathName)
{
	if (mkdir(_pathName, 0xFFF) == -1)
		return FALSE;
	else
		return TRUE;
}

int GUIFBMakeDirHier(const char *_path)
{
	const char *next;
	const char *copyPath;
	char buffer[MAX_PATH_NAME_LENGTH];

	if (_dirExists(_path))
		return 1;

	next = _path;
	while ((next = strchr(next, PATH_NAME_SEPARATOR)) != (char *)NULL)
	{
		if (next != copyPath)
		{
			strncpy(buffer, copyPath, next-copyPath);
			buffer[next-copyPath] = 0;

			if (!_dirExists(buffer))
				if (!_createDirectory(buffer))
					return GUIFB_ERROR;
		}
		next++;
	}

	if (!_dirExists(copyPath))
		if (!_createDirectory(copyPath))
			return GUIFB_ERROR;

	return GUIFB_SUCCESS;
}

/*
** 
** 
** 
*/

int GUIFBFindMatchedFiles(const char *_currentPath, const char *_completion, const char *_pattern, int _includeFiles, 
						int _includeDirs, int _fullName, char **_bestMatch, const char ***_dirList,
						const char ***_fileList, int *_dirListSize, int *_fileListSize)
{
	char buffer[MAX_PATH_NAME_LENGTH];
	char localPath[MAX_PATH_NAME_LENGTH];
	char *firstPos, *ptr;
	char *thisPtr;
	int curLen, done;
	const char **tempDirList, **tempFileList;
	int tempDirListSize, tempFileListSize;

	*_bestMatch = 0;
	*_dirListSize = 0;
	*_fileListSize = 0;

	if (!_pattern && !_currentPath)
		return GUIFB_ERROR;

	if (_pattern && !strchr(_pattern, PATH_NAME_SEPARATOR))
	{
		*_bestMatch = NewString(_currentPath);
		return GUIFBFindFiles(_currentPath, _completion, _pattern, TRUE, TRUE, FALSE, _dirList, _fileList, _dirListSize, _fileListSize);
	}

	if (_pattern)
		strcpy(localPath, _pattern);
	else
		strcpy(localPath, "/*");

	if (_currentPath)
	{
		strcpy(buffer, _currentPath);
		curLen = strlen(buffer);
	}
	else
		curLen = 0;

	firstPos = buffer+curLen;

	done = FALSE;
	thisPtr = localPath;
	ptr = thisPtr;
	while(!done)
	{
		thisPtr = strchr(thisPtr, PATH_NAME_SEPARATOR);
		if (thisPtr == NULL)
			break;

		ptr = strchr(thisPtr+1, PATH_NAME_SEPARATOR);
		if (ptr)
			*ptr = 0;

		strcpy(firstPos, thisPtr);
		if (!_dirExists(buffer))
		{
			if (firstPos != buffer)
				*firstPos = 0;
			if (!GUIFBFindFiles((firstPos != buffer) ? buffer : "/", _completion, thisPtr+1, TRUE, TRUE, FALSE,
					&tempDirList, &tempFileList, &tempDirListSize, &tempFileListSize))
			{
/*
** Total failure
*/

				if (firstPos == buffer)
					*_bestMatch = NewString("/");
				else
					*_bestMatch = NewString(buffer);

				(void)GUIFBFindFiles(buffer, _completion, NULL, TRUE, TRUE, FALSE,
					_dirList, _fileList, _dirListSize, _fileListSize);

				return GUIFB_PARTIAL_MATCH;
			}
			else
			if (tempFileList || (!ptr && tempDirList))
			{
/*
** found some files that match, this is as far as we can go.
*/
				*_dirList = tempDirList;
				*_dirListSize = tempDirListSize;
				*_fileList = tempFileList;
				*_fileListSize = tempFileListSize;
				if (firstPos == buffer)
					*_bestMatch = NewString("/");
				else
					*_bestMatch = NewString(buffer);

				if (ptr)
					return GUIFB_PARTIAL_MATCH;
				else
					return ((*_dirListSize) + (*_fileListSize));
			}
		}

		firstPos += strlen(thisPtr);
		if (ptr)
			*ptr = PATH_NAME_SEPARATOR;
		ptr = thisPtr;
		thisPtr++;
	}

	*_bestMatch = NewString(localPath);
	if (ptr != localPath)
		*ptr = 0;
	return GUIFBFindFiles(localPath, _completion, ptr+1, TRUE, TRUE, FALSE,
		_dirList, _fileList, _dirListSize, _fileListSize);
}
