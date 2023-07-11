#include <gst/gst.h>
#include <gst/video/video.h>

int main() {
  // Create a pipeline.
  gst_init(NULL, NULL);
  GstElement *pipeline = gst_pipeline_new("stereo-disparity");

  // Create the video sources.
  GstElement *source1 = gst_element_factory_create("v4l2src", "source1");
  GstElement *source2 = gst_element_factory_create("v4l2src", "source2");

  // Set the video source properties.
  gst_element_set_property(source1, "device", "/dev/video0");
  gst_element_set_property(source2, "device", "/dev/video1");

  // Create the disparity generator.
  GstElement *disparity = gst_element_factory_create("disparity", "disparity");

  // Set the disparity generator properties.
  gst_element_set_property(disparity, "left-input", "source1");
  gst_element_set_property(disparity, "right-input", "source2");

  // Create the video sink.
  GstElement *sink = gst_element_factory_create("xvimagesink", "sink");

  // Set the video sink properties.
  gst_element_set_property(sink, "width", 640);
  gst_element_set_property(sink, "height", 480);

  // Add the elements to the pipeline.
  gst_bin_add_many(GST_BIN(pipeline), source1, source2, disparity, sink);

  // Link the elements together.
  gst_element_link_many(source1, disparity, sink);
  gst_element_link_many(source2, disparity, sink);

  // Start the pipeline.
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // Wait for the pipeline to finish.
  gst_element_wait_until_state_changed(pipeline, GST_STATE_PLAYING, GST_STATE_PAUSED);

  // Cleanup.
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}