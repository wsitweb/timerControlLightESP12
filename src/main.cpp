#if defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <WiFiClient.h>
  #include <ESP8266WebServer.h>
#elif defined(ESP32)
  #include <WiFi.h>
  #include <WiFiClient.h>
  #include <WebServer.h>
#elif defined(TARGET_RP2040)
  #include <WiFi.h>
  #include <WebServer.h>
#endif

#include <ElegantOTA.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <DNSServer.h>

const char* ssid = "timer";
const char* password = "12345678";
int RelayPin = 14;
// int ledPin = 14; 

int timeoutA = 1; //Время работы
int timeoutB = 1; //Время не работы

int StatusRelay = 0;
int StatusLoop = 0;
boolean relayState = HIGH;
unsigned long previousMillis = 0;
int valueBrightness = 0;

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 100);
DNSServer dnsServer;

#if defined(ESP8266)
  ESP8266WebServer server(80);
#elif defined(ESP32)
  WebServer server(80);
#elif defined(TARGET_RP2040)
  WebServer server(80);
#endif

unsigned long ota_progress_millis = 0;

void onOTAStart() {
  // Log when OTA has started
  Serial.println("OTA update started!");
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println("OTA update finished successfully!");
  } else {
    Serial.println("There was an error during OTA update!");
  }
  // <Add your own code here>
}

