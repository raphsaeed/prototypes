#include </usr/include/gstreamer-1.0/gst/gst.h>

int main() {
  // Initialize GStreamer.
  gst_init(NULL, NULL);

  // Create a pipeline.
  GstElement *pipeline = gst_pipeline_new("pipeline");

  // Create two video sources.
  GstElement *source1 = gst_element_factory_create("v4l2src", "source1");
  GstElement *source2 = gst_element_factory_create("v4l2src", "source2");

  // Set the device names for the video sources.
  g_object_set(source1, "device", "/dev/video0", NULL);
  g_object_set(source2, "device", "/dev/video2", NULL);

  // Create a muxer to combine the video streams.
  GstElement *muxer = gst_element_factory_create("flvmux", "muxer");

  // Create a sink to write the video to a file.
  GstElement *sink = gst_element_factory_create("filesink", "sink");

  // Set the output filename for the sink.
  g_object_set(sink, "location", "output.flv", NULL);

  // Add the elements to the pipeline.
  gst_bin_add_many(GST_BIN(pipeline), source1, source2, muxer, sink, NULL);

  // Link the elements together.
  gst_element_link_many(source1, muxer, sink, NULL);
  gst_element_link_many(source2, muxer, sink, NULL);

  // Start the pipeline.
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // Wait for the pipeline to finish.
  gst_element_wait_until_state_changed(pipeline, GST_STATE_PLAYING, GST_STATE_PAUSED);

  // Cleanup.
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}