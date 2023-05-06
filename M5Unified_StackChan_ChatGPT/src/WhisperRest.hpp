// https://github.com/m5stack/M5AtomU/blob/master/examples/Audio/STT/WhisperRest.h

#ifndef _WHISPERREST_H_
#define _WHISPERREST_H_

#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <driver/i2s.h>

#define JSON_DOCSIZE 512

struct pcmPack {
    uint8_t* packData;
    uint32_t packsize;
};

class WhisperRest {
   private:
    String api_token_str;
    StaticJsonDocument<JSON_DOCSIZE> rest_json_doc;
    char ascii2hex_buff[17] = "0123456789abcdef";

   public:
    QueueHandle_t xQ_i2sSteam = NULL;

   public:
    WhisperRest();
    ~WhisperRest();
    int gettoken(void);
    int creattoken(void);
    void settoken(String token_str);

    int Pcm2String(uint8_t* pcm_buff, uint32_t pcm_lan, String dev_pid,
                   String* results_str);
    int String2Pcm(String str, int spd, int pit, int vol, int per,
                   uint8_t* pcm_buff, size_t* len);
    int String2Pcm(String str, int spd, int pit, int vol, int per);
    String Str2UrlEncode(String str);
};

#endif