void handleRoot() {
    // String html = "<!DOCTYPE html><html><head><title>ElegantOTA Demo</title></head><body><h1>Hello! This is ElegantOTA Demo.</h1><!-- Your additional HTML content goes here --></body></html>";
    const char* html = R"(

<!DOCTYPE html>
<html lang='uk-Ua'>
<head>
    <link rel='shortcut icon' href='data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iNjQiIGhlaWdodD0iNjUiIHZpZXdCb3g9IjAgMCA2NCA2NSIgZmlsbD0ibm9uZSIgeG1sbnM9Imh0dHA6Ly93d3cudzMub3JnLzIwMDAvc3ZnIj4KPHJlY3QgeT0iMSIgd2lkdGg9IjY0IiBoZWlnaHQ9IjY0IiByeD0iMzIiIGZpbGw9IiMwOTBCMDgiLz4KPGcgZmlsdGVyPSJ1cmwoI2ZpbHRlcjBfZl84NzJfODg2KSI+CjxwYXRoIGZpbGwtcnVsZT0iZXZlbm9kZCIgY2xpcC1ydWxlPSJldmVub2RkIiBkPSJNNDIuMjMzNCA0Mi44Mjk2QzQwLjY3ODIgNDEuODQ1MyA0MC4zNzUyIDM5LjA1MDMgNDEuNTI1MiAzNy42MTMzQzQzLjE5NTEgMzUuNTI2OSA0NC4xOTM1IDMyLjg4MDEgNDQuMTkzNSAzMEM0NC4xOTM1IDIzLjI2NTcgMzguNzM0MyAxNy44MDY1IDMyIDE3LjgwNjVDMjUuMjY1NyAxNy44MDY1IDE5LjgwNjUgMjMuMjY1NyAxOS44MDY1IDMwQzE5LjgwNjUgMzIuODgwMSAyMC44MDUgMzUuNTI2OSAyMi40NzQ3IDM3LjYxMzNDMjMuNjI0OCAzOS4wNTAzIDIzLjMyMTggNDEuODQ1MyAyMS43NjY1IDQyLjgyOTZDMjAuOTQ0MiA0My4zNTAxIDE5Ljg3MSA0My4zMzU0IDE5LjE4NzQgNDIuNjQyNkMxNS45Nzk4IDM5LjM5MjMgMTQgMzQuOTI3NCAxNCAzMEMxNCAyMC4wNTg5IDIyLjA1ODkgMTIgMzIgMTJDNDEuOTQxMSAxMiA1MCAyMC4wNTg5IDUwIDMwQzUwIDM0LjkyNzQgNDguMDIwMSAzOS4zOTIzIDQ0LjgxMjYgNDIuNjQyNkM0NC4xMjkgNDMuMzM1NCA0My4wNTU4IDQzLjM1MDEgNDIuMjMzNCA0Mi44Mjk2WiIgZmlsbD0id2hpdGUiLz4KPC9nPgo8cGF0aCBmaWxsLXJ1bGU9ImV2ZW5vZGQiIGNsaXAtcnVsZT0iZXZlbm9kZCIgZD0iTTQyLjIzMzQgNDIuODI5NkM0MC42NzgyIDQxLjg0NTMgNDAuMzc1MiAzOS4wNTAzIDQxLjUyNTIgMzcuNjEzM0M0My4xOTUxIDM1LjUyNjkgNDQuMTkzNSAzMi44ODAxIDQ0LjE5MzUgMzBDNDQuMTkzNSAyMy4yNjU3IDM4LjczNDMgMTcuODA2NSAzMiAxNy44MDY1QzI1LjI2NTcgMTcuODA2NSAxOS44MDY1IDIzLjI2NTcgMTkuODA2NSAzMEMxOS44MDY1IDMyLjg4MDEgMjAuODA1IDM1LjUyNjkgMjIuNDc0NyAzNy42MTMzQzIzLjYyNDggMzkuMDUwMyAyMy4zMjE4IDQxLjg0NTMgMjEuNzY2NSA0Mi44Mjk2QzIwLjk0NDIgNDMuMzUwMSAxOS44NzEgNDMuMzM1NCAxOS4xODc0IDQyLjY0MjZDMTUuOTc5OCAzOS4zOTIzIDE0IDM0LjkyNzQgMTQgMzBDMTQgMjAuMDU4OSAyMi4wNTg5IDEyIDMyIDEyQzQxLjk0MTEgMTIgNTAgMjAuMDU4OSA1MCAzMEM1MCAzNC45Mjc0IDQ4LjAyMDEgMzkuMzkyMyA0NC44MTI2IDQyLjY0MjZDNDQuMTI5IDQzLjMzNTQgNDMuMDU1OCA0My4zNTAxIDQyLjIzMzQgNDIuODI5NloiIGZpbGw9IndoaXRlIi8+CjxwYXRoIGZpbGwtcnVsZT0iZXZlbm9kZCIgY2xpcC1ydWxlPSJldmVub2RkIiBkPSJNMzAuMjE2IDQ5Ljg2MjhDMjguOTUzNCA0OS43MzkzIDI3LjkzMTIgNTEuODg3MiAyOC44MjcgNTIuOTUzNEMyOS4wMTY5IDUzLjE3OTUgMjkuMjY1MiA1My4zMjQxIDI5LjUzMjYgNTMuMzU2MUMzMC4zMjc2IDUzLjQ1MTMgMzEuMTM0MSA1My41IDMxLjk1MDIgNTMuNUMzMi44MjA4IDUzLjUgMzMuNjgwNSA1My40NDQ1IDM0LjUyNyA1My4zMzY0QzM0Ljc4OTMgNTMuMzAyOSAzNS4wMzI3IDUzLjE2IDM1LjIxOTQgNTIuOTM3OUMzNi4xMTI2IDUxLjg3NDcgMzUuMDc2MiA0OS43MTY2IDMzLjgxNzkgNDkuODQ5MkMzMy4yMDI2IDQ5LjkxNDEgMzIuNTc5NiA0OS45NDcyIDMxLjk1MDIgNDkuOTQ3MkMzMS4zNjYyIDQ5Ljk0NzIgMzAuNzg3OCA0OS45MTg3IDMwLjIxNiA0OS44NjI4Wk0yNS40NDk0IDQzLjU4N0MyNC42OTE3IDQ0LjQ4ODggMjQuOTE3OSA0Ni4wMDk1IDI1LjkzMzggNDYuNDE1MkMyNy44MjM2IDQ3LjE2OTggMjkuODQ2OCA0Ny41Nzg3IDMxLjk1MDIgNDcuNTc4N0MzNC4wOTM2IDQ3LjU3ODcgMzYuMTUzNiA0Ny4xNTQxIDM4LjA3NDIgNDYuMzcxOEMzOS4wODQgNDUuOTYwNCAzOS4zMDU3IDQ0LjQ0NiAzOC41NTA5IDQzLjU0NzdDMzguMTE0MSA0My4wMjc4IDM3LjQ2MTQgNDIuODc1IDM2Ljg3MzkgNDMuMTAyMkMzNS4zMjM1IDQzLjcwMTkgMzMuNjY4NiA0NC4wMjYgMzEuOTUwMiA0NC4wMjZDMzAuMjY1NSA0NC4wMjYgMjguNjQxOSA0My43MTQ1IDI3LjExNzkgNDMuMTM3MkMyNi41MzIzIDQyLjkxNTQgMjUuODgzOSA0My4wNjk4IDI1LjQ0OTQgNDMuNTg3WiIgZmlsbD0iIzY0NjQ2NCIvPgo8ZGVmcz4KPGZpbHRlciBpZD0iZmlsdGVyMF9mXzg3Ml84ODYiIHg9IjIiIHk9IjAiIHdpZHRoPSI2MCIgaGVpZ2h0PSI1NS4xOTM0IiBmaWx0ZXJVbml0cz0idXNlclNwYWNlT25Vc2UiIGNvbG9yLWludGVycG9sYXRpb24tZmlsdGVycz0ic1JHQiI+CjxmZUZsb29kIGZsb29kLW9wYWNpdHk9IjAiIHJlc3VsdD0iQmFja2dyb3VuZEltYWdlRml4Ii8+CjxmZUJsZW5kIG1vZGU9Im5vcm1hbCIgaW49IlNvdXJjZUdyYXBoaWMiIGluMj0iQmFja2dyb3VuZEltYWdlRml4IiByZXN1bHQ9InNoYXBlIi8+CjxmZUdhdXNzaWFuQmx1ciBzdGREZXZpYXRpb249IjYiIHJlc3VsdD0iZWZmZWN0MV9mb3JlZ3JvdW5kQmx1cl84NzJfODg2Ii8+CjwvZmlsdGVyPgo8L2RlZnM+Cjwvc3ZnPgo=' type='image/x-icon'>
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>WSLED-lightControl</title>
    <meta charset='UTF-8'>
<style>
    :root{
        --size: calc( 1vh + 1vw );
    }
    *{
        margin: 0;
        padding: 0;
        box-sizing: border-box;
    }
    html body{
        width: 100%;
        height: 100%;
        background: #242527;
        color: white;
    }
</style>
</head>
<body>
    <header>
        <div style='background-color: #242527; color: white; min-height: 34px; display: flex; align-items: center; padding: 0 15px; display: flex; flex-wrap: wrap; justify-content: space-between;'>
            <p style='font: sans-serif; font-size: 12px;'>Режим роботи:
                <select style='border-radius: 6px; background: #18191B; color: white; padding: 3px 14px; font-size: 12px;' name='mode' onchange='switchMode(event)' id='select'>
                    <option value='1' selected='false'>Постійний</option>
                    <option value='2' selected='true'>Циклічний</option>
                </select>
            </p>
            <input type="range" name="brightness" id="brightness" min="0" max="255" value="1" onmouseup='brightness(event)' ontouchend='brightness(event)'>
        </div>
    </header>
    <main style='display: flex; flex-direction: column;'>
        <div style='min-height: calc(100vh - 34px); background: #18191B; display: flex; width: 100%; align-items: center; justify-content: center; flex-direction: column; position: relative;'>
            <img style='width: calc( var(--size) * 15); margin-bottom: 45px; cursor: pointer; z-index: 1;' onclick='toggleReley()' alt='light' src='data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTg5IiBoZWlnaHQ9IjIyNiIgdmlld0JveD0iMCAwIDE4OSAyMjYiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxwYXRoIGZpbGwtcnVsZT0iZXZlbm9kZCIgY2xpcC1ydWxlPSJldmVub2RkIiBkPSJNMTUyLjE5NiAxMDYuMDk4QzE1Mi4xOTYgMTIyLjQ1NiAxNDUuMjQgMTM4LjA2MiAxMzMuMDE4IDE0OS4xNTVDMTMzLjAxOCAxNDkuMzQzIDEzMi44MyAxNDkuNTMxIDEzMi42NDIgMTQ5LjcxOUMxMjMuNjY2IDE1OC42OTUgMTI0LjMyOCAxNzcuNDg5IDEyNC4zNjggMTc4LjYyMkMxMjQuMzY5IDE3OC42NTEgMTI0LjM2OSAxNzguNjY5IDEyNC4zNjkgMTc4LjY3NEMxMjQuMzY5IDE3OS42MTQgMTI0LjE4MSAxODAuNTU0IDEyMy40MjkgMTgxLjMwNkMxMjIuNjc3IDE4Mi4wNTggMTIxLjczNyAxODIuNDM0IDEyMC43OTcgMTgyLjQzNEg2Ny41ODc0QzY2LjY0NzMgMTgyLjQzNCA2NS43MDcyIDE4Mi4wNTggNjQuOTU1MSAxODEuMzA2QzY0LjM5MSAxODAuNTU0IDY0LjAxNSAxNzkuNjE0IDY0LjAxNSAxNzguNjc0QzY0LjAxNSAxNzguNjY5IDY0LjAxNTYgMTc4LjY1MSA2NC4wMTY2IDE3OC42MjJDNjQuMDU2NiAxNzcuNDg5IDY0LjcxODUgMTU4LjY5NSA1NS43NDIxIDE0OS43MTlDNDMuMzMyOCAxMzguODE0IDM2IDEyMi44MzIgMzYgMTA2LjA5OEMzNiA3NC4xMzQ4IDYyLjEzNDggNDggOTQuMDk4MiA0OEMxMjYuMDYyIDQ4IDE1Mi4xOTYgNzQuMTM0OCAxNTIuMTk2IDEwNi4wOThaTTEyNy4wMDIgMTQ1LjM5NEMxMjcuMTkgMTQ1LjAxOCAxMjcuMzc4IDE0NC42NDIgMTI3Ljc1NCAxNDQuNDU0QzEzOC44NDcgMTM0Ljg2NSAxNDUuMDUyIDEyMC43NjQgMTQ1LjA1MiAxMDYuMjg2QzE0NS4wNTIgNzguMjcxMiAxMjIuMzAxIDU1LjMzMjggOTQuMDk4MiA1NS4zMzI4QzY2LjA4MzIgNTUuMzMyOCA0My4xNDQ4IDc4LjA4MzIgNDMuMTQ0OCAxMDYuMjg2QzQzLjE0NDggMTIwLjk1MiA0OS41Mzc0IDEzNC44NjUgNjAuNjMwNiAxNDQuNjQyQzY5LjQ2NzYgMTUzLjQ3OSA3MC45NzE3IDE2OC41MjEgNzEuMTU5NyAxNzUuNDc4SDExNy4yMjVDMTE3LjQxMyAxNjguNzA5IDExOC43MjkgMTU0LjIzMSAxMjcuMDAyIDE0NS4zOTRaTTkwLjUyNTIgNjYuNjEzNEM5MC41MjUyIDY0LjU0NTIgOTIuMjE3NCA2My4wNDEgOTQuMDk3NiA2My4wNDFDMTE5LjEwNCA2My4wNDEgMTM5LjQxIDgzLjM0NzIgMTM5LjQxIDEwOC4zNTRDMTM5LjQxIDExMC40MjIgMTM3LjcxOCAxMTEuOTI2IDEzNS44MzggMTExLjkyNkMxMzMuNzcgMTExLjkyNiAxMzIuMjY2IDExMC4yMzQgMTMyLjI2NiAxMDguMzU0QzEzMi4yNjYgODcuMjk1NiAxMTUuMTU2IDcwLjE4NTggOTQuMDk3NiA3MC4xODU4QzkyLjAyOTQgNzAuMTg1OCA5MC41MjUyIDY4LjQ5MzYgOTAuNTI1MiA2Ni42MTM0Wk0xMTcuNDEyIDE4Ny4xMzRDMTIyLjExMyAxODcuMTM0IDEyNi4wNjEgMTkwLjg5NSAxMjYuMDYxIDE5NS43ODNDMTI2LjA2MSAyMDAuNjcyIDEyMi4xMTMgMjA0LjQzMiAxMTcuMjI0IDIwNC40MzJINzAuOTcxMkM2Ni4yNzA3IDIwNC40MzIgNjIuMzIyMyAyMDAuNjcyIDYyLjMyMjMgMTk1Ljc4M0M2Mi4zMjIzIDE5MS4wODMgNjYuMDgyNyAxODcuMTM0IDcwLjk3MTIgMTg3LjEzNEgxMTcuNDEyWk02OS40NjcgMTk1Ljc4M0M2OS40NjcgMTk2LjUzNSA3MC4wMzExIDE5Ny4yODcgNzAuOTcxMiAxOTcuMjg3SDExNy4yMjRDMTE4LjE2NCAxOTcuMjg3IDExOC45MTYgMTk2LjcyMyAxMTguNzI4IDE5NS43ODNDMTE4LjcyOCAxOTUuMDMxIDExOC4xNjQgMTk0LjI3OSAxMTcuMjI0IDE5NC4yNzlINzAuOTcxMkM3MC4yMTkxIDE5NC4yNzkgNjkuNDY3IDE5NC44NDMgNjkuNDY3IDE5NS43ODNaTTc3LjkyNzkgMjA4LjE5MkgxMTAuNDU1QzExNS4xNTYgMjA4LjE5MiAxMTkuMTA0IDIxMi4xNDEgMTE5LjEwNCAyMTYuODQxQzExOS4xMDQgMjIxLjU0MiAxMTUuMzQ0IDIyNS40OSAxMTAuNDU1IDIyNS40OUg3Ny45Mjc5QzczLjIyNzQgMjI1LjQ5IDY5LjI3OSAyMjEuNzMgNjkuMjc5IDIxNi44NDFDNjkuMjc5IDIxMi4xNDEgNzMuMDM5NCAyMDguMTkyIDc3LjkyNzkgMjA4LjE5MlpNNzcuOTI3OSAyMTguMzQ1SDExMC40NTVDMTExLjM5NSAyMTguMzQ1IDExMS45NiAyMTcuNzgxIDExMS45NiAyMTYuODQxQzExMS45NiAyMTYuMDg5IDExMS4zOTUgMjE1LjMzNyAxMTAuNDU1IDIxNS4zMzdINzcuOTI3OUM3Ny4xNzU4IDIxNS4zMzcgNzYuNDIzOCAyMTUuOTAxIDc2LjQyMzggMjE2Ljg0MUM3Ni40MjM4IDIxNy41OTMgNzYuOTg3OCAyMTguMzQ1IDc3LjkyNzkgMjE4LjM0NVoiIGZpbGw9IndoaXRlIi8+Cjwvc3ZnPgo='>
            <img style='width: calc( var(--size) * 15); margin-bottom: 45px; position: absolute; display: none;' alt='light' id='image' src='data:image/svg+xml;base64,PHN2ZyB3aWR0aD0iMTg5IiBoZWlnaHQ9IjIyNiIgdmlld0JveD0iMCAwIDE4OSAyMjYiIGZpbGw9Im5vbmUiIHhtbG5zPSJodHRwOi8vd3d3LnczLm9yZy8yMDAwL3N2ZyI+CjxwYXRoIGQ9Ik0xMjkuNjI2IDE0Ny43MDJDMTE5LjA5NyAxNTguMjMxIDEyMC40MTMgMTc5LjQ3OCAxMjAuNDEzIDE3OS40NzhINjcuMDE1QzY3LjAxNSAxNzkuNDc4IDY4LjE0MzEgMTU4LjIzMSA1Ny44MDIgMTQ3LjcwMkM0Ni4zMzI4IDEzNy41NDkgMzkgMTIyLjg4NCAzOSAxMDYuNTI2QzM5IDc2LjQ0MjYgNjMuNDQyNiA1MiA5My41MjU4IDUyQzEyMy42MDkgNTIgMTQ4LjA1MiA3Ni40NDI2IDE0OC4wNTIgMTA2LjUyNkMxNDguMDUyIDEyMi44ODQgMTQwLjkwNyAxMzcuNTQ5IDEyOS42MjYgMTQ3LjcwMloiIGZpbGw9IiNGRkQ1MTciLz4KPHBhdGggZD0iTTk3LjcyMTIgMy41NzIzOFYyNy42Mzg5Qzk3LjcyMTIgMjkuNzA3MiA5Ni4yMTcgMzEuMjExMyA5NC4xNDg4IDMxLjM5OTNDOTIuMDgwNiAzMS4zOTkzIDkwLjU3NjQgMjkuNzA3MiA5MC41NzY0IDI3LjgyN1YzLjU3MjM4QzkwLjU3NjQgMS41MDQxNiA5Mi4yNjg2IDAgOTQuMTQ4OCAwQzk2LjIxNyAwIDk3LjcyMTIgMS42OTIxOCA5Ny43MjEyIDMuNTcyMzhaIiBmaWxsPSIjRkZENTE3Ii8+CjxwYXRoIGQ9Ik0xNDguNDg3IDE4LjgwMkMxNDkuNjE1IDE3LjI5NzggMTUxLjY4MyAxNi43MzM4IDE1My4zNzUgMTcuODYxOUMxNTQuODc5IDE4Ljk5IDE1NS40NDMgMjEuMjQ2MyAxNTQuMzE1IDIyLjc1MDRMMTQwLjk2NiA0Mi44Njg1QzE0MC4yMTQgNDMuODA4NyAxMzkuMDg2IDQ0LjM3MjcgMTM3Ljk1NyA0NC4zNzI3QzEzNy4zOTMgNDQuMzcyNyAxMzYuNjQxIDQ0LjE4NDcgMTM2LjA3NyA0My44MDg2QzEzNC41NzMgNDIuNjgwNSAxMzQuMDA5IDQwLjYxMjMgMTM1LjEzNyAzOC45MjAxTDE0OC40ODcgMTguODAyWiIgZmlsbD0iI0ZGRDUxNyIvPgo8cGF0aCBkPSJNNTMuNTM2NSA0My4wNTY2QzUyLjk3MjQgNDMuNDMyNiA1Mi4yMjA0IDQzLjYyMDYgNTEuNjU2MyA0My42MjA2QzUwLjUyODIgNDMuNjIwNiA0OS40MDAxIDQzLjA1NjYgNDguNjQ4IDQxLjkyODRMMzUuNjc0NiAyMS42MjIzQzM0LjczNDUgMTkuOTMwMSAzNS4xMTA1IDE3Ljg2MTkgMzYuODAyNyAxNi43MzM4QzM4LjQ5NDkgMTUuNzkzNyA0MC41NjMxIDE2LjE2OTcgNDEuNjkxMiAxNy44NjE5TDU0LjY2NDYgMzguMTY4QzU1LjYwNDcgMzkuODYwMiA1NS4yMjg3IDQxLjkyODQgNTMuNTM2NSA0My4wNTY2WiIgZmlsbD0iI0ZGRDUxNyIvPgo8cGF0aCBkPSJNMjMuMDc3MyA3Mi4wMTE2TDEuODMxIDYwLjU0MjRDMC4xMzg4MjQgNTkuNjAyMyAtMC40MjUyMzQgNTcuNTM0MSAwLjMyNjg0NiA1NS42NTM5QzEuMjY2OTUgNTMuNzczNyAzLjUyMzE4IDUzLjIwOTYgNS4yMTUzNiA1NC4xNDk3TDI2LjQ2MTYgNjUuNjE5QzI4LjM0MTggNjYuNTU5MSAyOC45MDU5IDY4LjgxNTMgMjcuOTY1OCA3MC41MDc1QzI3LjIxMzcgNzEuNjM1NiAyNi4wODU2IDcyLjM4NzcgMjQuNzY5NCA3Mi4zODc3QzI0LjIwNTQgNzIuMzg3NyAyMy42NDEzIDcyLjM4NzcgMjMuMDc3MyA3Mi4wMTE2WiIgZmlsbD0iI0ZGRDUxNyIvPgo8cGF0aCBkPSJNMTY1LjIyIDcyLjE5OTdDMTY0LjY1NiA3Mi4zODc3IDE2NC4wOTIgNzIuNTc1NyAxNjMuNTI4IDcyLjU3NTdDMTYyLjIxMiA3Mi41NzU3IDE2MC44OTYgNzEuODIzNiAxNjAuMzMyIDcwLjY5NTVDMTU5LjM5MiA2OC44MTUzIDE2MC4xNDQgNjYuNzQ3MSAxNjEuODM2IDY1LjgwN0wxODMuMDgyIDU0LjMzNzhDMTg0Ljk2MiA1My4zOTc3IDE4Ny4wMzEgNTQuMTQ5NyAxODcuOTcxIDU1Ljg0MTlDMTg4LjkxMSA1Ny43MjIxIDE4OC4xNTkgNTkuNzkwMyAxODYuNDY3IDYwLjczMDRMMTY1LjIyIDcyLjE5OTdaIiBmaWxsPSIjRkZENTE3Ii8+Cjwvc3ZnPgo='>
            <div style='display: flex; color: white; width: calc(100vw - 40px); justify-content: space-between; position: absolute; bottom: 0; left: 0; margin: 20px 10px;'>
                <span style='padding: 5px 12px; background-color: #242527; border-radius: 10px;' id='startTime'>Час роботи: <span></span> год</span>
                <span style='padding: 5px 12px; background-color: #242527; border-radius: 10px;' id='toggleTime'>Час до <span></span> год</span>
            </div>
        </div>
        <div style='display: flex; flex-direction: column; align-items: center; margin-bottom: 30px;'>
            <h2 style='padding: 20px;'>Налаштування циклу</h2>
            <form method='post' action='/save' onsubmit='submitForm(event)' style='background: #18191B; padding: 20px 26px; border-radius: 10px; display: flex; flex-direction: column; gap: 30px;' >
                    <div>Час до вимкнення (год): <input type='text' name='timeoutA' value='0' style='background: #242527; border: none; color: white;border-radius: 10px; padding: 6px 10px; margin: 0 0 0 10px;' id='timeOff'><br></div>
                    <div>Час до увімкнення (год): <input type='text' name='timeoutB' value='0' style='background: #242527; border: none; color: white;border-radius: 10px; padding: 6px 10px; margin: 0 0 0 10px;' id='timeOn'><br></div>
                    <div style='display: flex; justify-content: space-between;'>
                        <input type='submit' value='Зберегти' style='color: white; background-color: #242527; border: none; padding: 8px 42px; border-radius: 12px;'>
                        <button onclick='startLoop()' style='color: white; background-color: #242527; border: none; padding: 8px 42px; border-radius: 12px;'>Увімкнути</button>
                    </div>
            </form>
        </div>
        <div style='display: flex; justify-content: center; margin-bottom: 50px;'>
            <div style='display: flex; align-items: center; gap: 20px; flex-direction: column; width: max-content;'>
                <button onclick='clearDate()' style='border: 1px solid white; color: white; background: #18191B; padding: 10px 50px; border-radius: 20px; width: 100%;'>Очистити кеш</button>
                <button onclick='reset()' style='border: 1px solid white; color: white; background: #18191B; padding: 10px 50px; border-radius: 20px; width: 100%'>Перезавантажити</button>
                <a href='./update' style='border: 1px solid white; color: white; background: #18191B; padding: 10px 50px; border-radius: 20px; width: 100%; text-decoration: none;'>Оновити прошивку</a>
            </div>
        </div>
    </main>
        <script>
            function fetchData(){
                fetch('/data')
                    .then(response => response.json())
                    .then(data => {
                        console.log('Data from ESP8266:', data);
                        const stateLoop = data.Loop;
                        const stateRelay = data.OnRelay;
                        const stateRelayOnLoop = data.relayStateOnLoop;
                        const startTime = data.second;
                        const prevTime = data.previousMillis;
                        const timeOff = data.timeOff;
                        const timeOn = data.timeOn;
                        const brightness = data.brightness;
                    
                        const itemSelect = document.getElementById('select');
                        const itemImage = document.getElementById('image');
                        const displayToggleTime = document.getElementById('toggleTime');
                        const displayStartTime = document.getElementById('startTime');
                        const inputTimeOff = document.getElementById('timeOff');
                        const inputTimeOn = document.getElementById('timeOn');
                        const brightnessRange = document.getElementById('brightness');
    
                        if(stateLoop){
                            itemSelect.children[1].selected = true;
                            itemSelect.children[0].selected = false;
                        }else{
                            itemSelect.children[0].selected = true;
                            itemSelect.children[1].selected = false;
                        }
    
                        if(stateRelay){
                            itemImage.style.display = 'block';
                        }else if(stateRelayOnLoop){
                            itemImage.style.display = 'block';
                        }else{
                            itemImage.style.display = 'none';
                        }
    
                        if(stateLoop){
                            if(stateRelayOnLoop){
                                displayToggleTime.children[0].innerText = `вимкнення: ${Math.abs( Math.ceil( ((startTime - prevTime)/1000/60/60 - timeOn) * 10) /10) }`;
                            }else{
                                displayToggleTime.children[0].innerText = `увімкнення: ${Math.abs(Math.ceil( ((startTime - prevTime)/1000/60/60 - timeOff) * 10) /10)}`;
                            }
                        }else{
                            displayToggleTime.children[0].innerText = `переключення: 0`;
                        }

                        brightnessRange.value = brightness;
                        displayStartTime.children[0].innerText = Math.ceil(startTime/1000/60/60*10)/10;
                        inputTimeOff.value = timeOff;
                        inputTimeOn.value = timeOn;
                    })
                    .catch(error => console.error('Error fetching data:',error));
            }
    
            function submitForm(event){
                event.preventDefault();
                const form = event.target;
                const formData = new FormData(form);
                const xhr = new XMLHttpRequest();
                console.log(formData);
                xhr.open('POST', form.action, true);
                xhr.onreadystatechange = function() {
                    if(xhr.status === 200) {
                        console.log('Change form: true');
                    }else{
                        console.error('Change form: error', xhr.readyState, xhr.status);
                    }
                };
                xhr.send(formData);
                fetchData();
            };
                
            function startLoop(){
                const xhr = new XMLHttpRequest();
    
                const url = '/loop';
                xhr.open('GET', url, true);
    
                xhr.onreadystatechange = function(){
                    if(xhr.status === 200){
                        console.log('loop statys: 1');
                    }else{
                        console.error('loop statys: error submit', xhr.readyState, xhr.status);
                    }
                };
                
                xhr.send();
                fetchData();
            };
            
            function toggleReley(){
                const xhr = new XMLHttpRequest();
    
                const url = '/toggle';
                xhr.open('GET', url, true);
    
                xhr.onreadystatechange = function(){
                    if(xhr.status === 200){
                        console.log('Relay statys: 1');
                    }else{
                        console.error('Relay statys: error submit', xhr.readyState, xhr.status);
                    }
                };
                xhr.send();
                fetchData();
            };
    
            function clearDate(){
                const xhr = new XMLHttpRequest();
    
                const url = '/clear';
                xhr.open('GET', url, true);
    
                xhr.onreadystatechange = function(){
                    if(xhr.status === 200){
                        console.log('Relay clear: 1');
                    }else{
                        console.error('Relay clear: error submit', xhr.readyState, xhr.status);
                    }
                };
                
                xhr.send();
                fetchData();
            };
    
            function reset(){
                const xhr = new XMLHttpRequest();
    
                const url = '/rest';
                xhr.open('GET', url, true);
    
                xhr.onreadystatechange = function(){
                    if(xhr.status === 200){
                        console.log('Rest state: 1');
                    }else{
                        console.error('Rest state: error submit'), xhr.readyState, xhr.status;
                    }
                };
                
                xhr.send();
            };
            
            function switchMode(event){
                const value = event.target.value;
                if(value == 1){
                    toggleReley();
                }else if(value == 2){
                    startLoop();
                }
                fetchData();
            }

            function brightness(event){
            const value = event.target.value;
            const xhr = new XMLHttpRequest();
            const url = `/brightness?value=${encodeURIComponent(value)}`;

            xhr.onreadystatechange = function() {
                if (xhr.readyState === XMLHttpRequest.DONE) {
                    if (xhr.status === 200) {
                        console.log('Данные успешно отправлены на сервер:', xhr.responseText);
                    } else {
                        console.error('Произошла ошибка при отправке данных:',xhr.readyState, xhr.status);
                    }
                }
            }
            xhr.open('GET', url, true);
            xhr.send();
            console.log(url);
            fetchData();
        }

        setInterval(fetchData, 1000*12);
        fetchData();
        </script>
</body>
</html>

)";

    server.send(200, "text/html", html);
}

