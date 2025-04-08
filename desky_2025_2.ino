#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "rm67162.h"
#include <TFT_eSPI.h>
#include "env.h"

// Define which mode to run (0 = original, 1 = Bitcoin price graph)
#define RUN_MODE 1

// Button pins
#define BUTTON1_PIN 0  // Adjust these pin numbers based on your hardware
#define BUTTON2_PIN 14 // Adjust these pin numbers based on your hardware

// Time interval options
#define INTERVAL_30M 0
#define INTERVAL_30H 1
#define INTERVAL_30D 2

// Initialize display
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

// Display dimensions
#define DISPLAY_WIDTH 536
#define DISPLAY_HEIGHT 240

// Constants for graph
const int GRAPH_WIDTH = 500;
const int GRAPH_HEIGHT = 160;
const int GRAPH_X = 18;
const int GRAPH_Y = 20;
const int MAX_DATA_POINTS = 30;

// Data structures
struct PriceData {
    unsigned long timestamp;
    float price;
};

PriceData priceHistory[MAX_DATA_POINTS];
int dataCount = 0;
int currentInterval = INTERVAL_30H; // Default to 30h interval

// Colors
#define GRAPH_COLOR TFT_GREEN
#define TEXT_COLOR TFT_WHITE
#define GRID_COLOR 0x4208
#define BACKGROUND_COLOR TFT_BLACK

// Function declarations for Bitcoin price graph
void btcSetup();
void btcLoop();
void handleButtons();
void switchInterval(int newInterval);
void fetchBitcoinData();

bool initializeDisplay() {
    Serial.println("Initializing display...");
    
    // Initialize the display driver
    rm67162_init();
    
    // Set display parameters
    lcd_setRotation(1);
    
    // Create sprite with error checking - ensure dimensions match display
    if (!sprite.createSprite(DISPLAY_WIDTH, DISPLAY_HEIGHT)) {
        Serial.println("Failed to create sprite!");
        return false;
    }
    
    // Configure sprite
    sprite.setSwapBytes(1);
    sprite.fillSprite(BACKGROUND_COLOR);
    
    // Initialize buttons
    pinMode(BUTTON1_PIN, INPUT_PULLUP);
    pinMode(BUTTON2_PIN, INPUT_PULLUP);
    
    Serial.println("Display initialized successfully");
    return true;
}

