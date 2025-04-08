#ifndef ENV_H
#define ENV_H

// WiFi credentials
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"

// Bitcoin API endpoint
#define BTC_API_URL "https://api.bitstamp.net/v2/ticker/btcusd/"

// CoinDesk API key
#define COINDESK_API_KEY "YOUR_COINDESK_API_KEY"
#define COINDESK_API_URL_30H "https://data-api.coindesk.com/index/cc/v1/historical/hours?market=cadli&instrument=BTC-USD&limit=30&aggregate=1&fill=true&apply_mapping=true&response_format=JSON&api_key=YOUR_COINDESK_API_KEY"
#define COINDESK_API_URL_30D "https://data-api.coindesk.com/index/cc/v1/historical/days?market=cadli&instrument=BTC-USD&limit=30&aggregate=1&fill=true&apply_mapping=true&response_format=JSON&api_key=YOUR_COINDESK_API_KEY"
#define COINDESK_API_URL_30M "https://data-api.coindesk.com/index/cc/v1/historical/minutes?market=cadli&instrument=BTC-USD&limit=30&aggregate=1&fill=true&apply_mapping=true&response_format=JSON&api_key=YOUR_COINDESK_API_KEY"

// YouTube API credentials
const char* env_key = "YOUR_YOUTUBE_API_KEY";
const char* youtube_channel_id = "YOUR_YOUTUBE_CHANNEL_ID";

#endif // ENV_H 