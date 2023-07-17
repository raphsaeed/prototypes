#include </usr/include/gstreamer-1.0/gst/gst.h>
#include <stdio.h>
#include <string>

//> g++ -o capture_images capture_images.cpp $(pkg-config --cflags --libs gstreamer-1.0)

// Number of images to capture from each camera
const int NUM_IMAGES = 10;

// Folder to save the captured images
const std::string SAVE_FOLDER = "/path/to/save/folder/";

// Callback function to handle new video frames
static GstFlowReturn new_sample_cb(GstElement* pipeline, gpointer user_data)
{
    static int count1 = 0;
    static int count2 = 0;

    GstSample* sample;
    g_signal_emit_by_name(pipeline, "pull-sample", &sample);
    
    if (sample)
    {
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_READ);

        std::string filename;
        if (pipeline == pipeline1)
        {
            filename = SAVE_FOLDER + "image1_" + std::to_string(count1) + ".jpeg";
            count1++;
        }
        else if (pipeline == pipeline2)
        {
            filename = SAVE_FOLDER + "image2_" + std::to_string(count2) + ".jpeg";
            count2++;
        }

        // Save the image to file
        FILE* file = fopen(filename.c_str(), "wb");
        if (file)
        {
            fwrite(map.data, 1, map.size, file);
            fclose(file);
        }

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
    while (true)
    {
        g_main_context_iteration(NULL, FALSE);

        if (count1 >= NUM_IMAGES && count2 >= NUM_IMAGES)
            break;
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

