#pragma once

class IFTPCheck
{
    public:
        virtual wxString getLatestFileName(UINT* newTime,const wxString& fileSpec) = 0;
        virtual void getFileContent(const char* content) = 0;
        virtual void parseFile(UINT newTime) = 0;
		virtual void setUpdateTime(UINT time) = 0;
    
};