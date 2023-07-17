#include </usr/include/gstreamer-1.0/gst/gst.h>
#include <stdio.h>
//> g++ -o capture_images capture_images.cpp $(pkg-config --cflags --libs gstreamer-1.0)

// Number of images to capture from each camera
const int NUM_IMAGES = 10;

// Callback function to handle new video frames
static GstFlowReturn new_sample_cb(GstElement* pipeline, gpointer user_data)
{
    GstSample* sample;
    g_signal_emit_by_name(pipeline, "pull-sample", &sample);
    
    if (sample)
    {
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_READ);
        
        // TODO: Save the image to file or perform further processing
        
        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);
    }
    
    return GST_FLOW_OK;
}

int main(int argc, char* argv[])
{
    gst_init(&argc, &argv);
    
    // Create GStreamer elements
    GstElement* pipeline1 = gst_parse_launch("v4l2src device=/dev/video0 ! videoconvert ! video/x-raw,format=RGB ! appsink name=appsink1", NULL);
    GstElement* pipeline2 = gst_parse_launch("v4l2src device=/dev/video1 ! videoconvert ! video/x-raw,format=RGB ! appsink name=appsink2", NULL);
    
    // Set callback function for new video frames
    GstElement* appsink1 = gst_bin_get_by_name(GST_BIN(pipeline1), "appsink1");
    g_object_set(appsink1, "emit-signals", TRUE, NULL);
    g_signal_connect(appsink1, "new-sample", G_CALLBACK(new_sample_cb), NULL);
    
    GstElement* appsink2 = gst_bin_get_by_name(GST_BIN(pipeline2), "appsink2");
    g_object_set(appsink2, "emit-signals", TRUE, NULL);
    g_signal_connect(appsink2, "new-sample", G_CALLBACK(new_sample_cb), NULL);
    
    // Start the pipelines
    gst_element_set_state(pipeline1, GST_STATE_PLAYING);
    gst_element_set_state(pipeline2, GST_STATE_PLAYING);
    
    // Wait for the specified number of images from each camera
    int count1 = 0;
    int count2 = 0;
    
    while (count1 < NUM_IMAGES || count2 < NUM_IMAGES)
    {
        GstMessage* msg = gst_bus_timed_pop_filtered(gst_element_get_bus(pipeline1), GST_CLOCK_TIME_NONE,
                                                     GST_MESSAGE_ERROR | GST_MESSAGE_EOS);
        
        if (msg != NULL)
        {
            GError* err;
            gchar* debug_info;
            
            switch (GST_MESSAGE_TYPE(msg))
            {
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
                    g_printerr("Unexpected message received.\n");
                    break;
            }
            
            gst_message_unref(msg);
            break;
        }
        
        g_main_context_iteration(NULL, FALSE);
    }
    
    // Stop the pipelines
    gst_element_set_state(pipeline1, GST_STATE_NULL);
    gst_element_set_state(pipeline2, GST_STATE_NULL);
    
    // Clean up
    gst_object_unref(appsink1);
    gst_object_unref(appsink2);
    gst_object_unref(pipeline1);
    gst_object_unref(pipeline2);
    
    return 0;
}
