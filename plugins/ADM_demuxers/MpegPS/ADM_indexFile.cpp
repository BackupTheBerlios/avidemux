/**
    \file ADM_indexFile
    \brief Handle index file reading
    copyright            : (C) 2009 by mean
    email                : fixounet@free.fr
        
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "ADM_default.h"
#include "ADM_indexFile.h"
#include "ctype.h"
/**

*/
dmxToken::dmxToken(const char *name,const char *value)
{
    this->name=ADM_strdup(name);
    this->value=ADM_strdup(value);
}
/**

*/
dmxToken::~dmxToken()
{
    if(name) ADM_dealloc(name);
    if(value) ADM_dealloc(value);
}

char *dmxToken::getName(void) {return name;}
char *dmxToken::getValue(void){return value;}
/**

*/
bool  dmxToken::isNumeric(void)
{
bool num=true;
int l=strlen(value);
    for(int i=0;i<l;i++)
    {
        char v=value[i];
        if(!isdigit(v)&& v!=0x0a && v!=0x0d) num=false;
    }
    return num;
}   
/**

*/
uint32_t dmxToken::getAsNumber(void)
{
uint32_t v;
    v=atoi(value);
    return v;
}

//****************************************************************************************

/**

*/

indexFile::indexFile()
{
    file=NULL;
}
/**

*/

indexFile::~indexFile()
{
   close();
}
/**

*/
void  indexFile::purgeTokens(void)
{
    while(ListOfTokens.size())
    {
        delete ListOfTokens[0];
        ListOfTokens.erase(ListOfTokens.begin());
    }
}
/**

*/
dmxToken        *indexFile::searchToken(const char *name)
{
    for(int i=0;i<ListOfTokens.size();i++)
    {
        dmxToken *tk=ListOfTokens[i];
        if(!strcasecmp(name,tk->getName())) return tk;
    }
    printf("[indexFile] Token %s not found\n",name);
    return NULL;
}
/**

*/

bool indexFile::open(const char *name)
{
    file=fopen(name,"rt");
    if(!file) return false;
    return true;
}

/**

*/

bool indexFile::close(void)
{
    if(file)
    {
        fclose(file);
        file=NULL;
    }
    purgeTokens();

}

/**

*/

bool indexFile::goToSection(const char *section)
{
char match[100];
    sprintf(match,"[%s]\n",section);
    fseek(file,0,SEEK_SET);
    while(1)
    {
        if(!fgets((char*)buffer,ADM_INDEX_BUFFER,file) )
        {
            printf("[indexFile] Cannot find section %s,%s*\n",section,match);
            return false;
        }
        if(!strcasecmp((char*)buffer,match)) return true;
    }
    return false;
}

/**
    \fn readSection
*/

bool indexFile::readSection(const char *section)
{
    if(false==goToSection(section)) return false;
    // Until we reach the next section, store all couples name/value into the
    //
    while(1)
    {
        if(!readString(ADM_INDEX_BUFFER,buffer)) break;
        if(buffer[0]=='[') break; // end of section
        // Now search the = and replace it by a zero
        char *head,*tail;
        head=(char *)buffer;
        tail=(char *)buffer;
        tail=strstr((char *)buffer,"=");
        if(!tail) 
        {
            printf("[psIndexer]Weird line :%s\n",buffer);
        }
        *tail=0;
        tail++;
        dmxToken *tk=new dmxToken(head,tail);
        ListOfTokens.push_back(tk);

    }
    return true;
}

/**

*/

uint32_t indexFile::getAsUint32(const char *name)
{
    dmxToken *token=searchToken(name);
    if(!token) return 0;
    if(token->isNumeric()) return token->getAsNumber();
    printf("[psIndex] token %s is not a digit : %s\n",name,token->getValue());
    return 0;
}

/** 
    \fn getAsHex
    \read entry as hex

*/

uint32_t indexFile::getAsHex(const char *name)
{
    uint32_t v;
    dmxToken *token=searchToken(name);
    char *s;
    if(!token) return 0;
    s=token->getValue();
    sscanf(s,"%x",&v);
    return v;
}

/**

*/

char *indexFile::getAsString(const char *name)
{
    dmxToken *token=searchToken(name);
    if(!token) return NULL;
    return token->getValue();

}

/**

*/

bool  indexFile::readString(uint32_t maxLen,uint8_t *buffer)
{
    if(!fgets((char *)buffer,maxLen,file)) return false;
    return true;
}


//EOF

