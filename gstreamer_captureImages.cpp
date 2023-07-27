//< g++ -std=c++17 -o capture_images gstreamer_captureImages.cpp `pkg-config --cflags --libs gstreamer-1.0`
#include </usr/include/gstreamer-1.0/gst/gst.h>
#include </usr/include/gstreamer-1.0/gst/app/gstappsink.h>
#include <stdio.h>
#include <string>
#include <unistd.h>  // for sleep
#include <filesystem>  // for directory creation

bool setup_directory(std::filesystem::path save_dir) {
    // Create directories and check if they already existed
    bool directory_was_created = std::filesystem::create_directories(save_dir);

    if (!directory_was_created) {
        for (const auto & file : std::filesystem::directory_iterator(save_dir)) {
            std::filesystem::remove(file);
        }
    }

    return directory_was_created;
}

void capture_image(GstElement* sink, const std::string& prefix, int count)
{
    // The rest of this function is unchanged
}

int main(int argc, char *argv[]) {
    
    int NUM_IMAGES = 5;

    if (argc != 2) {
        printf("capturing 5 images as no input argument entered\n");
    } else {
        NUM_IMAGES = std::stoi(argv[1]);
    }

    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path save_dir = cwd / "images2";

    std::string SAVE_FOLDER = save_dir.string() + "/";

    bool directory_setup_success = setup_directory(save_dir);
    if(!directory_setup_success) {
        printf("Directory setup failed!\n");
        return -1;
    }

    GstElement *pipeline, *source1, *source2, *jpegenc1, *jpegenc2, *sink1, *sink2;

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

    sleep(9);  // Let Camera's FOCUS

    /* Capture NUM_IMAGES from each source */
    for (int i = 0; i < NUM_IMAGES; i++) {
        printf("Capturing image %d\n", i+1);
        capture_image(sink1, "Right_", i);
        capture_image(sink2, "Left_", i);
        sleep(3);  // Wait for 3 seconds before capturing the next image
    }

    /* Free resources */
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(pipeline);
    printf("Capturing image\n");

    return 0;
}
