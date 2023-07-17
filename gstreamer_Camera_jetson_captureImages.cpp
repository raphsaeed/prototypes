//>g++ -o capture_images gstreamer_Camera_jetson_captureImages.cpp $(pkg-config --cflags --libs gstreamer-1.0)
#include </usr/include/gstreamer-1.0/gst/gst.h>
#include <stdio.h>
#include <string>

// Number of images to capture from each camera
const int NUM_IMAGES = 10;

// Folder to save the captured images
const std::string SAVE_FOLDER = "/media/raphs/RaphsORIN/projects/GitHUB/prototypes/images/";

static int count1 = 0;
static int count2 = 0;

// Callback function to handle new video frames
static GstFlowReturn new_sample_cb(GstElement* appsink, gpointer user_data)
{
    GstSample* sample;
    g_signal_emit_by_name(appsink, "pull-sample", &sample);
    
    if (sample)
    {
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_READ);

        std::string filename;
        if (appsink == user_data)
        {
            if(count1 < NUM_IMAGES)
            {
                filename = SAVE_FOLDER + "image1_" + std::to_string(count1) + ".jpeg";
                count1++;
            }
            else
            {
                return GST_FLOW_OK;
            }
        }
        else
        {
            if(count2 < NUM_IMAGES)
            {
                filename = SAVE_FOLDER + "image2_" + std::to_string(count2) + ".jpeg";
                count2++;
            }
            else
            {
                return GST_FLOW_OK;
            }
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
    //GstElement* pipeline1 = gst_parse_launch("v4l2src device=/dev/video0 ! videoconvert ! video/x-raw,format=RGB ! appsink name=appsink1", NULL);
    //GstElement* pipeline2 = gst_parse_launch("v4l2src device=/dev/video2 ! videoconvert ! video/x-raw,format=RGB ! appsink name=appsink2", NULL);

    // Create GStreamer elements
    GstElement* pipeline1 = gst_parse_launch("v4l2src device=/dev/video2 ! videoconvert ! jpegenc ! appsink name=appsink1", NULL);
    GstElement* pipeline2 = gst_parse_launch("v4l2src device=/dev/video0 ! videoconvert ! jpegenc ! appsink name=appsink2", NULL);

    
    // Set callback function for new video frames
    GstElement* appsink1 = gst_bin_get_by_name(GST_BIN(pipeline1), "appsink1");
    g_object_set(appsink1, "emit-signals", TRUE, NULL);
    g_signal_connect(appsink1, "new-sample", G_CALLBACK(new_sample_cb), appsink1);
    
    GstElement* appsink2 = gst_bin_get_by_name(GST_BIN(pipeline2), "appsink2");
    g_object_set(appsink2, "emit-signals", TRUE, NULL);
    g_signal_connect(appsink2, "new-sample", G_CALLBACK(new_sample_cb), appsink2);
    
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
