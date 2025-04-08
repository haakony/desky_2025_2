# Bitcoin Price Tracker for ESP32-S3 AMOLED Display

A real-time Bitcoin price tracker that displays price data on a LILYGO T-Display-S3 AMOLED screen. The device fetches Bitcoin price information from the CoinDesk API and presents it in an easy-to-read graph format.

![Bitcoin Price Tracker](https://imgaz.staticbg.com/images/oaupload/banggood/images/6C/81/ce4120bc-1df4-424e-a377-4cd4e02023ce.jpg.webp)

For a detailed blog post about this project, visit: [Bitcoin Price Tracker Blog Post](https://haakony.no/posts/desky_2025/)

## Features

- Real-time Bitcoin price updates (every 5 minutes)
- Interactive time intervals (30m, 30h, 30d)
- Visual price graph with auto-scaling
- High, low, and current price display
- Percentage change with color coding
- Clean, organized user interface

## Hardware Requirements

- [LILYGO T-Display-S3 AMOLED](https://www.banggood.com/custlink/3KGS4PpAQM) development board
- USB cable for power and programming
- WiFi connection

## Software Requirements

- Arduino IDE (2.0 or newer)
- ESP32 board support package
- Required libraries:
  - TFT_eSPI
  - ArduinoJson
  - HTTPClient (included with ESP32 core)

## Installation

1. Clone this repository:
   ```
   git clone https://github.com/haakony/desky2025_2.git
   ```

2. Open the project in Arduino IDE

3. Install required libraries:
   - TFT_eSPI
   - ArduinoJson

4. Configure the TFT_eSPI library for the LILYGO T-Display-S3:
   - Open the `User_Setup.h` file in the TFT_eSPI library
   - Set the display driver to `RM67162`
   - Configure the pins according to the LILYGO T-Display-S3 specifications

5. Create an `env.h` file based on the provided `env_template.h`:
   - Add your WiFi credentials
   - Add your CoinDesk API key

6. Compile and upload the code to your ESP32-S3

## Usage

1. Power on the device and connect it to your WiFi network
2. The display will show the Bitcoin price graph with the default 30-hour view
3. Use Button 1 to cycle through different time intervals (30m → 30h → 30d)
4. Use Button 2 to manually refresh the data

## Configuration

You can modify the following parameters in the code:

- `UPDATE_INTERVAL`: Change how often the price data is updated (default: 5 minutes)
- `GRAPH_WIDTH`, `GRAPH_HEIGHT`: Adjust the size of the graph
- `MAX_DATA_POINTS`: Change the number of data points displayed

## Troubleshooting

- **Display not working**: Check the TFT_eSPI library configuration
- **WiFi connection issues**: Verify your WiFi credentials in `env.h`
- **API errors**: Check your CoinDesk API key and internet connection
- **Button not responding**: Verify the button pin configuration

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- [LILYGO](https://www.banggood.com/custlink/3KGS4PpAQM) for the T-Display-S3 AMOLED board
- [CoinDesk](https://www.coindesk.com/) for providing the Bitcoin price API
- [Bodmer](https://github.com/Bodmer) for the TFT_eSPI library

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request. 