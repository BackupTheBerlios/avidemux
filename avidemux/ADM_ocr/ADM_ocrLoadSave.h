//
// C++ Interface: %{MODULE}
//
// Description: 
//
//
// Author: %{AUTHOR} <%{EMAIL}>, (C) %{YEAR}
//
// Copyright: See COPYING file that comes with this distribution
//
//
/**
 */
uint8_t saveGlyph(void)
{
  FILE *out;
  char *name=NULL;
  uint32_t slen;
    
  admGlyph *glyph=head.next;
    
  GUI_FileSelWrite(_("Select Glyphfile to save to"), &name);
  if(!name)
    return 0;
    
  out=fopen(name,"wb");
  ADM_dealloc(name);
  if(!out)
  {
    GUI_Error_HIG(_("Could not write the file"), NULL);
    return 0;
  }
#define WRITE(x) fwrite(&(x),sizeof(x),1,out);
    WRITE(nbGlyphs);
    
    while(glyph)
    {
      WRITE(glyph->width);
      WRITE(glyph->height);
      fwrite(glyph->data,glyph->width*glyph->height,1,out);
      if(glyph->code) slen=strlen(glyph->code);
      else slen=0;
      WRITE(slen);
      fwrite(glyph->code,slen,1,out);
      glyph=glyph->next;
    }
    
    fclose(out);
    return 1;

}
/**
 */
uint8_t loadGlyph(char *name)
{
  FILE *out;
  admGlyph *glyph,*nw;
  uint32_t N,w,h,slen;
    
  if(head.next)
  {
    if(!GUI_Question(_("Erase current glyphs ?")))
      return 0;
    destroyGlyphTree(&head);
    nbGlyphs=0;
  }
  glyph=&head;
  out=fopen(name,"rb");
  if(!out)
  {
    GUI_Error_HIG(_("File error"), _("Could not read \"%s\"."), name);
    return 0;
  }
#define READ(x) fread(&(x),sizeof(x),1,out);
    nbGlyphs=0;
    READ(N);
    while(N--)
    {
        
      READ(w);
      READ(h);
      nw=new admGlyph(w,h);
      fread(nw->data,w*h,1,out);
      READ(slen);
      if(slen)
      {
        nw->code=new char[slen+1];
        fread(nw->code,slen,1,out);
        nw->code[slen]=0;
      }
      glyph->next=nw;
      glyph=nw;
      nbGlyphs++;
    }
    
    fclose(out);
    return 1;

}
