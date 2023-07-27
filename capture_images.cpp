//< g++ -std=c++17 -o capture_images gstreamer_captureImages.cpp `pkg-config --cflags --libs gstreamer-1.0`
#include </usr/include/gstreamer-1.0/gst/gst.h>
#include </usr/include/gstreamer-1.0/gst/app/gstappsink.h>
#include <stdio.h>
#include <string>
#include <unistd.h>  // for sleep
#include <filesystem>  // for directory creation

bool setup_directory(std::filesystem::path save_dir) {  
    bool directory_was_created = true; 
    if (std::filesystem::exists(save_dir)) {
        for (const auto & file : std::filesystem::directory_iterator(save_dir)) {
            std::filesystem::remove(file);
        }
    } else {
        bool directory_was_created = std::filesystem::create_directories(save_dir);
        if (directory_was_created) {
            printf("Directory was created.\n");
        } else {
            printf("Failed to create directory.\n");
        }
    } 
    return directory_was_created;
}

// Function to capture and save an image from a sink
void capture_image(GstElement* sink, const std::string& prefix, int count, const std::string& save_folder)
{
    GstSample* sample;
    g_signal_emit_by_name(sink, "pull-sample", &sample);

    if (sample) {
        GstBuffer* buffer = gst_sample_get_buffer(sample);
        GstMapInfo map;
        gst_buffer_map(buffer, &map, GST_MAP_READ);

        // Use the passed in save_folder instead of the out-of-scope variable
        std::string filename = save_folder + prefix + std::to_string(count) + ".jpeg";

        FILE* file = fopen(filename.c_str(), "wb");
        if (file) {
            fwrite(map.data, 1, map.size, file);
            fclose(file);
        }

        gst_buffer_unmap(buffer, &map);
        gst_sample_unref(sample);
    }
}

int main(int argc, char *argv[]) {
    
    int NUM_IMAGES = 5;

    if (argc != 2) {
        printf("capturing 5 images as no input argument entered\n");
    } else {
        NUM_IMAGES = std::stoi(argv[1]);
    }

    std::filesystem::path cwd = std::filesystem::current_path();
    std::filesystem::path save_dir = cwd / "caliberation";

    bool directory_setup_success = setup_directory(save_dir);
    if(!directory_setup_success) {
        printf("Directory setup failed!\n");
        return -1;
    }

    std::string SAVE_FOLDER = save_dir.string() + "/";

    GstElement *pipeline, *source1, *source2, *jpegenc1, *jpegenc2, *sink1, *sink2, *queue1, *queue2, *video_sink1, *video_sink2;
    GstBus *bus;

    /* Initialize GStreamer */
    gst_init(&argc, &argv);

    /* Create the elements */
    source1 = gst_element_factory_make("v4l2src", "source1");
    source2 = gst_element_factory_make("v4l2src", "source2");
    jpegenc1 = gst_element_factory_make("jpegenc", "jpegenc1");
    jpegenc2 = gst_element_factory_make("jpegenc", "jpegenc2");
    sink1 = gst_element_factory_make("appsink", "sink1");
    sink2 = gst_element_factory_make("appsink", "sink2");
    queue1 = gst_element_factory_make("queue", "queue1");
    queue2 = gst_element_factory_make("queue", "queue2");
    video_sink1 = gst_element_factory_make("autovideosink", "video_sink1");
    video_sink2 = gst_element_factory_make("autovideosink", "video_sink2");

    /* Modify the source's properties */
    g_object_set(source1, "device", "/dev/video0", NULL);
    g_object_set(source2, "device", "/dev/video2", NULL);

    /* Create the empty pipeline */
    pipeline = gst_pipeline_new("test-pipeline");

    if (!pipeline || !source1 || !source2 || !jpegenc1 || !jpegenc2 || !sink1 || !sink2 || !queue1 || !queue2 || !video_sink1 || !video_sink2) {
        g_printerr("Not all elements could be created.\n");
        return -1;
    }

    /* Build the pipeline */
    gst_bin_add_many(GST_BIN(pipeline), source1, jpegenc1, sink1, queue1, video_sink1, source2, jpegenc2, sink2, queue2, video_sink2, NULL);
    if (gst_element_link_many(source1, queue1, video_sink1, jpegenc1, sink1, NULL) != TRUE ||
        gst_element_link_many(source2, queue2, video_sink2, jpegenc2, sink2, NULL) != TRUE) {
        g_printerr("Elements could not be linked.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    bus = gst_element_get_bus(pipeline);

    /* Start playing */
    GstStateChangeReturn ret = gst_element_set_state(pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr("Unable to set the pipeline to the playing state.\n");
        gst_object_unref(pipeline);
        return -1;
    }

    /* Capture NUM_IMAGES from each source */
    for (int i = 0; i < NUM_IMAGES; i++) {
        printf("Capturing image %d\n", i+1);
        // Pass SAVE_FOLDER as an argument to the function calls
        capture_image(sink1, "Right_", i, SAVE_FOLDER);
        capture_image(sink2, "Left_", i, SAVE_FOLDER);
        sleep(3);  // Wait for 3 seconds before capturing the next image
    }

    gst_object_unref(bus);
    gst_object_unref(pipeline);

    return 0;
}

