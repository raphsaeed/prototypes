#include </usr/include/gstreamer-1.0/gst/gst.h>

//g++ -o my_program my_program.cpp `pkg-config --cflags --libs gstreamer-1.0`

int main(int argc, char *argv[]) {
  gst_init(&argc, &argv);

  GstElement *pipeline = gst_pipeline_new("pipeline");

  GstElement *source1 = gst_element_factory_make("v4l2src", "source1");
  g_object_set(source1, "device", "/dev/video0", NULL);
  GstElement *encoder1 = gst_element_factory_make("x264enc", "encoder1");
  GstElement *parser1 = gst_element_factory_make("h264parse", "parser1");

  GstElement *source2 = gst_element_factory_make("v4l2src", "source2");
  g_object_set(source2, "device", "/dev/video2", NULL);
  GstElement *encoder2 = gst_element_factory_make("x264enc", "encoder2");
  GstElement *parser2 = gst_element_factory_make("h264parse", "parser2");

  GstElement *mux = gst_element_factory_make("matroskamux", "mux");
  GstElement *sink = gst_element_factory_make("filesink", "sink");
  g_object_set(sink, "location", "output.mkv", NULL);

  gst_bin_add_many(GST_BIN(pipeline), source1, encoder1, parser1, source2, encoder2, parser2, mux, sink, NULL);
  gst_element_link_many(source1, encoder1, parser1, mux, NULL);
  gst_element_link_many(source2, encoder2, parser2, mux, NULL);
  gst_element_link(mux, sink);

  gst_element_set_state(pipeline, GST_STATE_PLAYING);

  GstBus *bus = gst_element_get_bus(pipeline);
GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, 
    static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));


  if (msg != NULL) gst_message_unref(msg);
  gst_object_unref(bus);
  gst_element_set_state(pipeline, GST_STATE_NULL);
  gst_object_unref(pipeline);

  return 0;
}


