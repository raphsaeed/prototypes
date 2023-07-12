#include </usr/include/gstreamer-1.0/gst/gst.h>
//g++ -o disparity gstreamer_Camera_Disparity2.cpp $(pkg-config --cflags --libs gstreamer-1.0)

int main() {
  // Initialize GStreamer.
  gst_init(NULL, NULL);

  // Create a pipeline.
  GstElement *pipeline = gst_pipeline_new("pipeline");

  // Create the video sources.
  GstElement *source1 = gst_element_factory_make("v4l2src", "source1");
  GstElement *source2 = gst_element_factory_make("v4l2src", "source2");

  // Set the device names for the video sources.
  g_object_set(source1, "device", "/dev/video0", NULL);
  g_object_set(source2, "device", "/dev/video2", NULL);

  // Create queues.
  GstElement *queue1 = gst_element_factory_make("queue", "queue1");
  GstElement *queue2 = gst_element_factory_make("queue", "queue2");

  // Create the video sink.
  GstElement *sink = gst_element_factory_make("autovideosink", "sink");

  // Add the elements to the pipeline.
  gst_bin_add_many(GST_BIN(pipeline), source1, queue1, source2, queue2, sink, NULL);

  // Link the elements together.
  gst_element_link_many(source1, queue1, sink, NULL);
  gst_element_link_many(source2, queue2, sink, NULL);

  // Start the pipeline.
  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  // Wait for the pipeline to finish.
  GMainLoop *loop = g_main_loop_new(NULL, FALSE);
  g_main_loop_run(loop);

  // Cleanup.
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}
