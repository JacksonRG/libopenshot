/**
 * @file
 * @brief Unit tests for openshot::DummyReader
 * @author Jonathan Thomas <jonathan@openshot.org>
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

#include <memory>

#include <catch2/catch.hpp>

#include "DummyReader.h"
#include "Exceptions.h"
#include "CacheMemory.h"
#include "Fraction.h"
#include "Frame.h"

TEST_CASE( "Default constructor", "[libopenshot][dummyreader]" ) {
	// Create a default fraction (should be 1/1)
	openshot::DummyReader r;
	r.Open(); // Open the reader

	// Check values
	CHECK(r.info.width == 1280);
	CHECK(r.info.height == 768);
	CHECK(r.info.fps.num == 24);
	CHECK(r.info.fps.den == 1);
	CHECK(r.info.sample_rate == 44100);
	CHECK(r.info.channels == 2);
	CHECK(r.info.duration == 30.0);

	CHECK(r.Name() == "DummyReader");

	auto cache = r.GetCache();
	CHECK(cache == nullptr);
}

TEST_CASE( "Constructor", "[libopenshot][dummyreader]" ) {
	// Create a default fraction (should be 1/1)
	openshot::DummyReader r(openshot::Fraction(30, 1), 1920, 1080, 44100, 2, 60.0);
	r.Open(); // Open the reader

	// Check values
	CHECK(r.info.width == 1920);
	CHECK(r.info.height == 1080);
	CHECK(r.info.fps.num == 30);
	CHECK(r.info.fps.den == 1);
	CHECK(r.info.sample_rate == 44100);
	CHECK(r.info.channels == 2);
	CHECK(r.info.duration == 60.0);
}

TEST_CASE( "Blank_Frame", "[libopenshot][dummyreader]" ) {
	// Create a default fraction (should be 1/1)
	openshot::DummyReader r(openshot::Fraction(30, 1), 1920, 1080, 44100, 2, 30.0);
	r.Open(); // Open the reader

	// Get a blank frame (because we have not passed a Cache object (full of Frame objects) to the constructor
	// Check values
	CHECK(r.GetFrame(1)->number == 1);
	CHECK(r.GetFrame(1)->GetPixels(700)[700] == 0); // black pixel
	CHECK(r.GetFrame(1)->GetPixels(701)[701] == 0); // black pixel
}

TEST_CASE( "Fake_Frame", "[libopenshot][dummyreader]" ) {

	// Create cache object to hold test frames
	openshot::CacheMemory cache;

	// Let's create some test frames
	for (int64_t frame_number = 1; frame_number <= 30; frame_number++) {
		// Create blank frame (with specific frame #, samples, and channels)
		// Sample count should be 44100 / 30 fps = 1470 samples per frame
		int sample_count = 1470;
		auto f = std::make_shared<openshot::Frame>(frame_number, sample_count, 2);

		// Create test samples with incrementing value
		float *audio_buffer = new float[sample_count];
		for (int64_t sample_number = 0; sample_number < sample_count; sample_number++) {
			// Generate an incrementing audio sample value (just as an example)
			audio_buffer[sample_number] = float(frame_number) + (float(sample_number) / float(sample_count));
		}

		// Add custom audio samples to Frame (bool replaceSamples, int destChannel, int destStartSample, const float* source,
		f->AddAudio(true, 0, 0, audio_buffer, sample_count, 1.0); // add channel 1
		f->AddAudio(true, 1, 0, audio_buffer, sample_count, 1.0); // add channel 2

		// Add test frame to dummy reader
		cache.Add(f);
	}

	// Create a default fraction (should be 1/1)
	openshot::DummyReader r(openshot::Fraction(30, 1), 1920, 1080, 44100, 2, 30.0, &cache);
	r.Open(); // Open the reader

	// Verify our artificial audio sample data is correct
	CHECK(r.GetFrame(1)->number == 1);
	CHECK(r.GetFrame(1)->GetAudioSamples(0)[0] == 1);
	CHECK(r.GetFrame(1)->GetAudioSamples(0)[1] == Approx(1.00068033).margin(0.00001));
	CHECK(r.GetFrame(1)->GetAudioSamples(0)[2] == Approx(1.00136054).margin(0.00001));
	CHECK(r.GetFrame(2)->GetAudioSamples(0)[0] == 2);
	CHECK(r.GetFrame(2)->GetAudioSamples(0)[1] == Approx(2.00068033).margin(0.00001));
	CHECK(r.GetFrame(2)->GetAudioSamples(0)[2] == Approx(2.00136054).margin(0.00001));

	// Clean up
	cache.Clear();
	r.Close();
}

TEST_CASE( "Invalid_Fake_Frame", "[libopenshot][dummyreader]" ) {
	// Create fake frames (with specific frame #, samples, and channels)
	auto f1 = std::make_shared<openshot::Frame>(1, 1470, 2);
	auto f2 = std::make_shared<openshot::Frame>(2, 1470, 2);

	// Add test frames to cache object
	openshot::CacheMemory cache;
	cache.Add(f1);
	cache.Add(f2);

	// Create a default fraction (should be 1/1)
	openshot::DummyReader r(openshot::Fraction(30, 1), 1920, 1080, 44100, 2, 30.0, &cache);
	r.Open();

	// Verify exception
	CHECK(r.GetFrame(1)->number == 1);
	CHECK(r.GetFrame(2)->number == 2);
	CHECK_THROWS_AS(r.GetFrame(3)->number, openshot::InvalidFile);

	// Clean up
	cache.Clear();
	r.Close();
}

TEST_CASE( "Json", "[libopenshot][dummyreader]") {
	openshot::DummyReader r1;
	openshot::DummyReader r2(openshot::Fraction(24, 1), 1280, 768, 44100, 2, 30.0);
	auto json1 = r1.Json();
	auto json2 = r2.JsonValue();
	auto json_string2 = json2.toStyledString();
	CHECK(json_string2 == json1);
}

TEST_CASE( "SetJson", "[libopenshot][dummyreader]") {
	openshot::DummyReader r1;
	std::stringstream json_stream;
	json_stream << R"json(
		{
			"width": 1920,
			"height": 1080,
			"fps": { "num": 15, "den": 1 },
			"duration": 15.0
		}
		)json";

	r1.SetJson(json_stream.str());
	CHECK(r1.info.width == 1920);
	CHECK(r1.info.height == 1080);
	CHECK(r1.info.fps.num == 15);
	CHECK(r1.info.fps.den == 1);
	CHECK(r1.info.duration == 15.0);
}
