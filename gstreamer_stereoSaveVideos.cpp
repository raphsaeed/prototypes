//> g++ -o gstreamer_stereoSaveVideos gstreamer_stereoSaveVideos.cpp $(pkg-config --cflags --libs gstreamer-1.0)
#include </usr/include/gstreamer-1.0/gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline1, *pipeline2, *source1, *source2, *convert1, *convert2, *encode1, *encode2, *mux1, *mux2, *sink1, *sink2;
    GstBus *bus1, *bus2;
    GstMessage *msg1, *msg2;
    GstStateChangeReturn ret1, ret2;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    source1 = gst_element_factory_make("v4l2src", "source1");
    source2 = gst_element_factory_make("v4l2src", "source2");
    convert1 = gst_element_factory_make("videoconvert", "convert1");
    convert2 = gst_element_factory_make("videoconvert", "convert2");
    encode1 = gst_element_factory_make("x264enc", "encode1");
    encode2 = gst_element_factory_make("x264enc", "encode2");
    mux1 = gst_element_factory_make("mp4mux", "mux1");
    mux2 = gst_element_factory_make("mp4mux", "mux2");
    sink1 = gst_element_factory_make("filesink", "sink1");
    sink2 = gst_element_factory_make("filesink", "sink2");

    /* Create the empty pipelines */
    pipeline1 = gst_pipeline_new("test-pipeline1");
    pipeline2 = gst_pipeline_new("test-pipeline2");

    if (!pipeline1 || !pipeline2 || !source1 || !source2 || !convert1 || !convert2 || !encode1 || !encode2 || !mux1 || !mux2 || !sink1 || !sink2) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipelines */
    gst_bin_add_many(GST_BIN(pipeline1), source1, convert1, encode1, mux1, sink1, NULL);
    gst_bin_add_many(GST_BIN(pipeline2), source2, convert2, encode2, mux2, sink2, NULL);
    if (gst_element_link_many(source1, convert1, encode1, mux1, sink1, NULL) != TRUE ||
        gst_element_link_many(source2, convert2, encode2, mux2, sink2, NULL) != TRUE) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline1);
        gst_object_unref(pipeline2);
        return -1;
    }

    /* Modify the source's properties */
    g_object_set(source1, "device", "/dev/video0", NULL);
    g_object_set(source2, "device", "/dev/video2", NULL);

    /* Modify the sink's properties */
    g_object_set(sink1, "location", "output1.mp4", NULL);
    g_object_set(sink2, "location", "output2.mp4", NULL);

    /* Start playing */
    ret1 = gst_element_set_state(pipeline1, GST_STATE_PLAYING);
    ret2 = gst_element_set_state(pipeline2, GST_STATE_PLAYING);
    if (ret1 == GST_STATE_CHANGE_FAILURE || ret2 == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipelines to the playing state.\n");
        gst_object_unref(pipeline1);
        gst_object_unref(pipeline2);
        return -1;
    }

    /* Capture video for a specific duration (e.g., 10 seconds) */
    gst_element_seek_simple(pipeline1, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), 10 * GST_SECOND);
    gst_element_seek_simple(pipeline2, GST_FORMAT_TIME, (GstSeekFlags)(GST_SEEK_FLAG_FLUSH | GST_SEEK_FLAG_KEY_UNIT), 10 * GST_SECOND);

    /* Wait until error or EOS */
    bus1 = gst_element_get_bus(pipeline1);
    bus2 = gst_element_get_bus(pipeline2);
    msg1 = gst_bus_timed_pop_filtered(bus1, GST_CLOCK_TIME_NONE, 
        (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));
    msg2 = gst_bus_timed_pop_filtered(bus2, GST_CLOCK_TIME_NONE, 
        (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse messages */
    // Parse message for pipeline1
    // ...
    // Parse message for pipeline2
    // ...

    /* Free resources */
    gst_object_unref(bus1);
    gst_object_unref(bus2);
    gst_element_set_state(pipeline1, GST_STATE_NULL);
    gst_element_set_state(pipeline2, GST_STATE_NULL);
    gst_object_unref(pipeline1);
    gst_object_unref(pipeline2);

    return 0;
}
