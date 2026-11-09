// Adafruit Matrix Portal S3 - HUB75 LED matrix base project.
//
// ESP-IDF equivalent of the RP2350 weather_thing setup: the esphome/esp-hub75
// component refreshes the panel continuously over GDMA/LCD_CAM, just like
// Interstate75 does on the RP2350. This file only proves the panel works:
// it cycles R/G/B fills and bounces a white pixel around the edge.

#include "hub75.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

namespace {
constexpr char kTag[] = "matrix";

// --- Matrix Portal S3 HUB75 wiring ------------------------------------------
// Fixed wiring of the onboard HUB75 connector (matches Adafruit's Protomatter
// example for this board). Don't change these unless the board changes.
constexpr int8_t kPinR1 = 42, kPinG1 = 41, kPinB1 = 40;
constexpr int8_t kPinR2 = 38, kPinG2 = 39, kPinB2 = 37;
constexpr int8_t kPinA = 45, kPinB = 36, kPinC = 48, kPinD = 35;
constexpr int8_t kPinE = 21;  // Address E, via the board's solder jumper; only
                              // needed for 64x64 (1/32 scan) panels.
constexpr int8_t kPinLat = 47, kPinOe = 14, kPinClk = 2;

// --- Panel geometry -----------------------------------------------------------
// Default: one 64x32 panel (the common 1/16-scan type).
//   - Chained panels: bump kChainPanels (panels wired left-to-right).
//   - 64x64 panels (1/32 scan): set kPanelHeight = 64 and keep kPinE = 21.
constexpr uint16_t kPanelWidth = 64;
constexpr uint16_t kPanelHeight = 32;
constexpr uint16_t kChainPanels = 1;
constexpr uint16_t kDisplayWidth = kPanelWidth * kChainPanels;
constexpr uint16_t kDisplayHeight = kPanelHeight;

Hub75Config MakeConfig() {
  Hub75Config config{};
  config.panel_width = kPanelWidth;
  config.panel_height = kPanelHeight;
  config.layout_cols = kChainPanels;
  config.layout = Hub75PanelLayout::HORIZONTAL;
  config.scan_wiring = Hub75ScanWiring::STANDARD_TWO_SCAN;
  config.shift_driver = Hub75ShiftDriver::GENERIC;  // try FM6126A if black screen
  config.double_buffer = true;                      // tear-free updates
  config.min_refresh_rate = 90;

  config.pins.r1 = kPinR1;
  config.pins.g1 = kPinG1;
  config.pins.b1 = kPinB1;
  config.pins.r2 = kPinR2;
  config.pins.g2 = kPinG2;
  config.pins.b2 = kPinB2;
  config.pins.a = kPinA;
  config.pins.b = kPinB;
  config.pins.c = kPinC;
  config.pins.d = kPinD;
  config.pins.e = kPinE;
  config.pins.lat = kPinLat;
  config.pins.oe = kPinOe;
  config.pins.clk = kPinClk;
  return config;
}
}  // namespace

extern "C" void app_main(void) {
  ESP_LOGI(kTag, "Matrix Portal S3 booting, panel %dx%d", kDisplayWidth, kDisplayHeight);

  static Hub75Driver display(MakeConfig());
  if (!display.begin()) {
    ESP_LOGE(kTag, "HUB75 init failed");
    return;
  }

  const uint16_t w = display.get_width();
  const uint16_t h = display.get_height();

  struct Color {
    uint8_t r, g, b;
  };
  constexpr Color kCycle[] = {{255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {255, 255, 255}};

  int step = 0;
  while (true) {
    // Solid color field, cycling R/G/B/white.
    const Color &c = kCycle[(step / 60) % 4];
    display.clear();
    display.fill(0, 0, w, h, c.r, c.g, c.b);

    // White pixel bouncing around the border to show addressing.
    const int perim = 2 * (w + h) - 4;
    const int p = step % perim;
    int x, y;
    if (p < w) {
      x = p; y = 0;
    } else if (p < w + h - 1) {
      x = w - 1; y = p - w + 1;
    } else if (p < 2 * w + h - 2) {
      x = w - 1 - (p - w - h + 1); y = h - 1;
    } else {
      x = 0; y = h - 1 - (p - 2 * w - h + 2);
    }
    display.set_pixel(x, y, 255, 255, 255);

    display.flip_buffer();
    ++step;
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}
