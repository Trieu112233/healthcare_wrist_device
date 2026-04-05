#include <stdio.h>

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

extern const ei_impulse_t impulse;

// custom function to display the results
static void display_custom_results(ei_impulse_result_t* result, const ei_impulse_t* impulse);
// get_signal declaration inserted here

static int get_signal_data_916888(size_t offset, size_t length, float *out_ptr);
static int get_signal_data_868007(size_t offset, size_t length, float *out_ptr);

// raw features array inserted here

static const float features_916888[] = { ... }; // copy features from project 916888
static const float features_868007[] = { ... }; // copy features from project 868007

int main(int argc, char **argv) {

    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

// process_impulse inserted here


    // new process_impulse call for project ID 916888
    signal.total_length = impulse_916888_1.dsp_input_frame_size;
    signal.get_data = &get_signal_data_916888;
    res = process_impulse(&impulse_handle_916888_1, &signal, &result, false);
    printf("process_impulse for project 916888 returned: %d\r\n", res);
    display_custom_results(&result, &impulse_916888_1);
    

    // new process_impulse call for project ID 868007
    signal.total_length = impulse_868007_1.dsp_input_frame_size;
    signal.get_data = &get_signal_data_868007;
    res = process_impulse(&impulse_handle_868007_1, &signal, &result, false);
    printf("process_impulse for project 868007 returned: %d\r\n", res);
    display_custom_results(&result, &impulse_868007_1);
    

    return 0;
}

// callback functions inserted here


static int get_signal_data_916888(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (features_916888 + offset)[i];
    }
    return EIDSP_OK;
}


static int get_signal_data_868007(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = (features_868007 + offset)[i];
    }
    return EIDSP_OK;
}


static void display_custom_results(ei_impulse_result_t* result, const ei_impulse_t *impulse)
{
    printf("Timing: DSP %d ms, inference %d ms, anomaly %d ms\r\n",
           result->timing.dsp,
           result->timing.classification,
           result->timing.anomaly);

    // Print the prediction results (object detection)
    if (result->bounding_boxes_count > 0) {
        printf("Object detection bounding boxes:\r\n");
        for (uint32_t i = 0; i < result->bounding_boxes_count; i++) {
            ei_impulse_result_bounding_box_t bb = result->bounding_boxes[i];
            if (bb.value == 0) {
                continue;
            }
            printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                   bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
        }
    } else {
        // Print the prediction results (classification)
        printf("Predictions:\r\n");
        for (uint16_t i = 0; i < impulse->label_count; i++) {
            printf("  %s: %.5f\r\n", impulse->categories[i], result->classification[i].value);
        }
        // Print anomaly result (if it exists)
        if(impulse->has_anomaly > 0){
            printf("Anomaly prediction: %.3f\r\n", result->anomaly);
        }
    }
#if EI_CLASSIFIER_HAS_VISUAL_ANOMALY
    // Print visual anomaly results (if applicable)
    if (impulse->has_anomaly == 3 && result->visual_ad_count > 0) {
        printf("Visual anomalies:\r\n");
        for (uint32_t i = 0; i < result->visual_ad_count; i++) {
            ei_impulse_result_bounding_box_t bb = result->visual_ad_grid_cells[i];
            if (bb.value == 0) {
                continue;
            }
            printf("  %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\r\n",
                   bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
        }
        printf("Visual anomaly values: Mean : %.3f Max : %.3f\r\n",
               result->visual_ad_result.mean_value, result->visual_ad_result.max_value);
    }
#endif
    printf("-----------------------------------------------------\n");
}