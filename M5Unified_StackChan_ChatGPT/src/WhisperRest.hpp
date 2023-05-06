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
    StaticJsonDocument<JSON_DOCSIZE> rest_json_doc;

   public:
    WhisperRest(String rest_url, String lang);
    ~WhisperRest();

    String Pcm2String(uint8_t* pcm_buff, uint32_t pcm_len);
};


/// @brief HTTP Client for POST using multipart/form-data
class MHTTPClient : HTTPClient {
  public:
    int MPOST(uint8_t * payload, size_t size);
    
    bool begin(String url) { return HTTPClient::begin(url) };
    String getString(void) { return HTTPClient::getString() };

};

#endif