String responseHTML = ""
    "<!DOCTYPE html><html lang='uk-Ua'><head>"
    "<meta charset='UTF-8'>"
    "<meta name='viewport' content='width=device-width'>"
    "<title>WSLED</title></head><body style='display: flex; align-items: center; flex-direction: column;'>"
    "<h1>Панель керування освітленням WSLED</h1>"
    "<a style='border: 1px solid white; color: white; background: #18191B; padding: 10px 50px; border-radius: 20px; width: 100%; text-decoration: none; max-width: fit-content;' href='http://" + apIP.toString() + "'>Перейти до керування</a></body></html>";

void handleSave() {
  // Получение значений из формы и сохранение их в EEPROM
  timeoutA = server.arg("timeoutA").toInt();
  timeoutB = server.arg("timeoutB").toInt();
  if(timeoutA > 9999){timeoutA = 9999;}else if(timeoutA < 1){timeoutA = 1;}
  if(timeoutB > 9999){timeoutB = 9999;}else if(timeoutB < 1){timeoutB = 1;}

  EEPROM.put(20, timeoutA);
  EEPROM.put(30, timeoutB);
  EEPROM.commit();

  server.send(200, "text/plain", "Настройки сохранены. Перезапустите устройство.");
  Serial.println("Отправка формы");
}

