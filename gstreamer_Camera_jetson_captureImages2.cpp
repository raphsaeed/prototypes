//< g++ -o capture_images gstreamer_Camera_jetson_captureImages.cpp $(pkg-config --cflags --libs gstreamer-1.0)
#include </usr/include/gstreamer-1.0/gst/gst.h>
#include </usr/include/gstreamer-1.0/gst/app/gstappsink.h>
#include <stdio.h>
#include <string>
#include <unistd.h>  // for sleep

// Number of images to capture from each camera
const int NUM_IMAGES = 10;

// Folder to save the captured images
const std::string SAVE_FOLDER = "/media/raphs/RaphsORIN/projects/GitHUB/prototypes/images/";

GstElement *sink1;
GstElement *sink2;
GMainLoop *loop;

// Callback function to handle new video frames
GstFlowReturn new_sample_cb(GstElement* sink, gpointer user_data)
{
    static int count1 = 0;
    static int count2 = 0;

    GstSample* sample;
    g_signal_emit_by_name(sink, "pull-sample", &sample);
    
    if (sample) {
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_READ);

        std::string filename;
        if (sink == sink1) {
            filename = SAVE_FOLDER + "image1_" + std::to_string(count1) + ".jpeg";
            count1++;
            sleep(3);  // Add a delay of 3 seconds between each image capture
        } else if (sink == sink2) {
            filename = SAVE_FOLDER + "image2_" + std::to_string(count2) + ".jpeg";
            count2++;
            sleep(3);  // Add a delay of 3 seconds between each image capture
        }

        // Save the image to file
        FILE* file = fopen(filename.c_str(), "wb");
        if (file) {
            fwrite(map.data, 1, map.size, file);
            fclose(file);
        }

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);
    }

    // Stop capturing after NUM_IMAGES
    if (count1 >= NUM_IMAGES && count2 >= NUM_IMAGES) {
        g_main_loop_quit(loop);
    }
    
    return GST_FLOW_OK;
}

int main(int argc, char *argv[]) {
    GstElement *pipeline, *source1, *source2, *jpegenc1, *jpegenc2;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    source1 = gst_element_factory_make("v4l2src", "source1");
    source2 = gst_element_factory_make("v4l2src", "source2");
    jpegenc1 = gst_element_factory_make("jpegenc", "jpegenc1");
    jpegenc2 = gst_element_factory_make("jpegenc", "jpegenc2");
    sink1 = gst_element_factory_make("appsink", "sink1");
    sink2 = gst_element_factory_make("appsink", "sink2");

    /* Modify the source's properties */
    g_object_set(source1, "device", "/dev/video0", NULL);
    g_object_set(source2, "device", "/dev/video2", NULL);

    /* Modify the sink's properties to emit signals for each new frame */
    g_object_set(sink1, "emit-signals", TRUE, NULL);
    g_signal_connect(sink1, "new-sample", G_CALLBACK(new_sample_cb), NULL);
    g_object_set(sink2, "emit-signals", TRUE, NULL);
    g_signal_connect(sink2, "new-sample", G_CALLBACK(new_sample_cb), NULL);

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("test-pipeline");

    if (!pipeline || !source1 || !source2 || !jpegenc1 || !jpegenc2 || !sink1 || !sink2) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source1, jpegenc1, sink1, source2, jpegenc2, sink2, NULL);
    if (gst_element_link_many(source1, jpegenc1, sink1, NULL) != TRUE ||
        gst_element_link_many(source2, jpegenc2, sink2, NULL) != TRUE) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Start playing */
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Run the main loop */
    loop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(loop);

    /* Free resources */
    g_main_loop_unref(loop);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);

    return 0;
}
