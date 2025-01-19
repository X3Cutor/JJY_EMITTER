#include "esp_pm.h"
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClient.h>

#include "config.h"

#define JJY_40k_OUTPUT_PIN 21 // Pin that outputs 40kHz code (-1 = not used)
#define JJY_60k_OUTPUT_PIN -1 // Pin that outputs 60kHz code (-1 = not used)
#define JJY_CODE_NONINVERTED_OUTPUT_PIN -1  // Pin that outputs unmodulated timecode in positive logic (-1 = not used)
#define JJY_CODE_INVERTED_OUTPUT_PIN -1 // Pin that outputs unmodulated timecode in negative logic (-1 = not used)

#define LEDC_40k_CHANNEL 0	   // LEDC 40kHz channel
#define LEDC_60k_CHANNEL 10	   // LEDC 60kHz channel
#define LEDC_RESOLUTION_BITS 1 // LEDC Resolution

// Class to create timecode
class jjy_timecode_generator_t
{
public:
	unsigned char year10;
	unsigned char year1;
	unsigned char yday100;
	unsigned char yday10;
	unsigned char yday1;
	unsigned short yday;
	unsigned char hour10;
	unsigned char hour1;
	unsigned char min10;
	unsigned char min1;
	unsigned char wday;
	unsigned char mon;
	unsigned char day;
	bool valid = false;

	unsigned char result[61];

public:
	void generate()
	{
		unsigned char pa1 = 0;
		unsigned char pa2 = 0;
		for (int sec = 0; sec < 60; sec++)
		{
			unsigned char cc;

			cc = 0;
#define SET(X) \
	if (X)     \
	cc |= 1
			switch (sec)
			{
			case 0: /* marker */
				cc = 2;
				break;

			case 1:
				SET(min10 & 4);
				pa2 ^= cc;
				break;

			case 2:
				SET(min10 & 2);
				pa2 ^= cc;
				break;

			case 3:
				SET(min10 & 1);
				pa2 ^= cc;
				break;

			case 4:
				cc = 0;
				break;

			case 5:
				SET(min1 & 8);
				pa2 ^= cc;
				break;

			case 6:
				SET(min1 & 4);
				pa2 ^= cc;
				break;

			case 7:
				SET(min1 & 2);
				pa2 ^= cc;
				break;

			case 8:
				SET(min1 & 1);
				pa2 ^= cc;
				break;

			case 9:
				cc = 2;
				break;

			case 10:
				cc = 0;
				break;

			case 11:
				cc = 0;
				break;

			case 12:
				SET(hour10 & 2);
				pa1 ^= cc;
				break;

			case 13:
				SET(hour10 & 1);
				pa1 ^= cc;
				break;

			case 14:
				cc = 0;
				break;

			case 15:
				SET(hour1 & 8);
				pa1 ^= cc;
				break;

			case 16:
				SET(hour1 & 4);
				pa1 ^= cc;
				break;

			case 17:
				SET(hour1 & 2);
				pa1 ^= cc;
				break;

			case 18:
				SET(hour1 & 1);
				pa1 ^= cc;
				break;

			case 19:
				cc = 2;
				break;

			case 20:
				cc = 0;
				break;

			case 21:
				cc = 0;
				break;

			case 22:
				SET(yday100 & 2);
				break;

			case 23:
				SET(yday100 & 1);
				break;

			case 24:
				cc = 0;
				break;

			case 25:
				SET(yday10 & 8);
				break;

			case 26:
				SET(yday10 & 4);
				break;

			case 27:
				SET(yday10 & 2);
				break;

			case 28:
				SET(yday10 & 1);
				break;

			case 29:
				cc = 2;
				break;

			case 30:
				SET(yday1 & 8);
				break;

			case 31:
				SET(yday1 & 4);
				break;

			case 32:
				SET(yday1 & 2);
				break;

			case 33:
				SET(yday1 & 1);
				break;

			case 34:
				cc = 0;
				break;

			case 35:
				cc = 0;
				break;

			case 36:
				cc = pa1;
				break;

			case 37:
				cc = pa2;
				break;

			case 38:
				cc = 0; /* SU1 */
				break;

			case 39:
				cc = 2;
				break;

			case 40:
				cc = 0; /* SU2 */
				break;

			case 41:
				SET(year10 & 8);
				break;

			case 42:
				SET(year10 & 4);
				break;

			case 43:
				SET(year10 & 2);
				break;

			case 44:
				SET(year10 & 1);
				break;

			case 45:
				SET(year1 & 8);
				break;

			case 46:
				SET(year1 & 4);
				break;

			case 47:
				SET(year1 & 2);
				break;

			case 48:
				SET(year1 & 1);
				break;

			case 49:
				cc = 2;
				break;

			case 50:
				SET(wday & 4);
				break;

			case 51:
				SET(wday & 2);
				break;

			case 52:
				SET(wday & 1);
				break;

			case 53:
				cc = 0;
				break;

			case 54:
				cc = 0;
				break;

			case 55:
				cc = 0;
				break;

			case 56:
				cc = 0;
				break;

			case 57:
				cc = 0;
				break;

			case 58:
				cc = 0;
				break;

			case 59:
				cc = 2;
				break;
			}
			result[sec] = cc;
		}
		result[60] = 2;
		valid = true;
	}
} gen;