void handleToggle(){
  if(StatusLoop){
    StatusLoop = !StatusLoop;
    relayState = LOW;
    EEPROM.put(10, StatusLoop);
    server.send(200, "text/plain", StatusRelay ? "Цыкл выключен" : "Ошибка цыкла");
  }else{
    StatusRelay = !StatusRelay;
    relayState = LOW;
    EEPROM.put(0, StatusRelay);
    EEPROM.commit();
    previousMillis = 0;
    server.send(200, "text/plain", StatusRelay ? "Переключено релле" : "Ошибка релле");
  }
  Serial.println("Переключатель");
}

void handleLoop(){
  if(StatusRelay){
    StatusRelay = 0;
    EEPROM.put(0, StatusRelay);
  }
  StatusLoop = 1;
  relayState = HIGH;
  EEPROM.put(10, StatusLoop);
  EEPROM.commit();

  server.send(200, "text/plain", StatusLoop ? "Включено цикл" : "Выключено цикл");
  Serial.println("Активация цыкла");
}

void handleClear(){
  for (int i = 0; i < 512; i++) {
    EEPROM.write(i, 0); // Запись нулевых значений во все ячейки EEPROM
  }
  EEPROM.commit();

  EEPROM.get(20, timeoutA);
  EEPROM.get(30, timeoutB);
  EEPROM.get(0, StatusRelay);
  EEPROM.get(10, StatusLoop);
  EEPROM.get(40, valueBrightness);
  relayState = LOW;
  Serial.println("Очистка");
  server.send(200, "text/plain", StatusLoop ? "Готово" : "Не готово");
}

