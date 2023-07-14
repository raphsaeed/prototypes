#include </usr/include/gstreamer-1.0/gst/gst.h>

//g++ -o my_program my_program.cpp `pkg-config --cflags --libs gstreamer-1.0`

int main(int argc, char *argv[]) {
    // Initialize GStreamer.
    gst_init(&argc, &argv);

    // Create a pipeline.
    GstElement *pipeline = gst_pipeline_new("pipeline");

    // Create two video sources.
    GstElement *source1 = gst_element_factory_make("v4l2src", "source1");
    GstElement *source2 = gst_element_factory_make("v4l2src", "source2");

    // Set the device names for the video sources.
    g_object_set(source1, "device", "/dev/video0", NULL);
    g_object_set(source2, "device", "/dev/video2", NULL);

    // Create a muxer to combine the video streams.
    GstElement *muxer = gst_element_factory_make("flvmux", "muxer");

    // Create a sink to write the video to a file.
    GstElement *sink = gst_element_factory_make("filesink", "sink");

    // Set the output filename for the sink.
    g_object_set(sink, "location", "output.flv", NULL);

    // Add the elements to the pipeline.
    gst_bin_add_many(GST_BIN(pipeline), source1, source2, muxer, sink, NULL);

    // Link the elements together.
    if (!gst_element_link_many(source1, muxer, NULL) || !gst_element_link_many(source2, muxer, NULL) || !gst_element_link(muxer, sink)) {
        g_warning("Failed to link elements!");
    }

    // Start the pipeline.
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Wait for EOS or ERROR
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    // Free resources
    if (msg != NULL)
        gst_message_unref(msg);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
