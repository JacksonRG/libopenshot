/**
 * @file
 * @brief Source file for VideoCacheThread class
 * @author Jonathan Thomas <jonathan@openshot.org>
 *
 * @ref License
 */

// Copyright (c) 2008-2019 OpenShot Studios, LLC
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "VideoCacheThread.h"

#include "CacheBase.h"
#include "Exceptions.h"
#include "Frame.h"
#include "OpenMPUtilities.h"
#include "ZmqLogger.h"

#include <algorithm>
#include <thread>    // for std::this_thread::sleep_for
#include <chrono>    // for std::chrono::microseconds

namespace openshot
{
	// Constructor
	VideoCacheThread::VideoCacheThread()
	: Thread("video-cache"), speed(1), is_playing(false),
	reader(NULL), max_frames_ahead(OPEN_MP_NUM_PROCESSORS * 2), current_display_frame(1)
    {
    }

    // Destructor
	VideoCacheThread::~VideoCacheThread()
    {
    }

	// Seek the reader to a particular frame number
	void VideoCacheThread::Seek(int64_t new_position)
	{
		current_display_frame = new_position;
	}

	// Play the video
	void VideoCacheThread::Play() {
		// Start playing
		is_playing = true;
	}

	// Stop the audio
	void VideoCacheThread::Stop() {
		// Stop playing
		is_playing = false;
	}

    // Start the thread
    void VideoCacheThread::run()
    {
      return;
        // Types for storing time durations in whole and fractional microseconds
        using micro_sec = std::chrono::microseconds;
        using double_micro_sec = std::chrono::duration<double, micro_sec::period>;

		while (!threadShouldExit() && is_playing) {
            // Calculate on-screen time for a single frame
            const auto frame_duration = double_micro_sec(1000000.0 / reader->info.fps.ToDouble());

            // Calculate bytes per frame. If we have a reference openshot::Frame, use that instead (the preview
            // window can be smaller, can thus reduce the bytes per frame)
            int64_t bytes_per_frame = (reader->info.height * reader->info.width * 4) +
                    (reader->info.sample_rate * reader->info.channels * 4);
            // if (last_cached_frame && last_cached_frame->has_image_data && last_cached_frame->has_audio_data) {
            //     bytes_per_frame = last_cached_frame->GetBytes();
            // }

            // Calculate # of frames on Timeline cache
            if (reader->GetCache() && reader->GetCache()->GetMaxBytes() > 0) {
                // Use 1/2 the cache size (so our cache will be 50% before the play-head, and 50% after it)
                max_frames_ahead = (reader->GetCache()->GetMaxBytes() / bytes_per_frame) / 2;
                // if (max_frames_ahead > 1000) {
                //     // Ignore values that are too large, and default to a safer value
                //     // max_frames_ahead = OPEN_MP_NUM_PROCESSORS * 2;
                //   max_frames_ahead = 1000;
                // }
            }

            // Calculate increment (based on speed)
            // Support caching in both directions
            int16_t increment = 1;
            if (speed < 0) {
                increment = -1;
            }

			// Always cache frames from the current display position to our maximum (based on the cache size).
			// Frames which are already cached are basically free. Only uncached frames have a big CPU cost.
			// By always looping through the expected frame range, we can fill-in missing frames caused by a
			// fragmented cache object (i.e. the user clicking all over the timeline).
            // int64_t starting_frame = current_display_frame - (max_frames_ahead / 2);
            // if (starting_frame < 0) {
            //   starting_frame = 0;
            // }
            int64_t starting_frame = current_display_frame;
            int64_t ending_frame = starting_frame + max_frames_ahead;
            if (speed < 0) {
                ending_frame = starting_frame - max_frames_ahead;
            }

            for (int64_t cache_frame = starting_frame; cache_frame <= ending_frame; cache_frame += increment) {
              if (current_display_frame > cache_frame || current_display_frame < starting_frame) {
                break;
              }
                if (reader && reader->GetCache() && !reader->GetCache()->Contains(cache_frame)) {
                    try
                    {
                        // This frame is not already cached... so request it again (to force the creation & caching)
                        // This will also re-order the missing frame to the front of the cache
                        last_cached_frame = reader->GetFrame(cache_frame);
                    }
                    catch (const OutOfBoundsFrame & e) {  }
                }
                // Sleep a short time to allow the video cache thread to start
                // if (cache_frame % 10 == 0){
                //   // std::this_thread::sleep_for(frame_duration / 40);
                //   std::this_thread::sleep_for(frame_duration);
                // }
                // std::this_thread::sleep_for(frame_duration / 10);
            }

			// Sleep for a fraction of frame duration
			std::this_thread::sleep_for(frame_duration);
		}

	return;
    }

  int VideoCacheThread::MaxFramesAhead() {
      if (reader && reader->GetCache() && reader->GetCache()->GetMaxBytes() > 0) {
          int64_t bytes_per_frame = (reader->info.height * reader->info.width * 4) +
                                    (reader->info.sample_rate * reader->info.channels * 4);
          return (reader->GetCache()->GetMaxBytes() / bytes_per_frame) / 2;
      }
      return 0;
  }
}
