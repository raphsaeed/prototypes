//>  g++ -std=c++11 -o display display_liveStereo.cpp $(pkg-config --cflags --libs gstreamer-1.0) -lpthread
//> g++ -o capture_images gstreamer_Camera_jetson_captureImages.cpp $(pkg-config --cflags --libs gstreamer-1.0)
#include </usr/include/gstreamer-1.0/gst/gst.h>

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source1, *source2, *sink1, *sink2;
    GstBus *bus;
    GstMessage *msg;
    GstStateChangeReturn ret;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    source1 = gst_element_factory_make("v4l2src", "source1");
    source2 = gst_element_factory_make("v4l2src", "source2");
    sink1 = gst_element_factory_make("autovideosink", "sink1");
    sink2 = gst_element_factory_make("autovideosink", "sink2");

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("test-pipeline");

    if (!pipeline || !source1 || !source2 || !sink1 || !sink2) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source1, sink1, source2, sink2, NULL);
    if (gst_element_link(source1, sink1) != TRUE || gst_element_link(source2, sink2) != TRUE) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Modify the source's properties */
    g_object_set(source1, "device", "/dev/video0", NULL);
    g_object_set(source2, "device", "/dev/video2", NULL);

    /* Start playing */
    ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Wait until error or EOS */
    bus = gst_element_get_bus(pipeline);
    msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE, 
        (GstMessageType)(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    /* Parse message */
    if (msg != NULL) {
        GError *err;
        gchar *debug_info;

        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error received from element %s: %s\n", GST_OBJECT_NAME(msg->src), err->message);
                g_printerr("Debugging information: %s\n", debug_info ? debug_info : "none");
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("End-Of-Stream reached.\n");
                break;
            default:
                /* We should not reach here because we only asked for ERRORs and EOS */
                g_printerr("Unexpected message received.\n");
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