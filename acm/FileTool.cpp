//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////

#include "Comm.h"

#include "FileTool.h"


//////////////////////////////////////////////////////////////////////////
//文件操作
//////////////////////////////////////////////////////////////////////////

#define SLASH_USE _T('\\')
#define SLASH_OLD _T('/')

bool fileExist(const tstring & name)
{
    return GetFileAttributes(name.c_str()) != -1;
}

bool removeFile(const tstring & name)
{
    return !!::DeleteFile(name.c_str());
}

bool makeDirDirect(const tstring & dir)
{
	return !!CreateDirectory(dir.c_str(), NULL);
}

bool makeDir(const tstring & dir)
{
    if (fileExist(dir))
    {
        return true;
    }
	tstring tempPath = dir;
	formatDirName(tempPath, false);
    tstring parentPath = getFilePath(tempPath);
    if (!parentPath.empty())
    {
        if(!makeDir(parentPath))
        {
            return false;
        }
    }
    return makeDirDirect(tempPath);
}

void formatSlash(tstring & name)
{
    std::replace(name.begin(), name.end(), SLASH_OLD, SLASH_USE);
}

void formatDirName(tstring & dirName, bool appendSlash/*=true*/)
{
    if (dirName.empty())
    {
        return;
    }
    formatSlash(dirName);

	size_t lastIndex = dirName.length()-1;

	if (appendSlash)
	{
		if (dirName[lastIndex] != SLASH_USE)
		{
			dirName += SLASH_USE;
		}
	}
	else
	{
		if (dirName[lastIndex] == SLASH_USE)
		{
			dirName.erase(lastIndex);
		}
	}
}

tstring getPathFile(const tstring & filename)
{
    tstring tempName(filename);
    formatSlash(tempName);
    size_t pos = tempName.find_last_of(SLASH_USE);
    if (pos == tempName.npos)
    {
        return tempName;
    }
    return tempName.substr(pos+1);
}

//获得文件名中的路径
tstring getFilePath(const tstring & filename)
{
    tstring tempName(filename);
    formatSlash(tempName);
    size_t pos = tempName.find_last_of(SLASH_USE);
    if (pos == tempName.npos)
    {
        return _T("");
    }
    return tempName.substr(0, pos);
}

tstring getFileName(const tstring & filename)
{
    tstring tempName(filename);
    size_t pos = tempName.find_last_of(_T('.'));
    if (pos == tempName.npos)
    {
        return tempName;
    }
    return tempName.substr(0, pos);
}

tstring getFileExt(const tstring & filename)
{
    tstring tempName(filename);
    size_t pos = tempName.find_last_of(_T('.'));
    if (pos == tempName.npos)
    {
        return _T("");
    }
    return tempName.substr(pos+1);
}

//////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////

bool cFindFile::findFirst(const tstring & name)
{
    reset();

    ZeroMemory(&m_data, sizeof(m_data));
    m_handle = ::FindFirstFile(name.c_str(), &m_data);
    if (INVALID_HANDLE_VALUE == m_handle)
    {
        m_handle = NULL;
    }
    return m_handle != NULL;
}

bool cFindFile::findNext()
{
    if (!valid())
    {
        return false;
    }
    return !!::FindNextFile(m_handle, &m_data);
}

void cFindFile::reset()
{
    if (valid())
    {
        ::FindClose(m_handle);
        m_handle = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////


bool findFiles(StringPool & vect, 
               const tstring & path, 
               const tstring & ext,
               bool unionPath)
{

    vect.clear();

    tstring formatedPath = path;
    formatDirName(formatedPath);

    tstring tempName = formatedPath + ext;

    cFindFile finder;
    if(!finder.findFirst(tempName))
    {
        return false;
    }
    do
    {
        if(finder.isDot() || finder.isDir())
        {
            continue;
        }
        if (unionPath)
        {
            vect.push_back(formatedPath + finder.fileName());
        }
        else
        {
            vect.push_back(finder.fileName());
        }
    }while(finder.findNext());

    return !vect.empty();
}

//读取文件内容
bool readFileBuffer(std::vector<TCHAR> & buffer, const tstring & fname, bool isBinary)
{
    tifstream file;
    if (isBinary)
    {
        file.open(fname.c_str(), tifstream::binary);
    }
    else
    {
        file.open(fname.c_str());
    }
    if (!file.good())
    {
        return false;
    }

    file.seekg(0, tifstream::end);
    long length = file.tellg();
    file.seekg(0, tifstream::beg);

    buffer.resize(length+1);
    file.read(&buffer[0], length);
    
    buffer[file.gcount()] = 0;
    buffer.resize(file.gcount());
    
    file.close();
    return true;
}