# © OpenShot Studios, LLC
#
# SPDX-License-Identifier: LGPL-3.0-or-later

# Find and load the ruby libopenshot wrapper library
require "openshot"

# Create a new FFmpegReader and Open it
r = Openshot::FFmpegReader.new("test.mp4")
r.Open()

# Get frame 1
f = r.GetFrame(1)

# Display the frame
f.Display()

