//
// C++ Implementation: %{MODULE}
//
// Description:
//
//
// Author:Mean (fixounet@free.Fr)
//
// Copyright: See COPYING file that comes with this distribution GPL
//	Use xvid cvs ratecontrol, including some VBV max bitrate constraints
//		so that it can be used for mpeg1/2 with tight constraints
//
//	Reuse some of Peter Cheat predictor model
//
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <limits.h>
#include "ADM_library/default.h"
#include "ADM_assert.h"

#include "ADM_toolkit/ADM_debugID.h"
#define MODULE_NAME  MODULE_XVID_RCTL
#include "ADM_toolkit/ADM_debug.h"

#include "ADM_xvidratectl/ADM_ratecontrol.h"

ADM_newXvidRcVBV::ADM_newXvidRcVBV(uint32_t fps1000, char *logname) : ADM_ratecontrol(fps1000,logname)
{
	rc=new ADM_newXvidRc(fps1000,logname);
	_state=RS_IDLE; 
	_minbr=0;
	_maxbr=2*9*1000*1000/8; // ~ 9MB*2
	_vbvsize=5*224*1024;	// 1MB vbv buffer size
	_stat=NULL;
	_lastSize=NULL;
	
	for(uint32_t j=0;j<AVG_LOOKUP;j++)
		_compr[j]=0;
}
ADM_newXvidRcVBV::~ADM_newXvidRcVBV()
{
	if(rc)
		delete rc;
	if(_stat)
		delete [] _stat;
	if(_lastSize)
		delete [] _lastSize;
	rc=NULL;
	_stat=NULL;
	_lastSize=NULL;
}
uint8_t ADM_newXvidRcVBV::setVBVInfo(uint32_t maxbr,uint32_t minbr, uint32_t vbvsize)
{
	_maxbr=maxbr;
	_minbr=minbr;
	_vbvsize=vbvsize*1024;
	printf("RC: Initializing vbv buffer \n");
	printf("RC: with min br= %lu kbps\n",(minbr*8)/1000);
	printf("RC:      max br= %lu kbps\n",(maxbr*8)/1000);
	printf("Rc:      VBV   = %lu kB\n",_vbvsize/1024);

	return 1;
}

