/*
 * This file is part of OpenStopMotion by Patrick Fedick
 *
 * $Author$
 * $Revision$
 * $Date$
 * $Id$
 *
 *
 * Copyright (c) 2013 Patrick Fedick <patrick@pfp.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CONFIG_H_
#define CONFIG_H_


class Config
{
	public:
		Config();
		~Config();
		void load();
		void save();

		QByteArray ScreenGeometry;
		ppl7::String	CaptureDir;
		ppl7::String	ReleaseDir;
		ppl7::String	ReleaseCommand;
		ppl7::String	Scene;
		ppl7::String	DeviceName;
		ppl7::String	ImageFormat;
		ppl7::String	ImageSize;

		ppl7::String	LedControlFile;
		ppl7::String	ArduinoDevice;
		int				ArduinoBaudRate;
		ppl7::String	MotionControlBaseUri;
		int				MotionControlMoveSteps;
		int				MotionControlTurnSteps;
		ppl7::String	MotionControlFile;

		int mergeFrames;
		int skipFrames;
		int	onionValue;
		int scalingMode;
		int jpegQuality;
		int pictureFormat;
		bool saveCamShot;
		bool saveComposited;
		bool darkLayout;

		int jpegQualityComp;
		int pictureFormatComp;

		int frameRate;
		int overblendFactor;

		bool interpolate;

		// Chroma-Keying
		bool 			chromaKeyEnabled;
		ppl7::String	chromaBGImage;
		int				chromaToleranceFar;
		int				chromaToleranceNear;
		int				chromaSpillRemove;
		ppl7::grafix::Color	chromaKey;
		ppl7::grafix::Color	replaceColor;
		int				chromaReplaceMode;

		// Foreground
		bool 			foregroundEnabled;
		ppl7::String	chromaFGImage;
		int				chromaToleranceFarFG;
		int				chromaToleranceNearFG;
		int				chromaSpillRemoveFG;
		ppl7::grafix::Color	chromaKeyFG;

};

#endif /* CONFIG_H_ */
