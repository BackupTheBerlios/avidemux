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

#include "x264ConfigDialog.h"
#include "x264CustomMatrixDialog.h"
#include "x264ZonesDialog.h"

#include "ADM_default.h"
#include "DIA_coreToolkit.h"
#include "DIA_fileSel.h"

x264ConfigDialog::x264ConfigDialog(vidEncVideoProperties *properties, vidEncOptions *encodeOptions, x264Options *options)
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

	ui.sarAsInputLabel->setText(QString("%2:%3").arg(properties->parWidth).arg(properties->parHeight));

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

	loadSettings(encodeOptions, options);
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

void x264ConfigDialog::loadSettings(vidEncOptions *encodeOptions, x264Options *options)
{
	// General tab
	switch (encodeOptions->encodeMode)
	{
		case ADM_VIDENC_MODE_CBR:	// Constant Bitrate (Single Pass)
			ui.encodingModeComboBox->setCurrentIndex(0);
			ui.targetRateControlSpinBox->setValue(encodeOptions->encodeModeParameter);
			break;
		case ADM_VIDENC_MODE_CQP:	// Constant Quality (Single Pass)
			ui.encodingModeComboBox->setCurrentIndex(1);
			ui.quantiserSpinBox->setValue(encodeOptions->encodeModeParameter);
			break;
		case ADM_VIDENC_MODE_AQP:	// Average Quantizer (Single Pass)
			ui.encodingModeComboBox->setCurrentIndex(2);
			ui.quantiserSpinBox->setValue(encodeOptions->encodeModeParameter);
			break;
		case ADM_VIDENC_MODE_2PASS_SIZE:	// Video Size (Two Pass)
			ui.encodingModeComboBox->setCurrentIndex(3);
			ui.targetRateControlSpinBox->setValue(encodeOptions->encodeModeParameter);
			break;
		case ADM_VIDENC_MODE_2PASS_ABR:	// Average Bitrate (Two Pass)
			ui.encodingModeComboBox->setCurrentIndex(4);
			ui.targetRateControlSpinBox->setValue(encodeOptions->encodeModeParameter);
			break;
	}

	if (options->getSarAsInput())
		ui.sarAsInputRadioButton->setChecked(true);
	else
	{
		bool predefined = false;

		for (int ratioIndex = 0; ratioIndex < aspectRatioCount; ratioIndex++)
		{
			if (options->getSarWidth() == predefinedARs[ratioIndex][0] && options->getSarHeight() == predefinedARs[ratioIndex][1])
			{
				ui.sarPredefinedRadioButton->setChecked(true);
				ui.sarPredefinedComboBox->setCurrentIndex(ratioIndex);
				predefined = true;
				break;
			}
		}

		if (!predefined)
		{
			ui.sarCustomSpinBox1->setValue(options->getSarWidth());
			ui.sarCustomSpinBox2->setValue(options->getSarHeight());
		}
	}

	// Motion Estimation tab
	ui.meSpinBox->setValue(options->getSubpixelRefinement());
	ui.rdoCheckBox->setChecked(options->getBFrameRdo());
	ui.meMethodComboBox->setCurrentIndex(options->getMotionEstimationMethod());
	ui.mvRangeSpinBox->setValue(options->getMotionVectorSearchRange());

	if (options->getMotionVectorLength() == -1)
		ui.mvLengthCheckBox->setChecked(false);
	else
	{
		ui.mvLengthCheckBox->setChecked(true);
		ui.mvLengthSpinBox->setValue(options->getMotionVectorLength());
	}

	if (options->getMotionVectorThreadBuffer() == -1)
		ui.minThreadBufferCheckBox->setChecked(false);
	else
	{
		ui.minThreadBufferCheckBox->setChecked(true);
		ui.minThreadBufferSpinBox->setValue(options->getMotionVectorThreadBuffer());
	}

	ui.predictModeComboBox->setCurrentIndex(options->getDirectPredictionMode());

	if (options->getDirectPredictionSize() == -1)
		ui.predictSizeComboBox->setCurrentIndex(0);
	else
		ui.predictSizeComboBox->setCurrentIndex(options->getDirectPredictionSize());

	ui.weightedPredictCheckBox->setChecked(options->getWeightedPrediction());
	ui.dct8x8CheckBox->setChecked(options->getDct8x8());
	ui.p8x8CheckBox->setChecked(options->getPartitionP8x8());
	ui.b8x8CheckBox->setChecked(options->getPartitionB8x8());
	ui.p4x4CheckBox->setChecked(options->getPartitionP4x4());
	ui.i8x8CheckBox->setChecked(options->getPartitionI8x8());
	ui.i4x4CheckBox->setChecked(options->getPartitionI4x4());

	// Frame tab
	ui.cabacCheckBox->setChecked(options->getCabac());
	ui.interlacedCheckBox->setChecked(options->getInterlaced());
	ui.loopFilterCheckBox->setChecked(options->getLoopFilter());
	ui.alphaC0SpinBox->setValue(options->getLoopFilterAlphaC0());
	ui.betaSpinBox->setValue(options->getLoopFilterBeta());
	ui.maxBFramesSpinBox->setValue(options->getBFrames());
	ui.BFrameBiasSpinBox->setValue(options->getBFrameBias());
	ui.refFramesSpinBox->setValue(options->getReferenceFrames());
	ui.bFrameRefCheckBox->setChecked(options->getBFrameReferences());
	ui.adaptiveBFrameCheckBox->setChecked(options->getAdaptiveBFrameDecision());
	ui.maxGopSizeSpinBox->setValue(options->getGopMaximumSize());
	ui.minGopSizeSpinBox->setValue(options->getGopMaximumSize());
	ui.IFrameThresholdSpinBox->setValue(options->getScenecutThreshold());
	ui.scenecutDetectionCheckBox->setChecked(options->getPreScenecutDetection());

	// Analysis tab
	ui.mixedRefsCheckBox->setChecked(options->getMixedReferences());
	ui.chromaMotionEstCheckBox->setChecked(options->getChromaMotionEstimation());
	ui.bFrameMotionEstCheckBox->setChecked(options->getBidirectionalMotionEstimation());

	if (options->getTrellis())
	{
		ui.trellisCheckBox->setChecked(true);
		ui.trellisComboBox->setCurrentIndex(options->getTrellis() - 1);
	}
	else
		ui.trellisCheckBox->setChecked(false);

	ui.fastPSkipCheckBox->setChecked(options->getFastPSkip());
	ui.dctDecimateCheckBox->setChecked(options->getDctDecimate());
	ui.noiseReductionSpinBox->setValue(options->getNoiseReduction());
	ui.interLumaSpinBox->setValue(options->getInterLumaDeadzone());
	ui.intraLumaSpinBox->setValue(options->getIntraLumaDeadzone());

	switch (options->getCqmPreset())
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

	memcpy(intra4x4Luma, options->getIntra4x4Luma(), sizeof(intra4x4Luma));
	memcpy(intraChroma, options->getIntraChroma(), sizeof(intraChroma));
	memcpy(inter4x4Luma, options->getInter4x4Luma(), sizeof(inter4x4Luma));
	memcpy(interChroma, options->getInterChroma(), sizeof(interChroma));
	memcpy(intra8x8Luma, options->getIntra8x8Luma(), sizeof(intra8x8Luma));
	memcpy(inter8x8Luma, options->getInter8x8Luma(), sizeof(inter8x8Luma));

	// Quantiser tab
	ui.quantiserMinSpinBox->setValue(options->getQuantiserMinimum());
	ui.quantiserMaxSpinBox->setValue(options->getQuantiserMaximum());
	ui.quantiserMaxStepSpinBox->setValue(options->getQuantiserStep());
	ui.avgBitrateToleranceSpinBox->setValue((int)(options->getAverageBitrateTolerance() * 100));
	ui.quantiserIpRatioSpinBox->setValue(options->getIpFrameQuantiser());
	ui.quantiserPbRatioSpinBox->setValue(options->getPbFrameQuantiser());
	ui.chromaLumaOffsetSpinBox->setValue(options->getChromaLumaQuantiserDifference());
	ui.quantiserCurveCompressSpinBox->setValue((int)(options->getQuantiserCurveCompression() * 100));
	ui.quantiserBeforeCompressSpinBox->setValue(options->getReduceFluxBeforeCurveCompression());
	ui.quantiserAfterCompressSpinBox->setValue(options->getReduceFluxAfterCurveCompression());
	ui.vbvMaxBitrateSpinBox->setValue(options->getVbvMaximumBitrate());
	ui.vbvBufferSizeSpinBox->setValue(options->getVbvBufferSize());
	ui.vbvBufferOccupancySpinBox->setValue((int)(options->getVbvInitialOccupancy() * 100));

	// Output tab
	if (!options->getIdcLevel())
		options->setIdcLevel(51);

	ui.idcLevelComboBox->setCurrentIndex(getValueIndexInArray(options->getIdcLevel(), idcLevel, idcLevelCount));

	QString strSpsId;

	strSpsId.setNum(options->getSpsIdentifier());
	ui.spsiComboBox->setCurrentIndex(ui.spsiComboBox->findText(strSpsId));
	ui.repeatabilityCheckBox->setChecked(options->getDeterministic());
	ui.accessUnitCheckBox->setChecked(options->getAccessUnitDelimiters());
	ui.psnrCheckBox->setChecked(options->getComputePsnr());
	ui.ssimCheckBox->setChecked(options->getComputeSsim());
	ui.overscanComboBox->setCurrentIndex(options->getOverscan());
	ui.videoFormatComboBox->setCurrentIndex(getValueIndexInArray(options->getVideoFormat(), videoFormat, videoFormatCount));
	ui.colourPrimariesComboBox->setCurrentIndex(getValueIndexInArray(options->getColorPrimaries(), colourPrimaries, colourPrimariesCount));
	ui.transferCharacteristicsComboBox->setCurrentIndex(getValueIndexInArray(options->getTransfer(), transferCharacteristics, transferCharacteristicsCount));
	ui.colourMatrixComboBox->setCurrentIndex(getValueIndexInArray(options->getColorMatrix(), colourMatrix, colourMatrixCount));
	ui.chromaSampleSpinBox->setValue(options->getChromaSampleLocation());
	ui.fullRangeSamplesCheckBox->setChecked(options->getFullRangeSamples());
}

