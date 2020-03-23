/**
 * @file
 * @brief Header file for QtHtmlReader class
 * @author Jonathan Thomas <jonathan@openshot.org>
 * @author Sergei Kolesov (jediserg)
 * @author Jeff Shillitto (jeffski)
 *
 * @ref License
 */

/* LICENSE
 *
 * Copyright (c) 2008-2019 OpenShot Studios, LLC
 * <http://www.openshotstudios.com/>. This file is part of
 * OpenShot Library (libopenshot), an open-source project dedicated to
 * delivering high quality video editing and animation solutions to the
 * world. For more information visit <http://www.openshot.org/>.
 *
 * OpenShot Library (libopenshot) is free software: you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * OpenShot Library (libopenshot) is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with OpenShot Library. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENSHOT_QT_HTML_READER_H
#define OPENSHOT_QT_HTML_READER_H

#include "ReaderBase.h"

#include <cmath>
#include <ctime>
#include <iostream>
#include <omp.h>
#include <stdio.h>
#include <memory>
#include "CacheMemory.h"
#include "Enums.h"
#include "Exceptions.h"

class QImage;

namespace openshot
{

	/**
	 * @brief This class uses Qt libraries, to create frames with rendered HTML, and return
	 * openshot::Frame objects.
	 *
	 * Supports HTML/CSS subset available via Qt libraries, see: https://doc.qt.io/qt-5/richtext-html-subset.html
	 *
	 * @code
	 * // Any application using this class must instantiate either QGuiApplication or QApplication
	 * QApplication a(argc, argv);
	 *
	 * // Create a reader to generate an openshot::Frame containing text
	 * QtHtmlReader r(720, // width
	 *              480, // height
	 *              5, // x_offset
	 *              5, // y_offset
	 *              GRAVITY_CENTER, // gravity
	 *              "<b>Check out</b> this Text!", // html
	 *              "b { color: #ff0000 }", // css
	 *              "#000000" // background_color
	 *              );
	 * r.Open(); // Open the reader
	 *
	 * // Get frame number 1 from the video (in fact, any frame # you request will return the same frame)
	 * std::shared_ptr<Frame> f = r.GetFrame(1);
	 *
	 * // Now that we have an openshot::Frame object, lets have some fun!
	 * f->Display(); // Display the frame on the screen
	 *
	 * // Close the reader
	 * r.Close();
	 * @endcode
	 */
	class QtHtmlReader : public ReaderBase
	{
	private:
		int width;
		int height;
		int x_offset;
		int y_offset;
		std::string html;
		std::string css;
		std::string background_color;
		std::shared_ptr<QImage> image;
		bool is_open;
		openshot::GravityType gravity;
	public:

		/// Default constructor (blank text)
		QtHtmlReader();

		/// @brief Constructor for QtHtmlReader with all parameters.
		/// @param width The width of the requested openshot::Frame (not the size of the text)
		/// @param height The height of the requested openshot::Frame (not the size of the text)
		/// @param x_offset The number of pixels to offset the text on the X axis (horizontal)
		/// @param y_offset The number of pixels to offset the text on the Y axis (vertical)
		/// @param gravity The alignment / gravity of the text
		/// @param html The HTML you want to render / display
		/// @param css The CSS you want to apply to style the HTML
		/// @param background_color The background color of the frame image (valid values are a color string in \#RRGGBB or \#AARRGGBB notation, a CSS color name, or 'transparent')
		QtHtmlReader(int width, int height, int x_offset, int y_offset, GravityType gravity, std::string html, std::string css, std::string background_color);

		/// Close Reader
		void Close();

		/// Get the cache object used by this reader (always returns NULL for this object)
		openshot::CacheMemory* GetCache() { return NULL; };

		/// Get an openshot::Frame object for a specific frame number of this reader.  All numbers
		/// return the same Frame, since they all share the same image data.
		///
		/// @returns The requested frame (containing the image)
		/// @param requested_frame The frame number that is requested.
		std::shared_ptr<openshot::Frame> GetFrame(int64_t requested_frame);

		/// Determine if reader is open or closed
		bool IsOpen() { return is_open; };

		/// Return the type name of the class
		std::string Name() { return "QtHtmlReader"; };

		/// Get and Set JSON methods
		std::string Json() const override; ///< Generate JSON string of this object
		void SetJson(const std::string value); ///< Load JSON string into this object
		Json::Value JsonValue() const override; ///< Generate Json::Value for this object
		void SetJsonValue(const Json::Value root); ///< Load Json::Value into this object

		/// Open Reader - which is called by the constructor automatically
		void Open();
	};

}

#endif
