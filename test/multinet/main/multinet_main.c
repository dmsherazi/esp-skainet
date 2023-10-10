#include "mn_perf_tester.h"
#include "model_path.h"
#include "esp_board_init.h"
#include "esp_mn_speech_commands.h"
#include "esp_process_sdkconfig.h"

static int start_multinet_test(int argc, char **argv)
{
    printf("Start to test MultiNet:\n");

    srmodel_list_t *models = esp_srmodel_init("model");
    char *wn_name = esp_srmodel_filter(models, ESP_WN_PREFIX, NULL);
    char *mn_name = esp_srmodel_filter(models, ESP_MN_PREFIX, NULL);
    char csv_file[128];
    char log_file[128];
    sprintf(csv_file, "/sdcard/%s.csv", mn_name);
    sprintf(log_file, "/sdcard/%s.log", mn_name);
    printf("test:%s, log:%s\n", csv_file, log_file);

    // Select speech enhancement pipeline
    afe_config_t afe_config = AFE_CONFIG_DEFAULT();
    afe_config.wakenet_model_name = wn_name;
    afe_config.wakenet_mode = DET_MODE_3CH_90;

    // Multinet
    esp_mn_iface_t *multinet = esp_mn_handle_from_name(mn_name);

    perf_tester_config_t *tester_config = get_perf_tester_config();
    offline_mn_tester(csv_file, log_file, &ESP_AFE_SR_HANDLE, &afe_config,
                      multinet, mn_name, TESTER_WAV_3CH, tester_config);
    return 0;
}

void app_main()
{
    ESP_ERROR_CHECK(esp_board_init(16000, 1, 16));
    ESP_ERROR_CHECK(esp_sdcard_init("/sdcard", 10));
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);

    // Init console repl
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_config = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_config.prompt = "perf_tester>";
    esp_console_dev_uart_config_t uart_config = ESP_CONSOLE_DEV_UART_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_uart(&uart_config, &repl_config, &repl));

    // Register cmd
    esp_console_register_help_command();
    register_perf_tester_config_cmd();
    register_perf_tester_start_cmd(&start_multinet_test);

    // Start console repl
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}