void setup() {
    Serial.begin(115200);
    Serial.println("Starting Desky 2025...");
    
    // Initialize display with error checking
    if (!initializeDisplay()) {
        Serial.println("Display initialization failed! Rebooting...");
        delay(1000);
        ESP.restart();
    }
    
    // Connect to WiFi
    Serial.println("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    
    int wifiAttempts = 0;
    while (WiFi.status() != WL_CONNECTED && wifiAttempts < 20) {
        delay(500);
        Serial.print(".");
        wifiAttempts++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConnected to WiFi");
        // Initial data fetch
        fetchBitcoinData();
    } else {
        Serial.println("\nFailed to connect to WiFi");
    }
}

void loop() {
    if (RUN_MODE == 0) {
        // Original functionality
        static unsigned long lastUpdate = 0;
        const unsigned long UPDATE_INTERVAL = 300000; // 5 minutes
        
        if (WiFi.status() == WL_CONNECTED) {
            if (millis() - lastUpdate >= UPDATE_INTERVAL) {
                fetchBitcoinData();
                lastUpdate = millis();
            }
        } else {
            // Try to reconnect to WiFi
            Serial.println("WiFi connection lost. Attempting to reconnect...");
            WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
            delay(5000);
        }
        
        drawGraph();
        delay(1000);
    } else {
        // Bitcoin price graph functionality
        btcLoop();
    }
}

void btcSetup() {
    // This function is called from setup() if RUN_MODE is 1
    Serial.println("Setting up Bitcoin price graph...");
    
    // Initial data fetch
    fetchBitcoinData();
}

void btcLoop() {
    static unsigned long lastUpdate = 0;
    const unsigned long UPDATE_INTERVAL = 300000; // 5 minutes
    
    // Check for button presses
    handleButtons();
    
    if (WiFi.status() == WL_CONNECTED) {
        if (millis() - lastUpdate >= UPDATE_INTERVAL) {
            fetchBitcoinData();
            lastUpdate = millis();
        }
    } else {
        // Try to reconnect to WiFi
        Serial.println("WiFi connection lost. Attempting to reconnect...");
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        delay(5000);
    }
    
    // Draw the graph and update the display
    drawGraph();
    
    // Add a small delay to prevent overwhelming the display
    delay(100);
}

void handleButtons() {
    static bool button1Pressed = false;
    static bool button2Pressed = false;
    
    // Check Button 1 (cycle through intervals)
    if (digitalRead(BUTTON1_PIN) == LOW) {
        if (!button1Pressed) {
            button1Pressed = true;
            // Cycle to next interval
            int nextInterval = (currentInterval + 1) % 3;
            switchInterval(nextInterval);
            delay(300); // Debounce
        }
    } else {
        button1Pressed = false;
    }
    
    // Check Button 2 (refresh data)
    if (digitalRead(BUTTON2_PIN) == LOW) {
        if (!button2Pressed) {
            button2Pressed = true;
            // Refresh data
            fetchBitcoinData();
            delay(300); // Debounce
        }
    } else {
        button2Pressed = false;
    }
}

void switchInterval(int newInterval) {
    if (newInterval != currentInterval) {
        currentInterval = newInterval;
        Serial.print("Switching to interval: ");
        switch (currentInterval) {
            case INTERVAL_30M:
                Serial.println("30 minutes");
                break;
            case INTERVAL_30H:
                Serial.println("30 hours");
                break;
            case INTERVAL_30D:
                Serial.println("30 days");
                break;
        }
        
        // Clear the display before fetching new data
        sprite.fillSprite(BACKGROUND_COLOR);
        lcd_PushColors(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, (uint16_t *)sprite.getPointer());
        
        // Fetch new data immediately
        fetchBitcoinData();
    }
}

void fetchBitcoinData() {
    if (WiFi.status() == WL_CONNECTED) {
        // Clear the display before fetching new data
        sprite.fillSprite(BACKGROUND_COLOR);
        lcd_PushColors(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, (uint16_t *)sprite.getPointer());
        
        HTTPClient http;
        String url;
        
        // Select the appropriate URL based on the current interval
        switch (currentInterval) {
            case INTERVAL_30M:
                url = COINDESK_API_URL_30M;
                break;
            case INTERVAL_30H:
                url = COINDESK_API_URL_30H;
                break;
            case INTERVAL_30D:
                url = COINDESK_API_URL_30D;
                break;
            default:
                url = COINDESK_API_URL_30H;
                break;
        }
        
        http.begin(url);
        int httpCode = http.GET();
        
        if (httpCode == HTTP_CODE_OK) {
            String payload = http.getString();
            parseBitcoinData(payload);
        } else {
            Serial.printf("HTTP request failed, error: %s\n", http.errorToString(httpCode).c_str());
        }
        
        http.end();
    }
}

void parseBitcoinData(String jsonString) {
    StaticJsonDocument<16384> doc;
    DeserializationError error = deserializeJson(doc, jsonString);
    
    if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        return;
    }
    
    JsonArray data = doc["Data"];
    dataCount = min((int)data.size(), MAX_DATA_POINTS);
    
    for (int i = 0; i < dataCount; i++) {
        priceHistory[i].timestamp = data[i]["TIMESTAMP"];
        priceHistory[i].price = data[i]["CLOSE"];
    }
}

