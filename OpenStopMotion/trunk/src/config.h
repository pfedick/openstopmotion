/*
 * config.h
 *
 *  Created on: 03.06.2012
 *      Author: patrickf
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
		ppl7::String	Scene;
		ppl7::String	DeviceName;
		ppl7::String	ImageFormat;
		ppl7::String	ImageSize;

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
