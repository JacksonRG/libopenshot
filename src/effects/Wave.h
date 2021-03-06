/**
 * @file
 * @brief Header file for Wave effect class
 * @author Jonathan Thomas <jonathan@openshot.org>
 *
 * @ref License
 */

// Copyright (c) 2008-2019 OpenShot Studios, LLC
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef OPENSHOT_WAVE_EFFECT_H
#define OPENSHOT_WAVE_EFFECT_H

#include "../EffectBase.h"

#include "../Frame.h"
#include "../Json.h"
#include "../KeyFrame.h"

#include <string>
#include <memory>


namespace openshot
{

	/**
	 * @brief This class distorts an image using a wave pattern
	 *
	 * Distoring an image with a wave can be used to simulate analog transmissions, and other effects, and each
	 * value of the wave computation can be animated with an openshot::Keyframe curves over time.
	 */
	class Wave : public EffectBase
	{
	private:
		//unsigned char *perm;

		/// Init effect settings
		void init_effect_details();

	public:
		Keyframe wavelength;	///< The length of the wave
		Keyframe amplitude;		///< The height of the wave
		Keyframe multiplier;	///< Amount to multiply the wave (make it bigger)
		Keyframe shift_x;		///< Amount to shift X-axis
		Keyframe speed_y;		///< Speed of the wave on the Y-axis

		/// Default constructor, useful when using Json to load the effect properties
		Wave();

		/// Constructor which takes 5 curves. The curves will distort the image.
		///
		/// @param wavelength The curve to adjust the wavelength (0.0 to 3.0)
		/// @param amplitude The curve to adjust the amplitude (0.0 to 5.0)
		/// @param multiplier The curve to adjust the multiplier (0.0 to 1.0)
		/// @param shift_x The curve to shift pixels along the x-axis (0 to 100)
		/// @param speed_y The curve to adjust the vertical speed (0 to 10)
		Wave(Keyframe wavelength, Keyframe amplitude, Keyframe multiplier, Keyframe shift_x, Keyframe speed_y);

		/// @brief This method is required for all derived classes of ClipBase, and returns a
		/// new openshot::Frame object. All Clip keyframes and effects are resolved into
		/// pixels.
		///
		/// @returns A new openshot::Frame object
		/// @param frame_number The frame number (starting at 1) of the clip or effect on the timeline.
		std::shared_ptr<openshot::Frame> GetFrame(int64_t frame_number) override { return GetFrame(std::make_shared<openshot::Frame>(), frame_number); }

		/// @brief This method is required for all derived classes of ClipBase, and returns a
		/// modified openshot::Frame object
		///
		/// The frame object is passed into this method and used as a starting point (pixels and audio).
		/// All Clip keyframes and effects are resolved into pixels.
		///
		/// @returns The modified openshot::Frame object
		/// @param frame The frame object that needs the clip or effect applied to it
		/// @param frame_number The frame number (starting at 1) of the clip or effect on the timeline.
		std::shared_ptr<openshot::Frame> GetFrame(std::shared_ptr<openshot::Frame> frame, int64_t frame_number) override;

		// Get and Set JSON methods
		std::string Json() const override; ///< Generate JSON string of this object
		void SetJson(const std::string value) override; ///< Load JSON string into this object
		Json::Value JsonValue() const override; ///< Generate Json::Value for this object
		void SetJsonValue(const Json::Value root) override; ///< Load Json::Value into this object

		/// Get all properties for a specific frame (perfect for a UI to display the current state
		/// of all properties at any time)
		std::string PropertiesJSON(int64_t requested_frame) const override;
	};

}

#endif
