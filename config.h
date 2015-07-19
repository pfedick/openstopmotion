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


		ppl7::String	CaptureDir;
		ppl7::String	Scene;
		ppl7::String	DeviceName;
		ppl7::String	ImageFormat;
		ppl7::String	ImageSize;

		int mergeFrames;
		int skipFrames;
		int	onionValue;

};

#endif /* CONFIG_H_ */
