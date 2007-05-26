/***************************************************************************
                    
    copyright            : (C) 2006 by mean
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
#include "config.h" 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
    
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include <unistd.h>
#include <glib.h>    
#include "default.h"
#include "ADM_misc.h"


#include "ADM_toolkit/filesel.h"
#include "ADM_toolkit/toolkit.hxx"

#include <ADM_assert.h>
#undef fread
#undef fwrite
#undef fopen
#undef fclose

size_t ADM_fread (void *ptr, size_t size, size_t n, FILE *sstream)
{
  return fread(ptr,size,n,sstream); 
  
}

size_t ADM_fwrite (void *ptr, size_t size, size_t n, FILE *sstream)
{
  return fwrite(ptr,size,n,sstream); 
  
}
FILE  *ADM_fopen (const char *file, const char *mode)
{
  FILE *f;
#ifndef ADM_WIN32
  return fopen(file,mode); 
#else
  
  
  gchar *retval = g_locale_from_utf8 (file, -1, NULL, NULL, NULL);
  f=fopen(retval,mode);
  g_free (retval);
  return f;
  
#endif
}
int    ADM_fclose (FILE *file)
{
  return fclose(file); 
}
//*****************************
static char basedir[1024]={0};
static char jobdir[1024]={0};
static char customdir[1024]={0};
int baseDirDone=0;
int jobDirDone=0;
int customDirDone=0;
#ifdef ADM_WIN32
const char *ADM_DIR_NAME="\\avidemux";
#else
const char *ADM_DIR_NAME="/.avidemux";
#endif
/*

*/
/*
      Get the  directory where jobs are stored
******************************************************/

char *ADM_getCustomDir(void)
{
  if(customDirDone) return customdir;

  char *rootDir;
  rootDir=ADM_getBaseDir();
  strncpy(customdir,rootDir,1023);
#if defined(ADM_WIN32)
  strcat(customdir,"\\custom"); 
#else
  strcat(customdir,"/custom");
#endif
  if(!ADM_mkdir(customdir))
  {
    GUI_Error_HIG(_("Oops"),_("can't create custom directory (%s)."),customdir);
                return NULL;
  }
  customDirDone=1;
  return customdir;
}
/*
      Get the  directory where jobs are stored
******************************************************/

char *ADM_getJobDir(void)
{
  if(jobDirDone) return jobdir;

  char *rootDir;
  rootDir=ADM_getBaseDir();
  strncpy(jobdir,rootDir,1023);
#if defined(ADM_WIN32)
  strcat(jobdir,"\\jobs"); 
#else
  strcat(jobdir,"/jobs");
#endif
  if(!ADM_mkdir(jobdir))
  {
    GUI_Error_HIG(_("Oops"),_("can't create job directory (%s)."),jobdir);
                return NULL;
  }
  jobDirDone=1;
  return jobdir;
}
/*
      Get the root directory for .avidemux stuff
******************************************************/
char *ADM_getBaseDir(void)
{
char *dirname=NULL;
DIR *dir=NULL;
char *home;
//
        if(baseDirDone) return basedir;
// Get the base directory
#if defined(ADM_WIN32)
        if( ! (home=getenv("USERPROFILE")) )
        {
          GUI_Error_HIG(_("Oops"),_("can't determine $USERPROFILE."));
                    home="c:\\";
        }

#else
        if( ! (home=getenv("HOME")) )
        {
          GUI_Error_HIG(_("Oops"),_("can't determine $HOME."));
                return NULL;
        }
#endif

 // Try to open the .avidemux directory
        dirname=new char[strlen(home)+strlen(ADM_DIR_NAME)+2];
        strcpy(dirname,home);
        strcat(dirname,ADM_DIR_NAME);
        if(!ADM_mkdir(dirname))
        {
          GUI_Error_HIG(_("Oops"),_("Cannot create the .avidemux directory"), NULL);
                        delete [] dirname;
                        return NULL;
        }
        delete [] dirname;

        // Now built the filename
        strncpy(basedir,home,1023);
        strncat(basedir,ADM_DIR_NAME,1023-strlen(basedir));
        baseDirDone=1;
        printf("Using %s as base directory for prefs/jobs/...\n",basedir);
        return basedir;
}
/*----------------------------------------
      Create a directory
      If it already exists, do nothing
------------------------------------------*/
uint8_t ADM_mkdir(const char *dirname)
{
DIR *dir=NULL;
              // Check it already exists ?
              dir=opendir(dirname);
              if(dir)
              { 
                  printf("Directory %s exists.Good.\n",dirname);
                  closedir(dir);
                  return 1;
              }
#if defined(ADM_WIN32)
                if(mkdir(dirname))
                {
                    printf("Oops: mkdir failed on %s\n",dirname);   
                    return 0;
                }
                
#else    
                char *sys=new char[strlen(dirname)+strlen("mkdir ")+2];
                strcpy(sys,"mkdir ");
                strcat(sys,dirname);
                printf("Creating dir :%s\n",sys);
                system(sys);
                delete [] sys;
#endif		
              if((dir=opendir(dirname))==NULL)
                {
                        return 0;
                }
              closedir(dir); 
              return 1;
}
uint8_t buildDirectoryContent(uint32_t *outnb,const char *base, char *jobName[],int maxElems,const char *ext)
{

DIR *dir;
struct dirent *direntry;
int dirmax=0,len;
int extlen=strlen(ext);
    ADM_assert(extlen);
    
         dir=opendir(base);
        if(!dir)
        {
                return 0;
        }
        while((direntry=readdir(dir)))
        {
                len=strlen(direntry->d_name);
                if(len<(extlen+1)) continue;
                int xbase=len-extlen;
                if(memcmp(direntry->d_name+xbase,ext,extlen))
                //if(direntry->d_name[len-1]!='s' || direntry->d_name[len-2]!='j' || direntry->d_name[len-3]!='.')
                {
                        printf("ignored:%s\n",direntry->d_name);
                        continue;
                }
                jobName[dirmax]=(char *)ADM_alloc(strlen(base)+strlen(direntry->d_name)+2);
                strcpy(jobName[dirmax],base);
                strcat(jobName[dirmax],"/");
                strcat(jobName[dirmax],direntry->d_name);
                dirmax++;
                if(dirmax>=maxElems)
                {
                        printf("[jobs]: Max # of jobs exceeded\n");
                         break;
                }
        }
        closedir(dir);
        *outnb=dirmax;
        return 1;
}
//------------------------------------------------------------------

