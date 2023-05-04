/*
  AudioFileSourceCoquiTTSStream
  Streaming CoquiTTS source

  Copyright (C) 2023  edgeless

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#if defined(ESP32) || defined(ESP8266)

#include "AudioFileSourceCoquiTTSStream.h"

// Coqui TTS Web API
// # python3 TTS/server/server.py --model_name tts_models/ja/kokoro/tacotron2-DDC
const String tts_url = "http://192.168.2.52:5002/api/tts";

// from http://hardwarefun.com/tutorials/url-encoding-in-arduino
// modified by chaeplin
static String URLEncode(const char* msg) {
  const char *hex = "0123456789ABCDEF";
  String encodedMsg = "";

  while (*msg != '\0') {
    if ( ('a' <= *msg && *msg <= 'z')
         || ('A' <= *msg && *msg <= 'Z')
         || ('0' <= *msg && *msg <= '9')
         || *msg  == '-' || *msg == '_' || *msg == '.' || *msg == '~' ) {
      encodedMsg += *msg;
    } else {
      encodedMsg += '%';
      encodedMsg += hex[*msg >> 4];
      encodedMsg += hex[*msg & 0xf];
    }
    msg++;
  }
  return encodedMsg;
}

//unchange
AudioFileSourceCoquiTTSStream::AudioFileSourceCoquiTTSStream()
{
  pos = 0;
  reconnectTries = 0;
  saveURL[0] = 0;
}

//unchange
AudioFileSourceCoquiTTSStream::AudioFileSourceCoquiTTSStream(const char *tts_text, const char *tts_parms)
{
// printf("AudioFileSourceCoquiTTSStream\r\n");
  saveURL[0] = 0;
  reconnectTries = 0;
  text = tts_text;
  parms = tts_parms;
  open((const char *)tts_url.c_str());
}

bool AudioFileSourceCoquiTTSStream::open(const char *url)
{
  pos = 0;
  http.begin(url + String("?text=") + URLEncode(text) + String(parms));
#ifndef ESP32
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
#endif

  //Make the request
  int code = http.GET();

  printf("code=%d\r\n",code);

  if (code != HTTP_CODE_OK) {
    http.end();
    cb.st(STATUS_HTTPFAIL, PSTR("Can't open HTTP request"));
//  printf("Can't open HTTP request\n\r");
    return false;
  }
  size = http.getSize();
// printf("size=%d\r\n",size);
  strncpy(saveURL, url, sizeof(saveURL));
  saveURL[sizeof(saveURL)-1] = 0;
  return true;
}

//unchange
AudioFileSourceCoquiTTSStream::~AudioFileSourceCoquiTTSStream()
{
  http.end();
}

//unhange
uint32_t AudioFileSourceCoquiTTSStream::read(void *data, uint32_t len)
{
  if (data==NULL) {
    audioLogger->printf_P(PSTR("ERROR! AudioFileSourceCoquiTTSStream::read passed NULL data\n"));
    return 0;
  }
  return readInternal(data, len, false);
}

//unhange
uint32_t AudioFileSourceCoquiTTSStream::readNonBlock(void *data, uint32_t len)
{
  if (data==NULL) {
    audioLogger->printf_P(PSTR("ERROR! AudioFileSourceCoquiTTSStream::readNonBlock passed NULL data\n"));
    return 0;
  }
  return readInternal(data, len, true);
}

//unhange
uint32_t AudioFileSourceCoquiTTSStream::readInternal(void *data, uint32_t len, bool nonBlock)
{
retry:
  if (!http.connected()) {
    cb.st(STATUS_DISCONNECTED, PSTR("Stream disconnected"));
    http.end();
    for (int i = 0; i < reconnectTries; i++) {
      char buff[64];
      sprintf_P(buff, PSTR("Attempting to reconnect, try %d"), i);
      cb.st(STATUS_RECONNECTING, buff);
      delay(reconnectDelayMs);
      if (open(saveURL)) {
        cb.st(STATUS_RECONNECTED, PSTR("Stream reconnected"));
        break;
      }
    }
    if (!http.connected()) {
      cb.st(STATUS_DISCONNECTED, PSTR("Unable to reconnect"));
      return 0;
    }
  }
  if ((size > 0) && (pos >= size)) return 0;

  WiFiClient *stream = http.getStreamPtr();

  // Can't read past EOF...
  if ( (size > 0) && (len > (uint32_t)(pos - size)) ) len = pos - size;

  if (!nonBlock) {
    int start = millis();
    while ((stream->available() < (int)len) && (millis() - start < 500)) yield();
  }

  size_t avail = stream->available();
  if (!nonBlock && !avail) {
    cb.st(STATUS_NODATA, PSTR("No stream data available"));
    http.end();
    goto retry;
  }
  if (avail == 0) return 0;
  if (avail < len) len = avail;

  int read = stream->read(reinterpret_cast<uint8_t*>(data), len);
  pos += read;
  return read;
}

//unhange
bool AudioFileSourceCoquiTTSStream::seek(int32_t pos, int dir)
{
  audioLogger->printf_P(PSTR("ERROR! AudioFileSourceCoquiTTSStream::seek not implemented!"));
  (void) pos;
  (void) dir;
  return false;
}

//unhange
bool AudioFileSourceCoquiTTSStream::close()
{
  http.end();
  return true;
}

//unhange
bool AudioFileSourceCoquiTTSStream::isOpen()
{
  return http.connected();
}

//unhange
uint32_t AudioFileSourceCoquiTTSStream::getSize()
{
  return size;
}

//unhange
uint32_t AudioFileSourceCoquiTTSStream::getPos()
{
  return pos;
}

#endif