static void start_wifi()
{

	WiFi.mode(WIFI_OFF);
	WiFi.setAutoReconnect(true);
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);

	Serial.println();
	Serial.println();
	Serial.print("Wait for WiFi... ");
	while (!WiFi.isConnected())
	{
		Serial.printf(".");
		delay(1000);
	}
	configTzTime(tz, ntp[0], ntp[1], ntp[2]);

}

void setup()
{
	Serial.begin(115200);

	start_wifi();

	if (JJY_60k_OUTPUT_PIN != -1)
	{
		ledcSetup(LEDC_60k_CHANNEL, 60000.0, LEDC_RESOLUTION_BITS);
		ledcAttachPin(JJY_60k_OUTPUT_PIN, LEDC_60k_CHANNEL);
	}

	if (JJY_40k_OUTPUT_PIN != -1)
	{
		ledcSetup(LEDC_40k_CHANNEL, 40000.0, LEDC_RESOLUTION_BITS);
		ledcAttachPin(JJY_40k_OUTPUT_PIN, LEDC_40k_CHANNEL);
	}

	if (JJY_CODE_NONINVERTED_OUTPUT_PIN != -1)
		pinMode(JJY_CODE_NONINVERTED_OUTPUT_PIN, OUTPUT);

	if (JJY_CODE_INVERTED_OUTPUT_PIN != -1)
		pinMode(JJY_CODE_INVERTED_OUTPUT_PIN, OUTPUT);
}

void loop()
{
	static uint32_t min_origin_tick;
	static int last_min;
	static bool last_on_state;
	time_t t;
	t = time(&t);
	struct tm *tm = localtime(&t);
	bool date_valid = true;
	if(tm->tm_year + 1900 < 2000) date_valid = false; // Probably NTP has not been acquired yet.
	if (last_min != tm->tm_min && tm->tm_sec == 0)
	{
		// The change of minutes. Creates a one-minute time code.
		gen.year10 = (tm->tm_year / 10) % 10;
		gen.year1 = tm->tm_year % 10;
		gen.yday100 = ((tm->tm_yday + 1) / 100) % 10;
		gen.yday10 = ((tm->tm_yday + 1) / 10) % 10;
		gen.yday1 = (tm->tm_yday + 1) % 10;
		gen.yday = (tm->tm_yday + 1);
		gen.hour10 = (tm->tm_hour / 10) % 10;
		gen.hour1 = tm->tm_hour % 10;
		gen.min10 = (tm->tm_min / 10) % 10;
		gen.min1 = tm->tm_min % 10;
		gen.wday = tm->tm_wday;
		gen.mon = tm->tm_mon;
		gen.day = tm->tm_mday;
		gen.generate();
		min_origin_tick = millis();

		Serial.printf("%d%d/%d/%d (%d%d%d) %d%d:%d%d\r\n",
					  gen.year10,
					  gen.year1,
					  gen.mon + 1,
					  gen.day,
					  gen.yday100,
					  gen.yday10,
					  gen.yday1,
					  gen.hour10,
					  gen.hour1,
					  gen.min10,
					  gen.min1);
	}
	last_min = tm->tm_min;

	// A job every second
	uint32_t sub_min = millis() - min_origin_tick;
	int sec = sub_min / 1000;
	int sub_sec = sub_min % 1000;
	bool on = false;
	if (sec < 60) // Sometimes sec is 60, but ignore it for now.
	{
		switch (gen.result[sec])
		{
		case 0:
			if (sub_sec < 800)
				on = true; // "0" コード
			break;
		case 1:
			if (sub_sec < 500)
				on = true; // "1" コード
			break;
		case 2:
			if (sub_sec < 200)
				on = true; // マーカー
			break;
		default:
			break;
		}
		if (on != last_on_state)
		{
			last_on_state = on;
			if(date_valid && gen.valid)
			{
				// Output only if the time is obtained correctly.
				if (on)
				{
					if (JJY_60k_OUTPUT_PIN != -1)
						ledcWrite(LEDC_60k_CHANNEL, (1 << LEDC_RESOLUTION_BITS) / 2); // Duty ratio = 50%
					if (JJY_40k_OUTPUT_PIN != -1)
						ledcWrite(LEDC_40k_CHANNEL, (1 << LEDC_RESOLUTION_BITS) / 2); // Duty ratio = 50%
					if (JJY_CODE_NONINVERTED_OUTPUT_PIN != -1)
						digitalWrite(JJY_CODE_NONINVERTED_OUTPUT_PIN, 1);
					if (JJY_CODE_INVERTED_OUTPUT_PIN != -1)
						digitalWrite(JJY_CODE_INVERTED_OUTPUT_PIN, 0);
				}
				else
				{
					if (JJY_60k_OUTPUT_PIN != -1)
						ledcWrite(LEDC_60k_CHANNEL, 0); // Duty ratio 0 = OFF
					if (JJY_40k_OUTPUT_PIN != -1)
						ledcWrite(LEDC_40k_CHANNEL, 0); // Duty ratio 0 = OFF
					if (JJY_CODE_NONINVERTED_OUTPUT_PIN != -1)
						digitalWrite(JJY_CODE_NONINVERTED_OUTPUT_PIN, 0);
					if (JJY_CODE_INVERTED_OUTPUT_PIN != -1)
						digitalWrite(JJY_CODE_INVERTED_OUTPUT_PIN, 1);
				}
			}
		}
	}
	vTaskDelay(5); // Sleep for the appropriate number of ticks
}
