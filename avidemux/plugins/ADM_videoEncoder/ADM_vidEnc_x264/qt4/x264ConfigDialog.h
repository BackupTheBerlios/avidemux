#ifndef x264ConfigDialog_h
#define x264ConfigDialog_h

#include "ui_x264ConfigDialog.h"
#include "../options.h"

extern "C"
{
#include "ADM_vidEnc_plugin.h"
}

class x264ConfigDialog : public QDialog
{
	Q_OBJECT

private:
	Ui_x264ConfigDialog ui;

	static const int aspectRatioCount = 4;
	int predefinedARs[aspectRatioCount][2];

	static const int idcLevelCount = 15;
	uint8_t idcLevel[idcLevelCount];

	static const int videoFormatCount = 6;
	uint8_t videoFormat[videoFormatCount];

	static const int colourPrimariesCount = 7;
	uint8_t colourPrimaries[colourPrimariesCount];

	static const int transferCharacteristicsCount = 9;
	uint8_t transferCharacteristics[transferCharacteristicsCount];

	static const int colourMatrixCount = 8;
	uint8_t colourMatrix[colourMatrixCount];

	int lastBitrate, lastVideoSize;
	uint8_t intra4x4Luma[16], intraChroma[16];
	uint8_t inter4x4Luma[16], interChroma[16];
	uint8_t intra8x8Luma[64], inter8x8Luma[64];

	void loadSettings(vidEncProperties *properties, x264Options *options);
	int getValueIndexInArray(uint8_t value, const uint8_t valueArray[], int elementCount);

public:
	x264ConfigDialog(vidEncProperties *properties, x264Options *options);
	void saveSettings(vidEncProperties *properties, x264Options *options);

private slots:
	// General tab
	void encodingModeComboBox_currentIndexChanged(int index);
	void quantiserSlider_valueChanged(int value);
	void quantiserSpinBox_valueChanged(int value);
	void targetRateControlSpinBox_valueChanged(int value);

	// Motion Estimation tab
	void meSlider_valueChanged(int value);
	void meSpinBox_valueChanged(int value);
	void dct8x8CheckBox_toggled(bool checked);
	void p8x8CheckBox_toggled(bool checked);

	// Frame tab
	void loopFilterCheckBox_toggled(bool checked);
	void cabacCheckBox_toggled(bool checked);

	// Analysis tab
	void trellisCheckBox_toggled(bool checked);
	void matrixCustomEditButton_pressed();

	// Advanced Rate Control tab
	void zoneAddButton_pressed();
	void zoneEditButton_pressed();
	void frameTypeFileButton_pressed();
};
#endif	// x264ConfigDialog_h
