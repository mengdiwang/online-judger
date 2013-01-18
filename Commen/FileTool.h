//////////////////////////////////////////////////////////////////////////
/* 
 * author: youlanhai
 * e-mail: you_lan_hai@foxmail.com
 * blog: http://blog.csdn.net/you_lan_hai
 * data: 2012-2013
 */
//////////////////////////////////////////////////////////////////////////
#pragma once


typedef std::vector<tstring> StringPool;

bool fileExist(const tstring & name);

bool removeFile(const tstring & name);

bool makeDirDirect(const tstring & dir);

bool makeDir(const tstring & dir);

void formatSlash(tstring & name);

//格式化文件夹中的斜线。尾部追加斜线。
void formatDirName(tstring & dirName, bool appendSlash=true);

//获得路径中的文件名。即去除路径名，与getFilePath相反。
tstring getPathFile(const tstring & filename);

//获得文件名中的路径。即去除文件名，与getPathFile相反。
tstring getFilePath(const tstring & filename);

//获得文件名。即去除扩展名。
tstring getFileName(const tstring & filename);

//获得文件扩展名。不含'.'
tstring getFileExt(const tstring & filename);

//查找给定目录下的文件
bool findFiles(StringPool & vect, 
               const tstring & path, 
               const tstring & ext,
               bool unionPath=true);

//读取文件内容
bool readFileBuffer(std::vector<TCHAR> & buffer, const tstring & fname, bool isBinary);

//////////////////////////////////////////////////////////////////////////
//文件搜索
//////////////////////////////////////////////////////////////////////////
class cFindFile
{
public:
    cFindFile() : m_handle(NULL) {   }
    ~cFindFile(){ reset(); }

    //执行第一次查找
    bool findFirst(const tstring & name);

    //后续查找。必须调用一次findFirst才可用。
    bool findNext();

    bool valid(){ return NULL != m_handle; }

    tstring fileName(){ return m_data.cFileName; }

    bool isDot()
    { 
        tstring fname = fileName();
        return fname==_T(".") || fname==_T(".."); 
    }

    bool isDir()
    {
        return (m_data.dwFileAttributes & FILE_ATTRIBUTE_DEVICE) != 0;
    }

private:

    void reset();

private:
    WIN32_FIND_DATA m_data;
    HANDLE          m_handle;
};