void relayLoop(){
  unsigned long currentMillis = millis();
  if(relayState == HIGH){
    if(currentMillis - previousMillis >= timeoutA*1000*60*60){
      relayState = LOW;
      previousMillis = currentMillis;
      Serial.println("В цыкле статус ВЫКЛ");
    }
  }else {
    if (currentMillis - previousMillis >= timeoutB*1000*60*60) {
      relayState = HIGH;
      previousMillis = currentMillis;
      Serial.println("В цыкле статус ВКЛ");
    }
  }
  
}

void handleRestart(){
    server.send(200, "text/plain", "Rebooting...");
      Serial.println("Перезапуск");
    delay(1000);
    ESP.restart();
}

void handleDataRequest(){
    StaticJsonDocument<200> jsonDoc;

    jsonDoc["second"] = millis();
    jsonDoc["timeOn"] = timeoutA;
    jsonDoc["timeOff"] = timeoutB;
    jsonDoc["OnRelay"] = StatusRelay;
    jsonDoc["Loop"] = StatusLoop;
    jsonDoc["relayStateOnLoop"] = relayState;
    jsonDoc["previousMillis"] = previousMillis;
    jsonDoc["brightness"] = valueBrightness;

    String jsonString;
    serializeJson(jsonDoc, jsonString);
    server.send(200, "application/json", jsonString);
}

