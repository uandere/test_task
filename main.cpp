#include <string>
#include <tuple>
#include <gst/gst.h>
#include "utils.h"

int main(int argc, char *argv[]) {

    // Parsing arguments
    const auto &[inputFile, outputFile, is_verbose] = parse_args(argc, argv);

    // Initializing GStreamer
    gst_init(nullptr, nullptr);

    // Providing progress information
    std::string audio_progress = "";
    std::string video_progress = "";
    std::string mux_progress = "";

    if (is_verbose) {
        audio_progress = "! progressreport name=audio_processed:";
        video_progress = "! progressreport name=video_processed:";
        mux_progress   = "! progressreport name=muxed_completed:";
    }

    // Pipeline description string
    std::string pipeline_description_str = std::string("filesrc location=") + inputFile +
                                           " ! qtdemux name=demux demux.audio_0 ! queue ! decodebin " + audio_progress +
                                           " ! audioconvert ! audioresample ! opusenc "
                                           " ! mux.  demux.video_0 ! queue ! decodebin " + video_progress +
                                           " ! videoconvert ! av1enc ! mux. matroskamux name=mux " + mux_progress +
                                           " ! filesink location=" + outputFile;
    const gchar *pipeline_description = pipeline_description_str.c_str();

    // Creating the pipeline from the description string
    GstElement *pipeline = gst_parse_launch(pipeline_description, nullptr);

    // Checking if the pipeline creation was successful
    if (!pipeline) {
        g_printerr("Failed to create pipeline\n");
        return -1;
    }

    // Starting the pipeline
    g_print("Starting the pipeline...\n");
    gst_element_set_state(pipeline, GST_STATE_PLAYING);

    // Running the main loop
    GstBus *bus = gst_element_get_bus(pipeline);
    GstMessage *msg = gst_bus_timed_pop_filtered(bus, GST_CLOCK_TIME_NONE,
                                                 static_cast<GstMessageType>(GST_MESSAGE_ERROR | GST_MESSAGE_EOS));

    // Creating error_code variable for correct pipeline destruction
    int error_code = 0;

    // Parsing and handling the message
    if (msg != nullptr) {
        GError *err = nullptr;
        gchar *debug_info = nullptr;

        switch (GST_MESSAGE_TYPE(msg)) {
            case GST_MESSAGE_ERROR:
                gst_message_parse_error(msg, &err, &debug_info);
                g_printerr("Error from GStreamer: %s\n", debug_info ? debug_info : "none");

                // getting correct error code
                if (std::string(debug_info).contains("No such file")) {
                    error_code = static_cast<int>(error_codes::UnableToOpenInputFile);
                } else {
                    error_code = static_cast<int>(error_codes::PipelineInternalError);
                }
                g_clear_error(&err);
                g_free(debug_info);
                break;
            case GST_MESSAGE_EOS:
                g_print("Finished.\n");
                break;
            default:
                g_printerr("Unexpected message received\n");
                error_code = static_cast<int>(error_codes::PipelineInternalError);
                break;
        }

        gst_message_unref(msg);
    }

    // Cleanup
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_object_unref(GST_OBJECT(pipeline));

    return error_code;
}
