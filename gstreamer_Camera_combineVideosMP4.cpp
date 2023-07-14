#include </usr/include/gstreamer-1.0/gst/gst.h>

//g++ -o my_program my_program.cpp `pkg-config --cflags --libs gstreamer-1.0`


int main() {
  // Initialize GStreamer.
  gst_init(NULL, NULL);

  // Create a pipeline.
  gst_element_factory_set_data(
      gst_element_factory_find("videotestsrc"), "num-buffers", G_TYPE_INT, 10);
  GstElement *pipeline = gst_pipeline_new("pipeline");

  // Create the video sources.
  GstElement *source1 = gst_element_factory_create("v4l2src", "source1");
  GstElement *source2 = gst_element_factory_create("v4l2src", "source2");

  // Set the properties of the video sources.
  gst_element_set_property(source1, "device", "/dev/video0");
  gst_element_set_property(source2, "device", "/dev/video1");

  // Add the video sources to the pipeline.
  gst_bin_add_many(GST_BIN(pipeline), source1, source2, NULL);

  // Create the video sink.
  GstElement *sink = gst_element_factory_create("filesink", "sink");

  // Set the properties of the video sink.
  gst_element_set_property(sink, "location", "output.mp4");

  // Add the video sink to the pipeline.
  gst_bin_add(GST_BIN(pipeline), sink);

  // Link the elements in the pipeline.
  gst_element_link_many(source1, source2, sink, NULL);

  // Start the pipeline.
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // Wait for the pipeline to finish.
  gst_element_wait_until_state_changed(pipeline, GST_STATE_PLAYING, GST_STATE_PAUSED);

  // Cleanup.
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}