void handleBrightness(){
  if(server.hasArg("value")){
    valueBrightness = server.arg("value").toInt();
    Serial.print("Received brightness value: ");
    Serial.println(valueBrightness);
    EEPROM.put(40, valueBrightness);
    if(StatusLoop){
      relayLoop();
    }
    EEPROM.commit();
  }
  server.send(200, "text/plain", "OK"); 
}

void setup(void) {
  EEPROM.begin(512);

  EEPROM.get(20, timeoutA);
  EEPROM.get(30, timeoutB);
  EEPROM.get(0, StatusRelay);
  EEPROM.get(10, StatusLoop);
  EEPROM.get(40, valueBrightness);
  
  Serial.begin(115200);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid, password);

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);
  server.on("/toggle", HTTP_GET, handleToggle);
  server.on("/loop", HTTP_GET, handleLoop);
  server.on("/clear", HTTP_GET, handleClear);
  server.on("/rest", HTTP_GET, handleRestart);
  server.on("/data", HTTP_GET, handleDataRequest);
  server.on("/brightness", HTTP_GET, handleBrightness);

  pinMode(RelayPin, OUTPUT);

  analogWriteFreq(200);
  analogWriteRange(255);

  ElegantOTA.begin(&server);
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd);

  dnsServer.start(DNS_PORT, "*", apIP);
  
  server.onNotFound([]() {
    server.send(200, "text/html", responseHTML);
  });

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {

  if(previousMillis == 0 && StatusLoop){
        StatusRelay = HIGH;
        previousMillis = millis();
  }

  if(StatusLoop){
    StatusRelay = 0;
    relayLoop();
  }else if(StatusRelay){
    relayState = LOW;
  }

  analogWrite(RelayPin, relayState || StatusRelay ? valueBrightness : 0);

  dnsServer.processNextRequest();
  server.handleClient();
  ElegantOTA.loop();
}
