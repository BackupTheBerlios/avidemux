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
// FIXME : We need total frame # to do only pass2
//
#ifndef ADM_RTCL
#define ADM_RTCL
typedef enum 
{
	RF_I=1,
	RF_P=2,
	RF_B=3,
}ADM_rframe;

typedef enum
{
	RS_IDLE,
	RS_PASS1,
	RS_PASS2
}ADM_rstate;

typedef struct
{
	uint32_t quant;
	uint32_t size;
}ADM_pass_stat;
#define FORBIDDEN {ADM_assert(0);return 0;}

class ADM_ratecontrol
{
protected:
		uint32_t _nbFrames;
		uint32_t _fps1000;
		char 	*_logname;
		ADM_rstate _state;
public:
			ADM_ratecontrol(uint32_t fps1000, char *logname);
	virtual 	~ADM_ratecontrol() {};
	/** Maxbr & minbr in Bps, vbvsize in kBytes); Default is none */
	virtual 	uint8_t setVBVInfo(uint32_t maxbr,uint32_t minbr, uint32_t vbvsize) FORBIDDEN ;
	virtual		uint8_t startPass1( void )FORBIDDEN;
	virtual		uint8_t logPass1(uint32_t qz, ADM_rframe ftype,uint32_t size)FORBIDDEN;
	virtual		uint8_t startPass2( uint32_t size ,uint32_t nbFrame)FORBIDDEN;
	virtual		uint8_t getQz( uint32_t *qz, ADM_rframe *type )FORBIDDEN;
	virtual		uint8_t logPass2( uint32_t qz, ADM_rframe ftype,uint32_t size)FORBIDDEN;

};
#if 0
class ADM_oldXvidRc : public ADM_ratecontrol
{
protected:
public:
			ADM_oldXvidRc(uint32_t fps1000, char *logname);
	virtual 	~ADM_oldXvidRc() ;
	/** Maxbr & minbr in kbps, vbvsize in kBytes); Default is none */
	virtual 	uint8_t setVBVInfo(uint32_t maxbr,uint32_t minbr, uint32_t vbvsize);
	virtual		uint8_t startPass1( void );
	virtual		uint8_t logPass1(uint32_t qz, ADM_rframe ftype,uint32_t size);
	virtual		uint8_t startPass2( uint32_t size,uint32_t nbFrame );
	virtual		uint8_t getQz( uint32_t *qz, ADM_rframe *type );
	virtual		uint8_t logPass2( uint32_t qz, ADM_rframe ftype,uint32_t size);

};
#endif

class ADM_newXvidRc :public ADM_ratecontrol
{
protected:
public:
			ADM_newXvidRc(uint32_t fps1000, char *logname);
	virtual 	~ADM_newXvidRc() ;
	/** Maxbr & minbr in kbps, vbvsize in kBytes); Default is none */
	virtual 	uint8_t setVBVInfo(uint32_t maxbr,uint32_t minbr, uint32_t vbvsize);
	virtual		uint8_t startPass1( void );
	virtual		uint8_t logPass1(uint32_t qz, ADM_rframe ftype,uint32_t size);
	virtual		uint8_t startPass2( uint32_t size,uint32_t nbFrame );
	virtual		uint8_t getQz( uint32_t *qz, ADM_rframe *type );
	virtual		uint8_t logPass2( uint32_t qz, ADM_rframe ftype,uint32_t size);
			// Used for VBV
			uint8_t getInfo(uint32_t framenum, uint32_t *qz, uint32_t *size);

};
class ADM_newXvidRcVBV :public ADM_ratecontrol
{
protected:
			ADM_newXvidRc	*rc;
			uint32_t	_minbr,_maxbr,_vbvsize;
			ADM_pass_stat  *_stat;
public:
			ADM_newXvidRcVBV(uint32_t fps1000, char *logname);
	virtual 	~ADM_newXvidRcVBV() ;
	/** Maxbr & minbr in kbps, vbvsize in kBytes); Default is none */
	virtual 	uint8_t setVBVInfo(uint32_t maxbr,uint32_t minbr, uint32_t vbvsize);
	virtual		uint8_t startPass1( void );
	virtual		uint8_t logPass1(uint32_t qz, ADM_rframe ftype,uint32_t size);
	virtual		uint8_t startPass2( uint32_t size,uint32_t nbFrame );
	virtual		uint8_t getQz( uint32_t *qz, ADM_rframe *type );
	virtual		uint8_t logPass2( uint32_t qz, ADM_rframe ftype,uint32_t size);

};

#endif
//EOF