/*

** note: it modifies it's first argument
*/
void simplify_path(char **buf){
   unsigned int last1slash = 0;
   unsigned int last2slash = 0;
	while( !strncmp(*buf,"/../",4) )
		memmove(*buf,*buf+3,strlen(*buf+3)+1);
	for(unsigned int i=0;i<strlen(*buf)-2;i++)
		while( !strncmp(*buf+i,"/./",3) )
			memmove(*buf+i,*buf+i+2,strlen(*buf+i+2)+1);
	for(unsigned int i=0;i<strlen(*buf)-3;i++){
		if( *(*buf+i) == '/' ){
			last2slash = last1slash;
			last1slash = i;
		}
		if( !strncmp(*buf+i,"/../",4) ){
			memmove(*buf+last2slash,*buf+i+3,strlen(*buf+i+3)+1);
			return simplify_path(buf);
		}
	}
}

//
//	Make it absolute
//
char *PathCanonize(const char *tmpname)
{
	char path[300];
	char *out ;

	if( ! getcwd(path,300) ){
		fprintf(stderr,"\ngetcwd() failed with: %s (%u)\n",
		               strerror(errno), errno );
		path[0] = '\0';
	}
	if(!tmpname || tmpname[0]==0)
	{
		out=new char [strlen(path)+2];
		strcpy(out,path);
#ifndef ADM_WIN32		
		strcat(out,"/");
#else
		strcat(out,"\\");
#endif	
		printf("\n Canonizing null string ??? (%s)\n",out);
	}else if(tmpname[0]=='/'
#if defined(ADM_WIN32)
		|| tmpname[1]==':'
#endif	
	
	)
	{
		out=new char[strlen(tmpname)+1];
		strcpy(out,tmpname);
		return out;
	}else{
		out=new char[strlen(path)+strlen(tmpname)+6];
		strcpy(out,path);
#ifndef ADM_WIN32		
		strcat(out,"/");
#else
		strcat(out,"\\");
#endif		
		strcat(out,tmpname);
	}
	simplify_path(&out);
	return out;
}
/*
	Strip the path and only keep the name

*/
void		PathStripName(char *str)
{
		int len=strlen(str);
		if(len<=1) return;
		len--;
#ifndef ADM_WIN32		
		while( *(str+len)!='/' && len)
#else
	while( *(str+len)!='\\' && len)
#endif		
		{
			 *(str+len)=0;
			 len--;
		}
}

/*
	Get the filename without path

*/
const char *GetFileName(const char *str)
{
	char *filename;
        char *filename2;
#ifndef ADM_WIN32		
	filename = strrchr(str, '/');
        
#else
	filename = strrchr(str, '\\');
        filename2 = strrchr(str, '/');
        if(filename2 && filename)
        {
          if(filename2>filename) filename=filename2; 
        }
#endif
	if (filename)
		return filename+1;
	else
		return str;
}

/*
	Split patch into absolute path+name and extention

*/
void PathSplit(char *str, char **root, char **ext)
{
	char *full;
	uint32_t l;

		full=PathCanonize(str);
		// Search the last .
		l=strlen(full);
		l--;
		ADM_assert(l>0);
		while( *(full+l)!='.' && l) l--;
		if(!l || l==(strlen(full)-1))
		{
			if(l==(strlen(full)-1))
			{
				*(full+l)=0;  // remove trailing .
			}
			*ext=new char[2];
			*root=full;
			strcpy(*ext,"");
			return ;
		}
		// else we do get an extension
		// starting at l+1
		uint32_t suff;

		suff=strlen(full)-l-1;
		*ext=new char[suff+1];
		strcpy(*ext,full+l+1);
		*(full+l)=0;
		*root=full;
		return ;
}
