#include "Ringtones.h"

const Rtttl ringtone[] = {
    {"Silent", "Silent:"},
    {"Default", "24:d=32,o=5,b=565:f6,p,f6,4p,p,f6,p,f6,2p,p,b6,p,b6,p,b6,p,b6,p,b,p,b,p,b,p,b,p,b,p,b,p,b,p,b,1p.,2p.,p,"},
    {"Text Alert", "TextAlert:d=16,o=7,b=225:e6,p,g6,p,e,p,c,p,d,p,2g"},
    {"Pager 1", "Pager1:d=16,o=5,b=200:d6,8p,d6,8p,f6,32p,d6"},
    {"Pager 2", "Pager2:d=16,o=6,b=160:8d,p,2d,p,8d,p,2d,p,8d,p,2d"},
    {"Desk Phone", "DeskPhone:d=8,o=5,b=500:c#,f,c#,f,c#,f,c#,f,c#,f,4p.,c#,f,c#,f,c#,f,c#,f,c#,f,1p.,c#,f,c#,f,c#,f,c#,f,c#,f,"
                   "4p.,c#,f,c#,f,c#,f,c#,f,c#,f"},
    {"Nokia", "Nokia:d=4,o=5,b=225:8e6,8d6,f#,g#,8c#6,8b,d,e,8b,8a,c#,e,2a"},
    {"Ericcson 1",
     "Ericcson1:d=4,o=6,b=180:32b5,32d,32b5,32d,32b5,32d,32b5,32d,32d,32f,32d,32f,32d,32f,32d,32f,32f,32a,32f,32a,32f,32a,32f,"
     "32a,32p,32b5,32d,32b5,32d,32b5,32d,32b5,32d,32d,32f,32d,32f,32d,32f,32d,32f,32f,32a,32f,32a,32f,32a,32f,32a,2p"},
    {"Ericcson 2", "Ericcson2:d=4,o=5,b=355:16b4,16d,16b4,16d,16b4,16d,16b4,16d,16d,16f,16d,16f,16d,16f,16d,16f,16f,16a,16f,16a,"
                   "16f,16a,16f,16a,1p"},
    {"Ericcson 3", "Ericsson3:d=16,o=6,b=355:b5,d,b5,d,b5,d,b5,d,d,f,d,f,d,f,d,f,f,a,f,a,f,a,f,a"},
    {"Double Ring", "DoubleRing:d=16,o=5,b=200:c6,f6,c7,c6,f6,c7,c6,f6,c7,8p,c,f,c6,c,f,c6,c,f,c6"},
    {"Triple Ring", "TripleRing:d=8,o=5,b=635:c,e,g,c,e,g,c,e,g,c6,e6,g6,c6,e6,g6,c6,e6,g6,c7,e7,g7,c7,e7,g7,c7,e7,g7"},
    {"Ring Ring", "RingRing:d=4,o=5,b=100:32b,32d6,32g6,32g6,32g6,8p,32b,32d6,32g6,32g6,32g6,2p,32b,32d6,32g6,32g6,32g6,8p,32b,"
                  "32d6,32g6,32g6,32g6,2p,32b,32d6,32g6,32g6,32g6,8p,32b,32d6,32g6,32g6,32g6"},
    {"Urgent", "Urgent:d=8,o=6,b=500:c,e,d7,c,e,a#,c,e,a,c,e,g,c,e,a,c,e,a#,c,e,d7"},
    {"Telephone 1", "Telephone1:d=4,o=5,b=100:8e#6,8p,8a#6,8p,8g#6,8a#6,8g#6,8p,8a#6,8p,8a#6,8p,8g#6,p"},
    {"Telephone 2", "Telephone2:d=4,o=5,b=100:8e#7,8p,8d#7,8p,8e#7,p,8p,8a#6,8p,8a#6,8p,8g#6,p"},
    {"GrandeValse", "GrandeValse:d=8,o=6,b=180:g,f,a5,b5,e,d,f5,g5,p,d,c,e5,g5,c"},
    {"Mosaic",
     "Mosaic:d=8,o=6,b=400:c,e,g,e,c,g,e,g,c,g,c,e,c,g,e,g,e,c,p,c5,e5,g5,e5,c5,g5,e5,g5,c5,g5,c5,e5,c5,g5,e5,g5,e5,c5"},
    {"Blinky", "Blinky:d=4,o=5,b=200:32b,32p,32b,8p,32g,32p,32g,8p,32d6,32p,32d6,8p,32d,32p,32d,2p,32b,32p,32b,8p,32g,32p,32g,8p,"
               "32d6,32p,32d6,8p,32d,32p,32d,2p,32b,32p,32b,8p,32g,32p,32g,8p,32d6,32p,32d6,8p,32d,32p,32d"},
    {"SoftKnock", "SoftKnock:d=32,o=5,b=100:e,4p,e,p,e,8p,e,4p,e,8p,e,4p"},
    {"Satellite", "Satellit:d=8,o=6,b=112:g,e,16f,16e,d,c,g,c7,c"},
    {"Scale", "Scale:d=4,o=5,b=125:32c,32d,32e,32f,32g,32a,32b,32c6,32b,32a,32g,32f,32e,32d,32c,32c,32d,32e,32f,32g,32a,32b,32c6,"
              "32b,32a,32g,32f,32e,32d,32c,32c,32d,32e,32f,32g,32a,32b,32c6,32b,32a,32g,32f,32e,32d,32c"},
    {"Fight", "ByuFight:d=4,o=5,b=200:a.,8g#,a.,8g#,a,f,d,2c"},
    {"Police", "Police:d=2,o=5,b=160:g,c6,g,c6,g,c6,g,c6,g,c6"},
    {"HurdyGurdy", "Hurdy-Gu:d=32,o=5,b=300:16a#.3,16p.,f,c#,16p.,a#,f#,16p.,f,c#,8p.,a#.3"},
    {"Superson", "Superson:d=8,o=6,b=112:b5,c,c,c#,f#5,4a5,4p,c,c,c,c#,f#5,4a5,4p,4c#,c#,c#,c,c#,c,c#,16c,p,4c#,c,c#,c"},
    {"HyperMusic", "HyperMus:d=4,o=6,b=140:8f,8a,8d7,8a,8a,8d7,8a,8a,8c7,8a,8a,8c7,8a,8g,8b5,8g,8g,8b5,8g,8g,8a_5,8d,8g,8g"},
    {"LaLa Land", "LaLaLand:d=16,o=5,b=125:e,p,f,p,e,p,f,p,e,p,f,p,4g,f,p,4g,f,p,4g,f,p,4g,f,p,4g,f,p,4g,f,p,4g,f,p,4f,4e"},
    {"Tubular Bells", "TubularB:d=4,o=6,b=200:8b,8e,8a,8e,8b,8e,8g,8a,8e,8c7,8e,8d7,8e,8b,8c7,8e,8b,8e,8a,8e,8b,8e,8g,8a,8e,8c7"},
    {"Tetris", "tetris:d=4,o=5,b=160:e6,8b,8c6,8d6,16e6,16d6,8c6,8b,a,8a,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,2a,8p,d6,8f6,a6,8g6,"
               "8f6,e6,8e6,8c6,e6,8d6,8c6,b,8b,8c6,d6,e6,c6,a,a"},
    {"Bass Beats", "BassBeat:d=4,o=6,b=140:8a5,8a,8c7,f5,8f,8g,g5,8g,8a,d5,8d,8f,8d,8a5,8a,8c7,f5,8f,8g,g5,8g,8a,d5,8d,8f,8d"},
    {"SOS", "Sos:d=16,o=6,b=160:g,p,g,p,g,4p,8g.,p,8g.,p,8g.,4p,g,p,g,p,g,1p,g,p,g,p,g,4p,8g.,p,8g.,p,8g.,4p,g,p,g,p,g"},
    {"Universe", "LastUniv:d=4,o=5,b=200:8a,8a,8a,8f6,8e6,8d6,8e6,8d6,8c6,8d6,8c6,8g,8e6,8g6,8e6,8e6,8e6,8d6,8e6,8f6,8f6,8f6"},
    {"Eye Of Tiger",
     "EyeOfTig:d=4,o=5,b=100:8a#6,16c#7,8a#6,16p,16g#6,16f#6,16a#6,8g#6,16f#6,16p,16g#6,16g#6,16g#6,16a#6,16g#6,8f#6"},
    {"Bugs Bunny",
     "BugsBunn:d=8,o=7,b=200:4a,g,2a,p,4e,d,2e,p,4a,g,2a,g,4e,d,4e,p,d,4b6,4d,g,4a,g,g,e,g,4f#,p,a,4b,a,g,4f#,4e,2a"},
    {"Twin Peaks",
     "TwinPeak:d=4,o=6,b=90:8c5,f5,2c7,8p,8c5,d5,2a,8p,8c5,f5,2c7,8p,8c5,d5,2a,8p,8c5,8f5,8c,8f,8g,8a,8g,8f,8c5,d5,2a"},
    {"Daft Funk", "DaFunk:d=16,o=5,b=200:2f6,8d#6,8f6,8g#6,2c6,8p,8a#,8c6,8d#6,2g#,8p,8g,8g#,8c6,2f,4g,4g#"},
    {"Highway", "HighwayT:d=8,o=5,b=180:32p,d#,d#,4d#,2p,c,c,4c#,2p,c,c,c#,c,c,c#,c,d#,4d#"},
    {"Intermission", "Intermis:d=4,o=6,b=180:d,d5,p,d5,2f5,d,d5,p,d5,2g5,d,d5,p,d5,2d_5,2c"},
};

const int numRingtones = sizeof(ringtone) / sizeof(Rtttl);
