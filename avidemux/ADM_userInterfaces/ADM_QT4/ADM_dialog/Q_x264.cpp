/***************************************************************************
                         Q_x264.cpp  -  description
                         --------------------------

                          GUI for configuring x264

    begin                : Tue May 18 2008
    copyright            : (C) 2008 by gruntster
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
#include "ui_x264.h"
#include "Q_x264_matrix.h"
#include "Q_x264_zones.h"

#include "ADM_default.h"
#include "ADM_encoder/ADM_vidEncode.hxx"
#include "ADM_codecs/ADM_x264param.h"
#include "DIA_coreToolkit.h"
#include "avidemutils.h"
#include "avi_vars.h"
#include "DIA_fileSel.h"

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

	void loadSettings(COMPRES_PARAMS *genericParam);
	int getValueIndexInArray(uint8_t value, const uint8_t valueArray[], int elementCount);

public:
	x264ConfigDialog(COMPRES_PARAMS *config);
	void saveSettings(COMPRES_PARAMS *genericParam);

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

x264ConfigDialog::x264ConfigDialog(COMPRES_PARAMS *config)
{
	static const int _predefinedARs[aspectRatioCount][2] = {{16, 15}, {64, 45}, {8, 9}, {32, 27}};

	// Mappings for x264 array index -> UI combobox index
	static const uint8_t _idcLevel[idcLevelCount] = {1, 11, 12, 13, 2, 21, 22, 3, 31, 32, 4, 41, 42, 5, 51};
	static const uint8_t _videoFormat[videoFormatCount] = {5, 0, 1, 2, 3, 4};
	static const uint8_t _colourPrimaries[colourPrimariesCount] = {2, 1, 4, 5, 6, 7, 8};
	static const uint8_t _transferCharacteristics[transferCharacteristicsCount] = {2, 1, 4, 5, 8, 9, 10, 6, 7};
	static const uint8_t _colourMatrix[colourMatrixCount] = {2, 1, 4, 5, 6, 7, 0, 8};

	memcpy(predefinedARs, _predefinedARs, sizeof(predefinedARs));
	memcpy(idcLevel, _idcLevel, sizeof(idcLevel));
	memcpy(videoFormat, _videoFormat, sizeof(videoFormat));
	memcpy(colourPrimaries, _colourPrimaries, sizeof(colourPrimaries));
	memcpy(transferCharacteristics, _transferCharacteristics, sizeof(transferCharacteristics));	
	memcpy(colourMatrix, _colourMatrix, sizeof(colourMatrix));	

	ui.setupUi(this);

	// General tab
	lastBitrate = 1500;
	lastVideoSize = 700;

	connect(ui.encodingModeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(encodingModeComboBox_currentIndexChanged(int)));
	connect(ui.quantiserSlider, SIGNAL(valueChanged(int)), this, SLOT(quantiserSlider_valueChanged(int)));
	connect(ui.quantiserSpinBox, SIGNAL(valueChanged(int)), this, SLOT(quantiserSpinBox_valueChanged(int)));
	connect(ui.targetRateControlSpinBox, SIGNAL(valueChanged(int)), this, SLOT(targetRateControlSpinBox_valueChanged(int)));

	uint32_t war = video_body->getPARWidth();
	uint32_t har = video_body->getPARHeight();
	const char *s;

	getAspectRatioFromAR(war, har, &s);
	ui.sarAsInputLabel->setText(QString("%1 (%2:%3)").arg(s).arg(war).arg(har));

	// Motion Estimation tab
	connect(ui.meSlider, SIGNAL(valueChanged(int)), this, SLOT(meSlider_valueChanged(int)));
	connect(ui.meSpinBox, SIGNAL(valueChanged(int)), this, SLOT(meSpinBox_valueChanged(int)));
	connect(ui.dct8x8CheckBox, SIGNAL(toggled(bool)), this, SLOT(dct8x8CheckBox_toggled(bool)));
	connect(ui.p8x8CheckBox, SIGNAL(toggled(bool)), this, SLOT(p8x8CheckBox_toggled(bool)));

	// Frame tab
	connect(ui.loopFilterCheckBox, SIGNAL(toggled(bool)), this, SLOT(loopFilterCheckBox_toggled(bool)));
	connect(ui.cabacCheckBox, SIGNAL(toggled(bool)), this, SLOT(cabacCheckBox_toggled(bool)));

	// Analysis tab
	connect(ui.trellisCheckBox, SIGNAL(toggled(bool)), this, SLOT(trellisCheckBox_toggled(bool)));
	connect(ui.matrixCustomEditButton, SIGNAL(pressed()), this, SLOT(matrixCustomEditButton_pressed()));

	// Advanced Rate Control
	connect(ui.zoneAddButton, SIGNAL(pressed()), this, SLOT(zoneAddButton_pressed()));
	connect(ui.zoneEditButton, SIGNAL(pressed()), this, SLOT(zoneEditButton_pressed()));
	connect(ui.frameTypeFileButton, SIGNAL(pressed()), this, SLOT(frameTypeFileButton_pressed()));	

	loadSettings(config);
}

// General tab
void x264ConfigDialog::encodingModeComboBox_currentIndexChanged(int index)
{
	bool enable = false;

	switch (index)
	{
		case 0:
			ui.targetRateControlLabel1->setText(QT_TR_NOOP("Target Bitrate"));
			ui.targetRateControlLabel2->setText(QT_TR_NOOP("kbit/s"));
			ui.targetRateControlSpinBox->setValue(lastBitrate);
			break;
		case 1: // Constant Quality - 1 pass
			ui.quantiserLabel2->setText(QT_TR_NOOP("Quantiser"));
			enable = true;
			break;
		case 2: // Average Quantiser - 1 pass
			ui.quantiserLabel2->setText(QT_TR_NOOP("Quality"));
			enable = true;
			break;
		case 3: // Video Size - 2 pass
			ui.targetRateControlLabel1->setText(QT_TR_NOOP("Target Video Size"));
			ui.targetRateControlLabel2->setText(QT_TR_NOOP("MB"));
			ui.targetRateControlSpinBox->setValue(lastVideoSize);
			break;
		case 4: // Average Bitrate - 2 pass
			ui.targetRateControlLabel1->setText(QT_TR_NOOP("Average Bitrate"));
			ui.targetRateControlLabel2->setText(QT_TR_NOOP("kbit/s"));
			ui.targetRateControlSpinBox->setValue(lastBitrate);
			break;
	}

	ui.quantiserLabel1->setEnabled(enable);
	ui.quantiserLabel2->setEnabled(enable);
	ui.quantiserLabel3->setEnabled(enable);
	ui.quantiserSlider->setEnabled(enable);
	ui.quantiserSpinBox->setEnabled(enable);

	ui.targetRateControlLabel1->setEnabled(!enable);
	ui.targetRateControlLabel2->setEnabled(!enable);
	ui.targetRateControlSpinBox->setEnabled(!enable);
}

void x264ConfigDialog::quantiserSlider_valueChanged(int value)
{
	ui.quantiserSpinBox->setValue(value);
}

void x264ConfigDialog::quantiserSpinBox_valueChanged(int value)
{
	ui.quantiserSlider->setValue(value);
}

void x264ConfigDialog::targetRateControlSpinBox_valueChanged(int value)
{
	if (ui.encodingModeComboBox->currentIndex() == 3)	// Video Size - 2 pass
		lastVideoSize = value;
	else
		lastBitrate = value;
}

// Motion Estimation tab
void x264ConfigDialog::meSlider_valueChanged(int value)
{
	ui.meSpinBox->setValue(value);
	ui.rdoCheckBox->setEnabled(value >= 6);

	if (value < 6)
		ui.rdoCheckBox->setChecked(false);
}

void x264ConfigDialog::meSpinBox_valueChanged(int value)
{
	ui.meSlider->setValue(value);
}

void x264ConfigDialog::dct8x8CheckBox_toggled(bool checked)
{
	if (!checked)
		ui.i8x8CheckBox->setChecked(false);
}

void x264ConfigDialog::p8x8CheckBox_toggled(bool checked)
{
	if (!checked)
		ui.p4x4CheckBox->setChecked(false);
}

// Frame tab
void x264ConfigDialog::loopFilterCheckBox_toggled(bool checked)
{
	if (!checked)
	{
		ui.alphaC0SpinBox->setValue(0);
		ui.betaSpinBox->setValue(0);
	}
}

void x264ConfigDialog::cabacCheckBox_toggled(bool checked)
{
	if (!checked && ui.trellisCheckBox->isChecked())
		if (GUI_Question(QT_TR_NOOP("Trellis optimisation isn't possible without CABAC coding enabled.  Trellis optimisation will automatically be disabled.\n\n Do you wish to continue?")))
			ui.trellisCheckBox->setChecked(false);
		else
			ui.cabacCheckBox->setChecked(true);
}

// Analysis tab
void x264ConfigDialog::trellisCheckBox_toggled(bool checked)
{
	if (checked && !ui.cabacCheckBox->isChecked())
		if (GUI_Question(QT_TR_NOOP("Trellis optimisation requires CABAC coding to be enabled.  CABAC coding will automatically be enabled.\n\nDo you wish to continue?")))
			ui.cabacCheckBox->setChecked(true);
		else
			ui.trellisCheckBox->setChecked(false);
}

void x264ConfigDialog::matrixCustomEditButton_pressed()
{
	x264CustomMatrixDialog dialog(intra4x4Luma, intraChroma, inter4x4Luma, interChroma, intra8x8Luma, inter8x8Luma);

	if (dialog.exec() == QDialog::Accepted)
		dialog.getMatrix(intra4x4Luma, intraChroma, inter4x4Luma, interChroma, intra8x8Luma, inter8x8Luma);
}

void x264ConfigDialog::zoneAddButton_pressed()
{
	x264ZonesDialog dialog;

	if (dialog.exec() == QDialog::Accepted)
	{
	}
}

void x264ConfigDialog::zoneEditButton_pressed()
{
	x264ZonesDialog dialog;

	if (dialog.exec() == QDialog::Accepted)
	{
	}
}

void x264ConfigDialog::frameTypeFileButton_pressed()
{
	char frameTypeFileName[1024];

	if (FileSel_SelectRead(QT_TR_NOOP("Select Frame Type/Quantiser File"), frameTypeFileName, 1023, NULL) && ADM_fileExist(frameTypeFileName))
		ui.frameTypeFileLineEdit->setText(frameTypeFileName);
}

int x264ConfigDialog::getValueIndexInArray(uint8_t value, const uint8_t valueArray[], int elementCount)
{
	int valueIndex = -1;

	for (int index = 0; index < elementCount; index++)
	{
		if (valueArray[index] == value)
		{
			valueIndex = index;
			break;
		}
	}

	return valueIndex;
}

void x264ConfigDialog::loadSettings(COMPRES_PARAMS *genericParam)
{
	ADM_x264Param *x264Param = (ADM_x264Param*)genericParam->extraSettings;

	// General tab
	switch (genericParam->mode)
	{
		case COMPRESS_CBR:	// Constant Bitrate (Single Pass)
			ui.encodingModeComboBox->setCurrentIndex(0);
			ui.targetRateControlSpinBox->setValue(genericParam->bitrate);
			break;
		case COMPRESS_CQ:	// Constant Quality (Single Pass)
			ui.encodingModeComboBox->setCurrentIndex(1);
			ui.targetRateControlSpinBox->setValue(genericParam->qz);
			break;
		case COMPRESS_AQ:	// Average Quantizer (Single Pass)
			ui.encodingModeComboBox->setCurrentIndex(2);
			ui.targetRateControlSpinBox->setValue(genericParam->qz);
			break;
		case COMPRESS_2PASS:	// Video Size (Two Pass)
			ui.encodingModeComboBox->setCurrentIndex(3);
			ui.targetRateControlSpinBox->setValue(genericParam->finalsize);
			break;
		case COMPRESS_2PASS_BITRATE:	// Average Bitrate (Two Pass)
			ui.encodingModeComboBox->setCurrentIndex(4);
			ui.targetRateControlSpinBox->setValue(genericParam->avg_bitrate);
			break;
	}

	if (x264Param->AR_AsInput)
		ui.sarAsInputRadioButton->setChecked(true);
	else
	{
		bool predefined = false;

		for (int ratioIndex = 0; ratioIndex < aspectRatioCount; ratioIndex++)
		{
			if (x264Param->AR_Num == predefinedARs[ratioIndex][0] && x264Param->AR_Den == predefinedARs[ratioIndex][1])
			{
				ui.sarPredefinedRadioButton->setChecked(true);
				ui.sarPredefinedComboBox->setCurrentIndex(ratioIndex);
				predefined = true;
				break;
			}
		}

		if (!predefined)
		{
			ui.sarCustomSpinBox1->setValue(x264Param->AR_Num);
			ui.sarCustomSpinBox2->setValue(x264Param->AR_Den);
		}
	}

	// Motion Estimation tab
	ui.meSpinBox->setValue(x264Param->PartitionDecision);
	ui.rdoCheckBox->setChecked(x264Param->RDO);
	ui.meMethodComboBox->setCurrentIndex(x264Param->Method);
	ui.mvRangeSpinBox->setValue(x264Param->Range);

	if (x264Param->mv_range == -1)
		ui.mvLengthCheckBox->setChecked(false);
	else
	{
		ui.mvLengthCheckBox->setChecked(true);
		ui.mvLengthSpinBox->setValue(x264Param->mv_range);
	}

	if (x264Param->mv_range_thread == -1)
		ui.minThreadBufferCheckBox->setChecked(false);
	else
	{
		ui.minThreadBufferCheckBox->setChecked(true);
		ui.minThreadBufferSpinBox->setValue(x264Param->mv_range_thread);
	}

	ui.predictModeComboBox->setCurrentIndex(x264Param->DirectMode);

	if (x264Param->direct_8x8_inference == -1)
		ui.predictSizeComboBox->setCurrentIndex(0);
	else
		ui.predictSizeComboBox->setCurrentIndex(x264Param->direct_8x8_inference);

	ui.weightedPredictCheckBox->setChecked(x264Param->Weighted);
	ui.dct8x8CheckBox->setChecked(x264Param->_8x8);
	ui.p8x8CheckBox->setChecked(x264Param->_8x8P);
	ui.b8x8CheckBox->setChecked(x264Param->_8x8B);
	ui.p4x4CheckBox->setChecked(x264Param->_4x4);
	ui.i8x8CheckBox->setChecked(x264Param->_8x8I);
	ui.i4x4CheckBox->setChecked(x264Param->_4x4I);

	// Frame tab
	ui.cabacCheckBox->setChecked(x264Param->CABAC);
	ui.interlacedCheckBox->setChecked(x264Param->interlaced);
	ui.loopFilterCheckBox->setChecked(x264Param->DeblockingFilter);
	ui.alphaC0SpinBox->setValue(x264Param->Strength);
	ui.betaSpinBox->setValue(x264Param->Threshold);
	ui.maxBFramesSpinBox->setValue(x264Param->MaxBFrame);
	ui.BFrameBiasSpinBox->setValue(x264Param->Bias);
	ui.refFramesSpinBox->setValue(x264Param->MaxRefFrames);
	ui.bFrameRefCheckBox->setChecked(x264Param->BasReference);
	ui.adaptiveBFrameCheckBox->setChecked(x264Param->Adaptative);
	ui.maxGopSizeSpinBox->setValue(x264Param->MaxIdr);
	ui.minGopSizeSpinBox->setValue(x264Param->MinIdr);
	ui.IFrameThresholdSpinBox->setValue(x264Param->SceneCut);
	ui.scenecutDetectionCheckBox->setChecked(x264Param->pre_scenecut);

	// Analysis tab
	ui.mixedRefsCheckBox->setChecked(x264Param->MixedRefs);
	ui.chromaMotionEstCheckBox->setChecked(x264Param->ChromaME);
	ui.bFrameMotionEstCheckBox->setChecked(x264Param->BidirME);

	if (x264Param->Trellis)
	{
		ui.trellisCheckBox->setChecked(true);
		ui.trellisComboBox->setCurrentIndex(x264Param->Trellis - 1);
	}
	else
		ui.trellisCheckBox->setChecked(false);

	ui.fastPSkipCheckBox->setChecked(x264Param->fastPSkip);
	ui.dctDecimateCheckBox->setChecked(x264Param->DCTDecimate);
	ui.noiseReductionSpinBox->setValue(x264Param->NoiseReduction);
	ui.interLumaSpinBox->setValue(x264Param->interLumaDeadzone);
	ui.intraLumaSpinBox->setValue(x264Param->intraLumaDeadzone);

	switch (x264Param->cqmPreset)
	{
		case 0:
			ui.matrixFlatRadioButton->setChecked(true);
			break;
		case 1:
			ui.matrixJvtRadioButton->setChecked(true);
			break;
		case 2:
			ui.matrixCustomRadioButton->setChecked(true);
			break;
	}

	memcpy(intra4x4Luma, x264Param->intra4x4Luma, sizeof(intra4x4Luma));
	memcpy(intraChroma, x264Param->intraChroma, sizeof(intraChroma));
	memcpy(inter4x4Luma, x264Param->inter4x4Luma, sizeof(inter4x4Luma));
	memcpy(interChroma, x264Param->interChroma, sizeof(interChroma));
	memcpy(intra8x8Luma, x264Param->intra8x8Luma, sizeof(intra8x8Luma));
	memcpy(inter8x8Luma, x264Param->inter8x8Luma, sizeof(inter8x8Luma));

	// Quantiser tab
	ui.quantiserMinSpinBox->setValue(x264Param->MinQp);
	ui.quantiserMaxSpinBox->setValue(x264Param->MaxQp);
	ui.quantiserMaxStepSpinBox->setValue(x264Param->QpStep);
	ui.avgBitrateToleranceSpinBox->setValue(x264Param->rateTolerance);
	ui.quantiserIpRatioSpinBox->setValue(x264Param->quantiserIpRatio);
	ui.quantiserPbRatioSpinBox->setValue(x264Param->quantiserPbRatio);
	ui.chromaLumaOffsetSpinBox->setValue(x264Param->chromaQuantiserOffset);
	ui.quantiserCurveCompressSpinBox->setValue(x264Param->BitrateVariability);
	ui.quantiserBeforeCompressSpinBox->setValue(x264Param->quantiserComplexityBlur);
	ui.quantiserAfterCompressSpinBox->setValue(x264Param->quantiserBlur);
	ui.vbvMaxBitrateSpinBox->setValue(x264Param->vbv_max_bitrate);
	ui.vbvBufferSizeSpinBox->setValue(x264Param->vbv_buffer_size);
	ui.vbvBufferOccupancySpinBox->setValue(x264Param->vbv_buffer_init);

	// Output tab
	if (!x264Param->idc)
		x264Param->idc = 51;

	ui.idcLevelComboBox->setCurrentIndex(getValueIndexInArray(x264Param->idc, idcLevel, idcLevelCount));

	QString strSpsId;

	strSpsId.setNum(x264Param->spsId);
	ui.spsiComboBox->setCurrentIndex(ui.spsiComboBox->findText(strSpsId));
	ui.repeatabilityCheckBox->setChecked(x264Param->deterministic);
	ui.accessUnitCheckBox->setChecked(x264Param->accessUnitDelimiters);
	ui.psnrCheckBox->setChecked(x264Param->psnrComputation);
	ui.ssimCheckBox->setChecked(x264Param->ssimComputation);
	ui.overscanComboBox->setCurrentIndex(x264Param->overscan);
	ui.videoFormatComboBox->setCurrentIndex(getValueIndexInArray(x264Param->videoFormat, videoFormat, videoFormatCount));
	ui.colourPrimariesComboBox->setCurrentIndex(getValueIndexInArray(x264Param->colourPrimaries, colourPrimaries, colourPrimariesCount));
	ui.transferCharacteristicsComboBox->setCurrentIndex(getValueIndexInArray(x264Param->transferCharacteristics, transferCharacteristics, transferCharacteristicsCount));
	ui.colourMatrixComboBox->setCurrentIndex(getValueIndexInArray(x264Param->colourMatrix, colourMatrix, colourMatrixCount));
	ui.chromaSampleSpinBox->setValue(x264Param->chromaSampleLocation);
	ui.fullRangeSamplesCheckBox->setChecked(x264Param->fullRangeSamples);
}

void x264ConfigDialog::saveSettings(COMPRES_PARAMS *genericParam)
{
	ADM_x264Param *x264Param = (ADM_x264Param*)genericParam->extraSettings;

	// General tab
	switch (ui.encodingModeComboBox->currentIndex())
	{
		case 0:	// Constant Bitrate (Single Pass)
			genericParam->mode = COMPRESS_CBR;
			genericParam->bitrate = ui.targetRateControlSpinBox->value();
			break;
		case 1: // Constant Quality (Single Pass)
			genericParam->mode = COMPRESS_CQ;
			genericParam->qz = ui.quantiserSpinBox->value();
			break;
		case 2: // Average Quantizer (Single Pass)
			genericParam->mode = COMPRESS_AQ;
			genericParam->qz = ui.quantiserSpinBox->value();
			break;
		case 3: // Video Size (Two Pass)
			genericParam->mode = COMPRESS_2PASS;
			genericParam->finalsize = ui.targetRateControlSpinBox->value();
			break;
		case 4: // Average Bitrate (Two Pass)
			genericParam->mode = COMPRESS_2PASS_BITRATE;
			genericParam->avg_bitrate = ui.targetRateControlSpinBox->value();
			break;
	}

	x264Param->AR_AsInput = (ui.sarAsInputRadioButton->isChecked());

	if (ui.sarCustomRadioButton->isChecked())
	{
		x264Param->AR_Num = ui.sarCustomSpinBox1->value();
		x264Param->AR_Den = ui.sarCustomSpinBox2->value();
	}
	else if (ui.sarPredefinedRadioButton->isChecked())
	{
		x264Param->AR_Num = predefinedARs[ui.sarPredefinedComboBox->currentIndex()][0];
		x264Param->AR_Den = predefinedARs[ui.sarPredefinedComboBox->currentIndex()][1];
	}
	else
	{
		// clear variables
		x264Param->AR_Num = 1;
		x264Param->AR_Den = 1;
	}

	// Motion Estimation tab
	x264Param->PartitionDecision = ui.meSpinBox->value();
	x264Param->RDO = ui.rdoCheckBox->isChecked();
	x264Param->Method = ui.meMethodComboBox->currentIndex();
	x264Param->Range = ui.mvRangeSpinBox->value();
	
	if (ui.mvLengthCheckBox->isChecked())
		x264Param->mv_range = ui.mvLengthSpinBox->value();
	else
		x264Param->mv_range = -1;

	if (ui.minThreadBufferCheckBox->isChecked())
		x264Param->mv_range_thread = ui.minThreadBufferSpinBox->value();
	else
		x264Param->mv_range_thread = -1;

	x264Param->DirectMode = ui.predictModeComboBox->currentIndex();
	x264Param->direct_8x8_inference = ui.predictSizeComboBox->currentIndex();

	if (x264Param->direct_8x8_inference == 0)
		x264Param->direct_8x8_inference = -1;

	x264Param->Weighted = ui.weightedPredictCheckBox->isChecked();
	x264Param->_8x8 = ui.dct8x8CheckBox->isChecked();
	x264Param->_8x8P = ui.p8x8CheckBox->isChecked();
	x264Param->_8x8B = ui.b8x8CheckBox->isChecked();
	x264Param->_4x4 = ui.p4x4CheckBox->isChecked();
	x264Param->_8x8I = ui.i8x8CheckBox->isChecked();
	x264Param->_4x4I = ui.i4x4CheckBox->isChecked();

	// Frame tab
	x264Param->CABAC = ui.cabacCheckBox->isChecked();
	x264Param->interlaced = ui.interlacedCheckBox->isChecked();
	x264Param->DeblockingFilter = ui.loopFilterCheckBox->isChecked();
	x264Param->Strength = ui.alphaC0SpinBox->value();
	x264Param->Threshold = ui.betaSpinBox->value();
	x264Param->MaxBFrame = ui.maxBFramesSpinBox->value();
	x264Param->Bias = ui.BFrameBiasSpinBox->value();
	x264Param->MaxRefFrames = ui.refFramesSpinBox->value();
	x264Param->BasReference = ui.bFrameRefCheckBox->isChecked();
	x264Param->Adaptative = ui.adaptiveBFrameCheckBox->isChecked();
	x264Param->MaxIdr = ui.maxGopSizeSpinBox->value();
	x264Param->MinIdr = ui.minGopSizeSpinBox->value();
	x264Param->SceneCut = ui.IFrameThresholdSpinBox->value();
	x264Param->pre_scenecut = ui.scenecutDetectionCheckBox->isChecked();

	// Analysis tab
	x264Param->MixedRefs = ui.mixedRefsCheckBox->isChecked();
	x264Param->ChromaME = ui.chromaMotionEstCheckBox->isChecked();
	x264Param->BidirME = ui.bFrameMotionEstCheckBox->isChecked();

	if (ui.trellisCheckBox->isChecked())
		x264Param->Trellis = ui.trellisComboBox->currentIndex() + 1;
	else
		x264Param->Trellis = 0;

	x264Param->fastPSkip = ui.fastPSkipCheckBox->isChecked();
	x264Param->DCTDecimate = ui.dctDecimateCheckBox->isChecked();
	x264Param->NoiseReduction = ui.noiseReductionSpinBox->value();
	x264Param->interLumaDeadzone = ui.interLumaSpinBox->value();
	x264Param->intraLumaDeadzone = ui.intraLumaSpinBox->value();

	if (ui.matrixCustomRadioButton->isChecked())
	{
		x264Param->cqmPreset = 2;

		memcpy(x264Param->intra4x4Luma, intra4x4Luma, sizeof(x264Param->intra4x4Luma));
		memcpy(x264Param->intraChroma, intraChroma, sizeof(x264Param->intraChroma));
		memcpy(x264Param->inter4x4Luma, inter4x4Luma, sizeof(x264Param->inter4x4Luma));
		memcpy(x264Param->interChroma, interChroma, sizeof(x264Param->interChroma));
		memcpy(x264Param->intra8x8Luma, intra8x8Luma, sizeof(x264Param->intra8x8Luma));
		memcpy(x264Param->inter8x8Luma, inter8x8Luma, sizeof(x264Param->inter8x8Luma));
	}
	else if (ui.matrixJvtRadioButton->isChecked())
		x264Param->cqmPreset = 1;
	else
		x264Param->cqmPreset = 0;

	// Quantiser tab
	x264Param->MinQp = ui.quantiserMinSpinBox->value();
	x264Param->MaxQp = ui.quantiserMaxSpinBox->value();
	x264Param->QpStep = ui.quantiserMaxStepSpinBox->value();
	x264Param->rateTolerance = ui.avgBitrateToleranceSpinBox->value();
	x264Param->quantiserIpRatio = ui.quantiserIpRatioSpinBox->value();
	x264Param->quantiserPbRatio = ui.quantiserPbRatioSpinBox->value();
	x264Param->chromaQuantiserOffset = ui.chromaLumaOffsetSpinBox->value();
	x264Param->BitrateVariability = ui.quantiserCurveCompressSpinBox->value();
	x264Param->quantiserComplexityBlur = ui.quantiserBeforeCompressSpinBox->value();
	x264Param->quantiserBlur = ui.quantiserAfterCompressSpinBox->value();
	x264Param->vbv_max_bitrate = ui.vbvMaxBitrateSpinBox->value();
	x264Param->vbv_buffer_size = ui.vbvBufferSizeSpinBox->value();
	x264Param->vbv_buffer_init = ui.vbvBufferOccupancySpinBox->value();

	// Output tab
	x264Param->idc = idcLevel[ui.idcLevelComboBox->currentIndex()];

	bool ok;
	x264Param->spsId = ui.spsiComboBox->currentText().toInt(&ok);
	ADM_assert(ok);

	x264Param->deterministic = ui.repeatabilityCheckBox->isChecked();
	x264Param->accessUnitDelimiters = ui.accessUnitCheckBox->isChecked();
	x264Param->psnrComputation = ui.psnrCheckBox->isChecked();
	x264Param->ssimComputation = ui.ssimCheckBox->isChecked();
	x264Param->overscan = ui.overscanComboBox->currentIndex();
	x264Param->videoFormat = videoFormat[ui.videoFormatComboBox->currentIndex()];
	x264Param->colourPrimaries = colourPrimaries[ui.colourPrimariesComboBox->currentIndex()];
	x264Param->transferCharacteristics = transferCharacteristics[ui.transferCharacteristicsComboBox->currentIndex()];
	x264Param->colourMatrix = colourMatrix[ui.colourMatrixComboBox->currentIndex()];
	x264Param->chromaSampleLocation = ui.chromaSampleSpinBox->value();
	x264Param->fullRangeSamples = ui.fullRangeSamplesCheckBox->isChecked();
}

uint8_t DIA_x264(COMPRES_PARAMS *config)
{
	ADM_assert(config->extraSettingsLen == sizeof(ADM_x264Param));

	x264ConfigDialog dialog(config);

	if (dialog.exec() == QDialog::Accepted)
	{
		dialog.saveSettings(config);

		return 1;
	}

	return 0;
}