#ifndef Q_x264_matrix_h
#define Q_x264_matrix_h

#include "ui_x264_matrix.h"

class x264CustomMatrixDialog : public QDialog
{
	Q_OBJECT

private:
	Ui_x264CustomMatrixDialog ui;

	void setIntra4x4Luma(const uint8_t intra4x4Luma[]);
	void setIntraChroma(const uint8_t intraChroma[]);
	void setInter4x4Luma(const uint8_t inter4x4Luma[]);
	void setInterChroma(const uint8_t interChroma[]);
	void setIntra8x8Luma(const uint8_t intra8x8Luma[]);
	void setInter8x8Luma(const uint8_t inter8x8Luma[]);

public:
	x264CustomMatrixDialog(const uint8_t* intra4x4Luma, const uint8_t* intraChroma, const uint8_t* inter4x4Luma, 
		const uint8_t* interChroma, const uint8_t* intra8x8Luma, const uint8_t* inter8x8Luma);
	void getMatrix(uint8_t* intra4x4Luma, uint8_t* intra4x4Chroma, uint8_t* inter4x4Luma, 
		uint8_t* inter4x4Chroma, uint8_t* intra8x8Luma, uint8_t* inter8x8Luma);

private slots:
	void loadFileButton_pressed();
};
#endif	// Q_x264_matrix_h
