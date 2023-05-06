// https://github.com/m5stack/M5AtomU/blob/master/examples/Audio/STT/WhisperRest.h

#ifndef _WHISPERREST_H_
#define _WHISPERREST_H_

#include <HTTPClient.h>
#include <ArduinoJson.h>
// #include <driver/i2s.h>

#define JSON_DOCSIZE 512

class WhisperRest {
   private:
    String api_token_str;
    String rest_url;
    String lang;
    HTTPClient resthttp;
    StaticJsonDocument<JSON_DOCSIZE> rest_json_doc;
    int MPOST(uint8_t * payload, size_t size);

   public:
    WhisperRest(String rest_url, String lang);
    ~WhisperRest();

    // HTTPClient don't have copy constructer.
    WhisperRest(const WhisperRest&) = delete;
    WhisperRest& operator=(const WhisperRest&) = delete;

    String Pcm2String(uint8_t* pcm_buff, uint32_t pcm_len);
};

#endif