void drawGraph() {
    // Clear the entire sprite with background color
    sprite.fillSprite(BACKGROUND_COLOR);
    
    // Draw title in top left
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(2);
    sprite.drawString("BTC 30h", 20, 10, 2);
    
    // Draw description in top right with current interval
    sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
    sprite.setTextSize(2);
    String intervalText;
    switch (currentInterval) {
        case INTERVAL_30M:
            intervalText = "BTC/USD 30m";
            break;
        case INTERVAL_30H:
            intervalText = "BTC/USD 30h";
            break;
        case INTERVAL_30D:
            intervalText = "BTC/USD 30d";
            break;
        default:
            intervalText = "BTC/USD 30h";
            break;
    }
    
    // Calculate text width to position it properly
    int16_t textWidth = sprite.textWidth(intervalText);
    int16_t textX = DISPLAY_WIDTH - textWidth - 40; // Position 20 pixels from the right edge
    
    sprite.drawString(intervalText, textX, 10, 2);
    
    // Draw grid
    drawGrid();
    
    // Draw price line
    if (dataCount > 1) {
        float minPrice = priceHistory[0].price;
        float maxPrice = priceHistory[0].price;
        
        // Find min and max prices
        for (int i = 1; i < dataCount; i++) {
            minPrice = min(minPrice, priceHistory[i].price);
            maxPrice = max(maxPrice, priceHistory[i].price);
        }
        
        // Add 5% padding
        float priceRange = maxPrice - minPrice;
        minPrice -= priceRange * 0.05;
        maxPrice += priceRange * 0.05;
        
        // Draw price line
        for (int i = 0; i < dataCount - 1; i++) {
            int x1 = GRAPH_X + (i * GRAPH_WIDTH / (dataCount - 1));
            int y1 = GRAPH_Y + GRAPH_HEIGHT - ((priceHistory[i].price - minPrice) * GRAPH_HEIGHT / (maxPrice - minPrice));
            int x2 = GRAPH_X + ((i + 1) * GRAPH_WIDTH / (dataCount - 1));
            int y2 = GRAPH_Y + GRAPH_HEIGHT - ((priceHistory[i + 1].price - minPrice) * GRAPH_HEIGHT / (maxPrice - minPrice));
            
            sprite.drawLine(x1, y1, x2, y2, GRAPH_COLOR);
        }
        
        // Calculate percentage change
        float firstPrice = priceHistory[0].price;
        float lastPrice = priceHistory[dataCount-1].price;
        float percentChange = ((lastPrice - firstPrice) / firstPrice) * 100;
        
        // Draw price information
        sprite.setTextSize(2);
        
        // High price at top left
        char highPriceStr[20];
        sprintf(highPriceStr, "H: $%.2f", maxPrice);
        sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
        sprite.drawString(highPriceStr, 20, GRAPH_Y - 15, 2);
        
        // Low price at bottom left
        char lowPriceStr[20];
        sprintf(lowPriceStr, "L: $%.2f", minPrice);
        sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
        sprite.drawString(lowPriceStr, 20, GRAPH_Y + GRAPH_HEIGHT + 10, 2);
        
        // Current price in the middle
        char currentPriceStr[20];
        sprintf(currentPriceStr, "BTC$%.2f ", lastPrice);
        sprite.setTextColor(TEXT_COLOR, BACKGROUND_COLOR);
        sprite.drawString(currentPriceStr, DISPLAY_WIDTH/2 - 50, GRAPH_Y + GRAPH_HEIGHT + 10, 2);
        
        // Percentage change with color based on direction - moved to far right
        char percentStr[20];
        sprintf(percentStr, "%.2f%%", percentChange);
        if (percentChange >= 0) {
            sprite.setTextColor(TFT_GREEN, BACKGROUND_COLOR);
        } else {
            sprite.setTextColor(TFT_RED, BACKGROUND_COLOR);
        }
        sprite.drawString(percentStr, DISPLAY_WIDTH - 100, GRAPH_Y + GRAPH_HEIGHT + 10, 2);
    }
    
    // Push the entire sprite to the display
    lcd_PushColors(0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, (uint16_t *)sprite.getPointer());
}

void drawGrid() {
    // Draw horizontal grid lines
    for (int i = 0; i <= 4; i++) {
        int y = GRAPH_Y + (i * GRAPH_HEIGHT / 4);
        sprite.drawLine(GRAPH_X, y, GRAPH_X + GRAPH_WIDTH, y, GRID_COLOR);
    }
    
    // Draw vertical grid lines
    for (int i = 0; i <= 6; i++) {
        int x = GRAPH_X + (i * GRAPH_WIDTH / 6);
        sprite.drawLine(x, GRAPH_Y, x, GRAPH_Y + GRAPH_HEIGHT, GRID_COLOR);
    }
} 