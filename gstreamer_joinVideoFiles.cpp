
//>  g++ -o gstreamer_joinVideoFiles gstreamer_joinVideoFiles.cpp $(pkg-config --cflags --libs gstreamer-1.0)
#include </usr/include/gstreamer-1.0/gst/gst.h>
#include <iostream>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source1, *source2, *decode1, *decode2, *convert1, *convert2, *mixer, *encoder, *muxer, *sink;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    source1 = gst_element_factory_make("filesrc", "source1");
    source2 = gst_element_factory_make("filesrc", "source2");
    decode1 = gst_element_factory_make("decodebin", "decode1");
    decode2 = gst_element_factory_make("decodebin", "decode2");
    convert1 = gst_element_factory_make("videoconvert", "convert1");
    convert2 = gst_element_factory_make("videoconvert", "convert2");
    mixer = gst_element_factory_make("videomixer", "mixer");
    encoder = gst_element_factory_make("x264enc", "encoder");
    muxer = gst_element_factory_make("mp4mux", "muxer");
    sink = gst_element_factory_make("filesink", "sink");

    /* Modify the source's properties */
    g_object_set(source1, "location", "video1.mp4", NULL);
    g_object_set(source2, "location", "video2.mp4", NULL);

    /* Modify the sink's properties */
    g_object_set(sink, "location", "output.mp4", NULL);

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("join-videos-pipeline");

    if (!pipeline || !source1 || !source2 || !decode1 || !decode2 || !convert1 || !convert2 || !mixer || !encoder || !muxer || !sink) {
        std::cerr << "Not all elements could be created." << std::endl;
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source1, decode1, convert1, mixer, encoder, muxer, sink, source2, decode2, convert2, NULL);
    if (!gst_element_link_many(source1, decode1, convert1, mixer, encoder, muxer, sink, NULL) ||
        !gst_element_link_many(source2, decode2, convert2, mixer, NULL)) {
        std::cerr << "Elements could not be linked." << std::endl;
        gst_object_unref(pipeline);
        return -1;
    }

    /* Start playing */
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        std::cerr << "Unable to set the pipeline to the playing state." << std::endl;
        gst_object_unref(pipeline);
        return -1;
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, GST_MESSAGE_ERROR | GST_MESSAGE_EOS);

    /* Parse message */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
        case GST_MESSAGE_ERROR:
            gst_message_parse_error(msg, &err, &debug_info);
            std::cerr << "Error received from element " << GST_OBJECT_NAME(msg->src) << ": " << err->message << std::endl;
            std::cerr << "Debugging information: " << (debug_info ? debug_info : "none") << std::endl;
            g_clear_error(&err);
            g_free(debug_info);
            break;
        case GST_MESSAGE_EOS:
            std::cout << "End-Of-Stream reached." << std::endl;
            break;
        default:
            std::cerr << "Unexpected message received." << std::endl;
            break;
        }
        gst_message_unref(msg);
    }

    /* Free resources */
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