void x264ConfigDialog::saveSettings(vidEncOptions *encodeOptions, x264Options *options)
{
	// General tab
	switch (ui.encodingModeComboBox->currentIndex())
	{
		case 0:	// Constant Bitrate (Single Pass)
			encodeOptions->encodeMode = ADM_VIDENC_MODE_CBR;
			encodeOptions->encodeModeParameter = ui.targetRateControlSpinBox->value();
			break;
		case 1: // Constant Quality (Single Pass)
			encodeOptions->encodeMode = ADM_VIDENC_MODE_CQP;
			encodeOptions->encodeModeParameter = ui.quantiserSpinBox->value();
			break;
		case 2: // Average Quantizer (Single Pass)
			encodeOptions->encodeMode = ADM_VIDENC_MODE_AQP;
			encodeOptions->encodeModeParameter = ui.quantiserSpinBox->value();
			break;
		case 3: // Video Size (Two Pass)
			encodeOptions->encodeMode = ADM_VIDENC_MODE_2PASS_SIZE;
			encodeOptions->encodeModeParameter = ui.targetRateControlSpinBox->value();
			break;
		case 4: // Average Bitrate (Two Pass)
			encodeOptions->encodeMode = ADM_VIDENC_MODE_2PASS_ABR;
			encodeOptions->encodeModeParameter = ui.targetRateControlSpinBox->value();
			break;
	}

	options->setSarAsInput(ui.sarAsInputRadioButton->isChecked());

	if (ui.sarCustomRadioButton->isChecked())
	{
		options->setSarWidth(ui.sarCustomSpinBox1->value());
		options->setSarHeight(ui.sarCustomSpinBox2->value());
	}
	else if (ui.sarPredefinedRadioButton->isChecked())
	{
		options->setSarWidth(predefinedARs[ui.sarPredefinedComboBox->currentIndex()][0]);
		options->setSarHeight(predefinedARs[ui.sarPredefinedComboBox->currentIndex()][1]);
	}
	else
	{
		// clear variables
		options->setSarWidth(1);
		options->setSarHeight(1);
	}

	// Motion Estimation tab
	options->setSubpixelRefinement(ui.meSpinBox->value());
	options->setBFrameRdo(ui.rdoCheckBox->isChecked());
	options->setMotionEstimationMethod(ui.meMethodComboBox->currentIndex());
	options->setMotionVectorSearchRange(ui.mvRangeSpinBox->value());
	
	if (ui.mvLengthCheckBox->isChecked())
		options->setMotionVectorLength(ui.mvLengthSpinBox->value());
	else
		options->setMotionVectorLength(-1);

	if (ui.minThreadBufferCheckBox->isChecked())
		options->setMotionVectorThreadBuffer(ui.minThreadBufferSpinBox->value());
	else
		options->setMotionVectorThreadBuffer(-1);

	options->setDirectPredictionMode(ui.predictModeComboBox->currentIndex());

	if (ui.predictSizeComboBox->currentIndex() == 0)
		options->setDirectPredictionSize(-1);
	else
		options->setDirectPredictionSize(ui.predictSizeComboBox->currentIndex());

	options->setWeightedPrediction(ui.weightedPredictCheckBox->isChecked());
	options->setDct8x8(ui.dct8x8CheckBox->isChecked());
	options->setPartitionP8x8(ui.p8x8CheckBox->isChecked());
	options->setPartitionB8x8(ui.b8x8CheckBox->isChecked());
	options->setPartitionP4x4(ui.p4x4CheckBox->isChecked());
	options->setPartitionI8x8(ui.i8x8CheckBox->isChecked());
	options->setPartitionI4x4(ui.i4x4CheckBox->isChecked());

	// Frame tab
	options->setCabac(ui.cabacCheckBox->isChecked());
	options->setInterlaced(ui.interlacedCheckBox->isChecked());
	options->setLoopFilter(ui.loopFilterCheckBox->isChecked());
	options->setLoopFilterAlphaC0(ui.alphaC0SpinBox->value());
	options->setLoopFilterBeta(ui.betaSpinBox->value());
	options->setBFrames(ui.maxBFramesSpinBox->value());
	options->setBFrameBias(ui.BFrameBiasSpinBox->value());
	options->setReferenceFrames(ui.refFramesSpinBox->value());
	options->setBFrameReferences(ui.bFrameRefCheckBox->isChecked());
	options->setAdaptiveBFrameDecision(ui.adaptiveBFrameCheckBox->isChecked());
	options->setGopMaximumSize(ui.maxGopSizeSpinBox->value());
	options->setGopMaximumSize(ui.minGopSizeSpinBox->value());
	options->setScenecutThreshold(ui.IFrameThresholdSpinBox->value());
	options->setPreScenecutDetection(ui.scenecutDetectionCheckBox->isChecked());

	// Analysis tab
	options->setMixedReferences(ui.mixedRefsCheckBox->isChecked());
	options->setChromaMotionEstimation(ui.chromaMotionEstCheckBox->isChecked());
	options->setBidirectionalMotionEstimation(ui.bFrameMotionEstCheckBox->isChecked());

	if (ui.trellisCheckBox->isChecked())
		options->setTrellis(ui.trellisComboBox->currentIndex() + 1);
	else
		options->setTrellis(0);

	options->setFastPSkip(ui.fastPSkipCheckBox->isChecked());
	options->setDctDecimate(ui.dctDecimateCheckBox->isChecked());
	options->setNoiseReduction(ui.noiseReductionSpinBox->value());
	options->setInterLumaDeadzone(ui.interLumaSpinBox->value());
	options->setIntraLumaDeadzone(ui.intraLumaSpinBox->value());

	if (ui.matrixCustomRadioButton->isChecked())
	{
		options->setCqmPreset(2);

		options->setIntra4x4Luma(intra4x4Luma);
		options->setIntraChroma(intraChroma);
		options->setInter4x4Luma(inter4x4Luma);
		options->setInterChroma(interChroma);
		options->setIntra8x8Luma(intra8x8Luma);
		options->setInter8x8Luma(inter8x8Luma);
	}
	else if (ui.matrixJvtRadioButton->isChecked())
		options->setCqmPreset(1);
	else
		options->setCqmPreset(0);

	// Quantiser tab
	options->setQuantiserMinimum(ui.quantiserMinSpinBox->value());
	options->setQuantiserMaximum(ui.quantiserMaxSpinBox->value());
	options->setQuantiserStep(ui.quantiserMaxStepSpinBox->value());
	options->setAverageBitrateTolerance(ui.avgBitrateToleranceSpinBox->value());
	options->setIpFrameQuantiser(ui.quantiserIpRatioSpinBox->value());
	options->setPbFrameQuantiser(ui.quantiserPbRatioSpinBox->value());
	options->setChromaLumaQuantiserDifference(ui.chromaLumaOffsetSpinBox->value());
	options->setQuantiserCurveCompression(ui.quantiserCurveCompressSpinBox->value());
	options->setReduceFluxBeforeCurveCompression(ui.quantiserBeforeCompressSpinBox->value());
	options->setReduceFluxAfterCurveCompression(ui.quantiserAfterCompressSpinBox->value());
	options->setVbvMaximumBitrate(ui.vbvMaxBitrateSpinBox->value());
	options->setVbvBufferSize(ui.vbvBufferSizeSpinBox->value());
	options->setVbvInitialOccupancy(ui.vbvBufferOccupancySpinBox->value());

	// Output tab
	options->setIdcLevel(idcLevel[ui.idcLevelComboBox->currentIndex()]);

	bool ok;
	options->setSpsIdentifier(ui.spsiComboBox->currentText().toInt(&ok));
	assert(ok);

	options->setDeterministic(ui.repeatabilityCheckBox->isChecked());
	options->setAccessUnitDelimiters(ui.accessUnitCheckBox->isChecked());
	options->setComputePsnr(ui.psnrCheckBox->isChecked());
	options->setComputeSsim(ui.ssimCheckBox->isChecked());
	options->setOverscan(ui.overscanComboBox->currentIndex());
	options->setVideoFormat(videoFormat[ui.videoFormatComboBox->currentIndex()]);
	options->setColorPrimaries(colourPrimaries[ui.colourPrimariesComboBox->currentIndex()]);
	options->setTransfer(transferCharacteristics[ui.transferCharacteristicsComboBox->currentIndex()]);
	options->setColorMatrix(colourMatrix[ui.colourMatrixComboBox->currentIndex()]);
	options->setChromaSampleLocation(ui.chromaSampleSpinBox->value());
	options->setFullRangeSamples(ui.fullRangeSamplesCheckBox->isChecked());
}

extern "C"
{
	int showX264ConfigDialog(vidEncVideoProperties *properties, vidEncOptions *encodeOptions, x264Options *options)
	{
		x264ConfigDialog dialog(properties, encodeOptions, options);

		if (dialog.exec() == QDialog::Accepted)
		{
			dialog.saveSettings(encodeOptions, options);

			return 1;
		}

		return 0;
	}
}
