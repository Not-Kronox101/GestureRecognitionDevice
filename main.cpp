#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

#include "edge-impulse-sdk/classifier/ei_run_classifier.h"

#define SAMPLE_COUNT EI_CLASSIFIER_RAW_SAMPLE_COUNT

// Raw features buffer
float features[SAMPLE_COUNT * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME];


void extract_features() {
    adc_init();

    adc_gpio_init(26);
    adc_gpio_init(27);
    adc_gpio_init(28);

    // Read raw values from the sensors
    for (int i = 0; i < SAMPLE_COUNT; i++) {

        adc_select_input(0);
        uint16_t raw_x = adc_read() * 16;

        adc_select_input(1);
        uint16_t raw_y = adc_read() * 16;

        adc_select_input(2);
        uint16_t raw_z = adc_read() * 16;

        // Store raw as float
        features[i * 3 + 0] = (float)raw_x;
        features[i * 3 + 1] = (float)raw_y;
        features[i * 3 + 2] = (float)raw_z;

        printf("Sample %d: X=%u Y=%u Z=%u\n", i, raw_x, raw_y, raw_z);

        sleep_ms(EI_CLASSIFIER_INTERVAL_MS);
    }
}

int main() {
    stdio_init_all();
    printf("Edge Impulse Pico Inference Example\n");

    while (true) {
        printf("Collecting features...\n");
        extract_features();

        // Create signal object
        signal_t signal;
        int err = numpy::signal_from_buffer(features, SAMPLE_COUNT * EI_CLASSIFIER_RAW_SAMPLES_PER_FRAME, &signal);
        if (err != 0) {
            printf("Signal error: %d\n", err);
            continue;
        }

        // Run inference
        ei_impulse_result_t result = { 0 };
        EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

        printf("run_classifier returned: %d\n", res);

        if (res == EI_IMPULSE_OK) {
            for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
                printf("%s: %.5f\n", result.classification[ix].label, result.classification[ix].value);
            }
        }

        sleep_ms(500);
    }
}