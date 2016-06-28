import json
import urllib2
import ConfigParser
from OmegaExpansion import onionI2C
from time import sleep

config = ConfigParser.RawConfigParser()
config.read('suretrac.cfg')

postUrl = config.get("SureTrac", "post_url")
postRate = config.getfloat("SureTrac", "post_rate")
ARDUINO_ADDR = 0x08
i2c = onionI2C.OnionI2C()


def ReadLatestSentence():
    currentByte = 0x00
    tempSentence = ""

    while (not chr(currentByte) == '*'):
        currentByte = i2c.readBytes(ARDUINO_ADDR, 0x00, 1)[0]
        tempSentence += chr(currentByte)

    currentByte = i2c.readBytes(ARDUINO_ADDR, 0x00, 1)[0]
    tempSentence += chr(currentByte)
    currentByte = i2c.readBytes(ARDUINO_ADDR, 0x00, 1)[0]
    tempSentence += chr(currentByte)

    try:
        sentence = str(tempSentence.strip())
        data = json.dumps({"nmea-sentence": sentence})
        clen = len(data)
        req = urllib2.Request(postUrl, data,
                              {'Content-Length': clen,
                               'Content-Type': 'application/json'})
        req.get_method = lambda: 'PUT'
        urllib2.urlopen(req)

        sleep(postRate)
    except Exception as err:
        print("EXCEPTION! - {0}".format(err))
        pass

while(True):
    ReadLatestSentence()
