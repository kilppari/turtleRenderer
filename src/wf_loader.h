#ifndef WF_LOADER_H
#define WF_LOADER_H

#include "renderer.h"

class WFLoader
{
public:
    enum                FileType { OBJ_FILE = 0, MTL_FILE };
    ModelData           m_LoadedData;
    char                m_CurrentMatName[50];

    WFLoader() {}
    bool load( const char *filepath, FileType type );
    void printData();

private:
    void parseObjectFile( const char *line );
    void parseMaterialFile( const char * line );
};

#endif /* WF_LOADER_H */