uint8_t ADM_newXvidRcVBV::startPass1( void )
{
	return rc->startPass1();
}
uint8_t ADM_newXvidRcVBV::startPass2( uint32_t size,uint32_t nbFrame )
{
	printf("Starting Xvid VBV with %lu frames, target size :%lu MB\n",nbFrame,size);
	_nbFrames=nbFrame;
	if(! rc->startPass2(size,nbFrame)) return 0;
	// Built pass 1 stat file in memory
	// we will need it later to project
	//________________________________
	_stat=new ADM_pass_stat[nbFrame];
	ADM_pass_stat *cur=_stat;
	
	for(uint32_t i=0;i<nbFrame;i++)
	{
		rc->getInfo(i,&(cur->quant),&(cur->size));
		cur++;
	}
	// a roundup is close to fps
	_roundup=(uint32_t )floor((_fps1000+500)/1000);
	// Do so check
	_vbv_fullness=(_vbvsize*8)/10; // Buffer starts 80% full
	_byte_per_image=_maxbr/_roundup;
	_lastSize=new uint32_t[_roundup];
	memset(_lastSize,0,_roundup*sizeof(uint32_t));
	_frame=0;
	for(uint32_t i=0;i<AVG_LOOKUP;i++)
		_compr[i]=1.0;
	printf("Rc: Byte per image : %lu \n",_byte_per_image);
	return 1;
}
uint8_t ADM_newXvidRcVBV::logPass1(uint32_t qz, ADM_rframe ftype,uint32_t size)
{
	return rc->logPass1(qz,ftype,size);
}
uint8_t ADM_newXvidRcVBV::logPass2(uint32_t qz, ADM_rframe ftype,uint32_t size)
{
	// update stored value
	_lastSize[_frame%_roundup]=size;
	
	_vbv_fullness+=_byte_per_image;
	if(_vbv_fullness<size)
	{
		printf("VBV buffer underflow :frame %lu, underflow : %lu\n",_frame,size-_vbv_fullness);
	}
	else
	{
		_vbv_fullness-=size;
	}
	if(_vbv_fullness>_vbvsize)
	{
		// not an error printf("VBV buffer overflow :frame %lu, overflow : %lu\n",_frame,_vbv_fullness-_vbvsize);
		_vbv_fullness=_vbvsize;
	}
	// update compr
	uint32_t rank;
		rank=_frame%AVG_LOOKUP;
		_compr[rank]=getComp(_stat[_frame].size,_stat[_frame].quant,size,qz);
	//
	aprintf("Frame %08lu size %d type:%d vbv fullness %u, kbytes :%lu qz used :%d\n",_frame,size, ftype,(100*_vbv_fullness)/_vbvsize,_vbv_fullness/1024,qz);
	// compute instantaneous br
	uint32_t br=0;
	for(uint32_t i=0;i<_roundup;i++)
	{
		br+=_lastSize[i];
	}
	br*=8;
	br/=1000;
	aprintf("br : %lu\n",br);
	_frame++;
	return rc->logPass2(qz,ftype,size);
}
uint8_t ADM_newXvidRcVBV::getQz( uint32_t *qz, ADM_rframe *type )
{
	if(! rc->getQz(qz,type)) return 0;
	// Now we have the temptative quant
	// Check that both the vbv buffer & bitrate stays full enough
	if(*qz<2) *qz=2;
	while(*qz<31 && project(_frame,*qz,*type)) (*qz)++;

	
	return 1;
}
//
// Return 1 if the frame and Qz fails the sanity check
//
uint8_t ADM_newXvidRcVBV::project(uint32_t framenum, uint32_t q, ADM_rframe frame)
{
	if(!checkVBV(framenum,q,frame)) return 1;
//	if(!checkBitrate(framenum,q,frame)) return 1;
	return 0;
}
uint8_t ADM_newXvidRcVBV::checkVBV(uint32_t framenum, uint32_t q, ADM_rframe frame)
{
	float ratio;
	// Project the next frames with the same Q factor reduction as now
	// and check
	
	// A bit simplistic...
	
	if(framenum<_nbFrames-_roundup)
	{
		uint32_t projected_vbv=(_vbv_fullness*9)/10; // Only use 90% of the buffer
		uint32_t framesize;
		
		// Q increase ratio
		
		float comp=0,size,qr;
			
	
			for(uint32_t i=0;i<AVG_LOOKUP;i++)
			{
				comp+=_compr[i];
			}
			
			comp=comp/AVG_LOOKUP;	// Average compression ratio
			ratio=getRatio(q,_stat[framenum].quant,comp);
			
		
		for(uint32_t i=0;i<_roundup>>1;i++)
		{
		
			size=ratio;
			size*=_stat[framenum+i].size;
			framesize=(uint32_t)floor(size);	// predicted size
			
			if(!i && frame==RF_I) // less compression ratio ~1.5 
				framesize=(framesize*3)>>1;
			
			aprintf("\t Org: %lu projected :%d VBV:%d q:%d ratio:%f alpha:%f\n",_stat[framenum+i].size,framesize,
					projected_vbv/1024,q,ratio,comp);
			if(projected_vbv<framesize)
			{
				aprintf("potential underflow at %d + %d , q:%d\n",framenum,i,q);
				return 0;
			}
			projected_vbv-=framesize;
			projected_vbv+=_byte_per_image;
			if(projected_vbv>_vbvsize)
			{
				projected_vbv=_vbvsize;
			}
		}
	}		
	return 1;

}


/*__________________________________________________________________

	Reverse the below formula
	newbits/oldbuts=newquang^ -comp
	log(newq^ -comp)=log(newbit/oldbits)
	comp=-log(newbits/oldbits)/log(newq/oldq)
*/
float ADM_newXvidRcVBV::getComp(int oldbits, int qporg, int newbits, int qpused)
{
	float comp;
/*	
	comp=newbits;
	comp/=oldbits;
	comp=log(comp);
	comp/=log(qpused/qporg);
	printf("Old q:%d new q : %d oldBits:%d newbits:%d comp:%f\n",
			qporg,qpused,oldbits,newbits,-comp);
	comp= -comp;
	if(comp>3) comp=3;
	if(comp<0.5) comp=0.5;
	return comp;
*/
	// Linear
	// comp=(Nb*Nq)/(Ob*Oq);
	comp=newbits;
	comp*=qpused;
	comp/=qporg;
	comp/=oldbits;
	// Clamp between max alpha/min alpha
	#define MAX_ALPHA 6
	#define MIN_ALPHA (1.0/MAX_ALPHA)
	if(comp>MAX_ALPHA) comp=MAX_ALPHA;
	if(comp<MIN_ALPHA) comp=MIN_ALPHA;
	return comp;
}
/*_______________________________________________________________
	Predict the size of the image
	Using a exp(-comp) formula instead of linear formula
	
	Idea by Peter Cheat
__________________________________________________________________
*/
float ADM_newXvidRcVBV::getRatio(uint32_t newq, uint32_t oldq, float alpha)
{
 // Peter Cheat formula :Pridicted Bits Frame 10 Will Use = (Bits Used At Quantiser 1) * (New Quantiser ^ -Compressibility)
 // avg lookup compressibility
 /*			exponential
 			qr=q;
			qr=qr/_stat[framenum].quant;	// average size reduction
			qr=pow(qr,-comp);
 */
 	// Linear
	// Ob*Oq*alpha=Nb*Nq
	// Nb/Ob=Oq/Nq*alpha
	alpha=1;
	float comp;
	
	comp=oldq;	
	comp/=newq;
	comp*=alpha;
	return comp;
}

//EOF




