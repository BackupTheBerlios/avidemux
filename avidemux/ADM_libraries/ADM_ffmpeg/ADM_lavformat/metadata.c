/*
 * copyright (c) 2009 Michael Niedermayer
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "metadata.h"

AVMetadataTag *
av_metadata_get(AVMetadata *m, const char *key, const AVMetadataTag *prev, int flags)
{
    unsigned int i, j;

    if(!m)
        return NULL;

    if(prev) i= prev - m->elems + 1;
    else     i= 0;

    for(; i<m->count; i++){
        const char *s= m->elems[i].key;
        if(flags & AV_METADATA_IGNORE_CASE) for(j=0; toupper(s[j]) == toupper(key[j]) && key[j]; j++);
        else                                for(j=0;         s[j]  ==         key[j]  && key[j]; j++);
        if(key[j])
            continue;
        if(s[j] && !(flags & AV_METADATA_IGNORE_SUFFIX))
            continue;
        return &m->elems[i];
    }
    return NULL;
}

int av_metadata_set(AVMetadata **pm, AVMetadataTag elem)
{
    AVMetadata *m= *pm;
    AVMetadataTag *tag= av_metadata_get(m, elem.key, NULL, 0);

    if(!m)
        m=*pm= av_mallocz(sizeof(*m));

    if(tag){
        av_free(tag->value);
        av_free(tag->key);
        *tag= m->elems[--m->count];
    }else{
        AVMetadataTag *tmp= av_realloc(m->elems, (m->count+1) * sizeof(*m->elems));
        if(tmp){
            m->elems= tmp;
        }else
            return AVERROR(ENOMEM);
    }
    if(elem.value){
        elem.key  = av_strdup(elem.key  );
        elem.value= av_strdup(elem.value);
        m->elems[m->count++]= elem;
    }
    if(!m->count)
        av_freep(pm);

    return 0;
}

void av_metadata_free(AVMetadata **pm)
{
    AVMetadata *m= *pm;

    if(m){
        while(m->count--){
            av_free(m->elems[m->count].key);
            av_free(m->elems[m->count].value);
        }
        av_free(m->elems);
    }
    av_freep(pm);
}
