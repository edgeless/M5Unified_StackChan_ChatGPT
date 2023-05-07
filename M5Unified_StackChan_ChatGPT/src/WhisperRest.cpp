// https://github.com/m5stack/M5AtomU/blob/master/examples/Audio/STT/WhisperRest.cpp

#include "WhisperRest.hpp"

/// @brief 
/// @param _rest_url Whisper Host. ex) 192.168.2.52:9000
/// @param _lang Language. ex) ja
WhisperRest::WhisperRest(String _rest_url, String _lang) {
    rest_url = _rest_url;
    lang = _lang;
    String apiurl = "http://";
    apiurl.concat(rest_url);
    apiurl.concat("/asr");
    apiurl.concat('?');
    apiurl.concat("method=openai-whisper");
    apiurl.concat('&');
    apiurl.concat("task=transcribe");
    apiurl.concat('&');
    apiurl.concat("language=");
    apiurl.concat(lang);
    apiurl.concat('&');
    apiurl.concat("encode=false");
    apiurl.concat('&');
    apiurl.concat("output=json");

    int res = resthttp.begin(apiurl);
    resthttp.setTimeout(10000);
    Serial.printf("resthttp.begin(%s): %d\n", apiurl.c_str(), res);
}

WhisperRest::~WhisperRest() {
    resthttp.end();
}

String WhisperRest::Pcm2String(int16_t* pcm_buff, uint32_t pcm_len) {

    uint64_t time = micros();
    
    int ret = MPOST((uint8_t*)pcm_buff, pcm_len * 2);

    Serial.printf("STT Result: %d,  Time %dms\n", ret, (micros() - time) / 10000);
    
    if (ret == 200) {
        String response = resthttp.getString();
        Serial.println(response);
        deserializeJson(rest_json_doc, response);
        Serial.println("text: " + rest_json_doc["text"].as<String>());
    } else {
        for (int i = 0; i<resthttp.headers(); i++)
            Serial.println(resthttp.header(i));
    }

    return rest_json_doc["text"].as<String>();
}

/// @brief multipart POST
/// @param payload
/// @param size 
/// @return 
int WhisperRest::MPOST(uint8_t * payload, size_t size) 
{
    String boundary = "----Boundary97BNFLxCpeVzEZwP";

    resthttp.addHeader("Content-Type", "multipart/form-data; boundary="+boundary);

    String part = "--" + boundary + "\r\n";
    part += "Content-Disposition: form-data; name=\"audio_file\"; filename=\"voice.raw\"\r\n";
    part += "Content-Type: audio/wav\r\n\r\n";
    for (size_t i = 0; i < size; i++) {
      part += (char)payload[i];
    }
    part += "\r\n--" + boundary + "--\r\n";

    return resthttp.POST(part);
}