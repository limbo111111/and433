# rtl_433

rtl_433 (despite the name) is a generic data receiver, mainly for the 433.92 MHz, 868 MHz (SRD), 315 MHz, 345 MHz, and 915 MHz ISM bands.

The official source code is in the https://github.com/merbanan/rtl_433/ repository.
For more documentation and related projects see the https://triq.org/ site.

It works with [RTL-SDR](https://github.com/osmocom/rtl-sdr/) and/or [SoapySDR](https://github.com/pothosware/SoapySDR/).
Actively tested and supported are Realtek RTL2832 based DVB dongles (using RTL-SDR) and LimeSDR ([LimeSDR USB](https://www.crowdsupply.com/lime-micro/limesdr) and [LimeSDR mini](https://www.crowdsupply.com/lime-micro/limesdr-mini) engineering samples kindly provided by [MyriadRf](https://myriadrf.org/)), PlutoSDR, HackRF One (using SoapySDR drivers), as well as SoapyRemote.

![rtl_433 screenshot](./docs/screenshot.png)

## Building / Installation

rtl_433 is written in portable C (C99 standard) and known to compile on Linux (also embedded), MacOS, and Windows systems.
Older compilers and toolchains are supported as a key-goal.
Low resource consumption and very few dependencies allow rtl_433 to run on embedded hardware like (repurposed) routers.
Systems with 32-bit i686 and 64-bit x86-64 as well as (embedded) ARM, like the Raspberry Pi and PlutoSDR are well supported.

See [BUILDING.md](docs/BUILDING.md)

On Debian (sid) or Ubuntu (19.10+), `apt-get install rtl-433` for other distros check https://repology.org/project/rtl-433/versions

On FreeBSD, `pkg install rtl-433`.

On MacOS, `brew install rtl_433`.

Docker images with rtl_433 are available [on the github page of hertzg](https://github.com/hertzg/rtl_433_docker).

## How to add support for unsupported sensors

See [CONTRIBUTING.md](./docs/CONTRIBUTING.md).

## Running

    rtl_433 -h

```

  A "rtl_433.conf" file is searched in "./", XDG_CONFIG_HOME e.g. "$HOME/.config/rtl_433/",
  SYSCONFDIR e.g. "/usr/local/etc/rtl_433/", then command line args will be parsed in order.
		= General options =
  [-V] Output the version string and exit
  [-v] Increase verbosity (can be used multiple times).
       -v : verbose notice, -vv : verbose info, -vvv : debug, -vvvv : trace.
  [-c <path>] Read config options from a file
		= Tuner options =
  [-d <RTL-SDR USB device index> | :<RTL-SDR USB device serial> | <SoapySDR device query> | rtl_tcp | help]
  [-g <gain> | help] (default: auto)
  [-t <settings>] apply a list of keyword=value settings to the SDR device
       e.g. for SoapySDR -t "antenna=A,bandwidth=4.5M,rfnotch_ctrl=false"
       for RTL-SDR use "direct_samp[=1]", "offset_tune[=1]", "digital_agc[=1]", "biastee[=1]"
  [-f <frequency>] Receive frequency(s) (default: 433920000 Hz)
  [-H <seconds>] Hop interval for polling of multiple frequencies (default: 600 seconds)
  [-p <ppm_error>] Correct rtl-sdr tuner frequency offset error (default: 0)
  [-s <sample rate>] Set sample rate (default: 250000 Hz)
  [-D quit | restart | pause | manual] Input device run mode options (default: quit).
		= Demodulator options =
  [-R <device> | help] Enable only the specified device decoding protocol (can be used multiple times)
       Specify a negative number to disable a device decoding protocol (can be used multiple times)
  [-X <spec> | help] Add a general purpose decoder (prepend -R 0 to disable all decoders)
  [-Y auto | classic | minmax] FSK pulse detector mode.
  [-Y level=<dB level>] Manual detection level used to determine pulses (-1.0 to -30.0) (0=auto).
  [-Y minlevel=<dB level>] Manual minimum detection level used to determine pulses (-1.0 to -99.0).
  [-Y minsnr=<dB level>] Minimum SNR to determine pulses (1.0 to 99.0).
  [-Y autolevel] Set minlevel automatically based on average estimated noise.
  [-Y squelch] Skip frames below estimated noise level to reduce cpu load.
  [-Y ampest | magest] Choose amplitude or magnitude level estimator.
		= Analyze/Debug options =
  [-A] Pulse Analyzer. Enable pulse analysis and decode attempt.
       Disable all decoders with -R 0 if you want analyzer output only.
  [-y <code>] Verify decoding of demodulated test data (e.g. "{25}fb2dd58") with enabled devices
		= File I/O options =
  [-S none | all | unknown | known] Signal auto save. Creates one file per signal.
       Note: Saves raw I/Q samples (uint8 pcm, 2 channel). Preferred mode for generating test files.
  [-r <filename> | help] Read data from input file instead of a receiver
  [-w <filename> | help] Save data stream to output file (a '-' dumps samples to stdout)
  [-W <filename> | help] Save data stream to output file, overwrite existing file
		= Data output options =
  [-F log | kv | json | csv | mqtt | influx | syslog | trigger | rtl_tcp | http | null | help] Produce decoded output in given format.
       Append output to file with :<filename> (e.g. -F csv:log.csv), defaults to stdout.
       Specify host/port for syslog with e.g. -F syslog:127.0.0.1:1514
  [-M time[:<options>] | protocol | level | noise[:<secs>] | stats | bits | help] Add various meta data to each output.
  [-K FILE | PATH | <tag> | <key>=<tag>] Add an expanded token or fixed tag to every output line.
  [-C native | si | customary] Convert units in decoded output.
  [-n <value>] Specify number of samples to take (each sample is an I/Q pair)
  [-T <seconds>] Specify number of seconds to run, also 12:34 or 1h23m45s
  [-E hop | quit] Hop/Quit after outputting successful event(s)
  [-h] Output this usage help and exit
       Use -d, -g, -R, -X, -F, -M, -r, -w, or -W without argument for more help



		= Supported device protocols =
    [01]  Subaru (Flipper-ARF)
    [02]  Hyundai/Kia RIO (Flipper-ARF)
    [03]  Mazda Siemens (Flipper-ARF)
    [04]  VAG pre-2004 (Flipper-ARF)
    [05]  Hyundai Santa Fe 13-16 (Flipper-ARF)
    [06]  Silvercrest Remote Control
    [07]  Rubicson, TFA 30.3197 or InFactory PT-310 Temperature Sensor
    [08]  Prologue, FreeTec NC-7104, NC-7159-675 temperature sensor
    [09]  Waveman Switch Transmitter
    [11]* ELV EM 1000
    [12]* ELV WS 2000
    [13]  LaCrosse TX Temperature / Humidity Sensor
    [15]  Acurite 896 Rain Gauge
    [16]  Acurite 609TXC Temperature and Humidity Sensor
    [17]  Oregon Scientific Weather Sensor
    [18]* Mebus 433
    [19]* Intertechno 433
    [20]  KlikAanKlikUit Wireless Switch
    [21]  AlectoV1 Weather Sensor (Alecto WS3500 WS4500 Ventus W155/W044 Oregon)
    [22]  Cardin S466-TX2
    [23]  Fine Offset Electronics, WH2, WH5, Telldus Temperature/Humidity/Rain Sensor
    [24]  Nexus, FreeTec NC-7345, NX-3980, Solight TE82S, TFA 30.3209 temperature/humidity sensor
    [25]  Ambient Weather F007TH, TFA 30.3208.02, SwitchDocLabs F016TH temperature sensor
    [26]  Calibeur RF-104 Sensor
    [27]  X10 RF
    [28]  DSC Security Contact
    [29]* Brennenstuhl RCS 2044
    [30]  Globaltronics GT-WT-02 Sensor
    [31]  Danfoss CFR Thermostat
    [34]  Chuango Security Technology
    [35]  Generic Remote SC226x EV1527
    [36]  TFA-Twin-Plus-30.3049, Conrad KW9010, Ea2 BL999
    [37]  Fine Offset Electronics WH1080/WH3080 Weather Station
    [38]  WT450, WT260H, WT405H
    [39]  LaCrosse WS-2310 / WS-3600 Weather Station
    [40]  Esperanza EWS
    [41]  Efergy e2 classic
    [42]* Inovalley kw9015b, TFA Dostmann 30.3161 (Rain and temperature sensor)
    [43]  Generic temperature sensor 1
    [44]  WG-PB12V1 Temperature Sensor
    [45]  Acurite 592TXR temp/humidity, 592TX temp, 5n1, 3n1, Atlas weather station, 515 fridge/freezer, 6045 lightning, 899 rain, 1190/1192 leak
    [46]  Acurite 986 Refrigerator / Freezer Thermometer
    [47]  HIDEKI TS04 Temperature, Humidity, Wind and Rain Sensor
    [48]  Watchman Sonic / Apollo Ultrasonic / Beckett Rocket oil tank monitor
    [49]  CurrentCost Current Sensor
    [50]  emonTx OpenEnergyMonitor
    [51]  HT680 Remote control
    [52]  Conrad S3318P, FreeTec NC-5849-913 temperature humidity sensor, ORIA WA50 ST389 temperature sensor
    [53]* Akhan 100F14 remote keyless entry
    [54]  Quhwa
    [55]  OSv1 Temperature Sensor
    [56]  Proove / Nexa / KlikAanKlikUit Wireless Switch
    [57]  Bresser Thermo-/Hygro-Sensor 3CH
    [58]  Springfield Temperature and Soil Moisture
    [59]  Oregon Scientific SL109H Remote Thermal Hygro Sensor
    [60]  Acurite 606TX / Technoline TX960 Temperature Sensor
    [61]  TFA pool temperature sensor
    [62]  Kedsum Temperature & Humidity Sensor, Pearl NC-7415
    [63]  Blyss DC5-UK-WH
    [64]  Steelmate TPMS
    [65]  Schrader TPMS
    [66]* LightwaveRF
    [67]* Elro DB286A Doorbell
    [68]  Efergy Optical
    [69]* Honda Car Key
    [72]  Radiohead ASK
    [73]  Kerui PIR / Contact Sensor
    [74]  Fine Offset WH1050 Weather Station
    [75]  Honeywell Door/Window Sensor, 2Gig DW10/DW11, RE208 repeater
    [76]  Maverick ET-732/733 BBQ Sensor
    [77]* RF-tech
    [78]  LaCrosse TX141-Bv2, TX141TH-Bv2, TX141-Bv3, TX141W, TX145wsdth, (TFA, ORIA) sensor
    [79]  Acurite 00275rm,00276rm Temp/Humidity with optional probe
    [80]  LaCrosse TX35DTH-IT, TFA Dostmann 30.3155 Temperature/Humidity sensor
    [81]  LaCrosse TX29IT, TFA Dostmann 30.3159.IT Temperature sensor
    [82]  Vaillant calorMatic VRT340f Central Heating Control
    [83]  Fine Offset Electronics, WH25, WH32, WH32B, WN32B, WH24, WH65B, HP1000, Misol WS2320 Temperature/Humidity/Pressure Sensor
    [84]  Fine Offset Electronics, WH0530 Temperature/Rain Sensor
    [85]  IBIS beacon
    [86]  Oil Ultrasonic STANDARD FSK
    [87]  Citroen TPMS
    [88]  Oil Ultrasonic STANDARD ASK
    [89]  Thermopro TP11 Thermometer
    [90]  Solight TE44/TE66, EMOS E0107T, NX-6876-917
    [91]* Wireless Smoke and Heat Detector GS 558
    [92]  Generic wireless motion sensor
    [93]  Toyota TPMS
    [94]  Ford TPMS
    [95]  Renault TPMS
    [96]  inFactory, nor-tec, FreeTec NC-3982-913 temperature humidity sensor
    [97]  FT-004-B Temperature Sensor
    [98]  Ford Car Key
    [99]  Philips outdoor temperature sensor (type AJ3650)
    [100]  Schrader TPMS EG53MA4, Saab, Opel, Vauxhall, Chevrolet
    [101]  Nexa
    [102]  ThermoPro TP08/TP12/TP20 thermometer
    [103]  GE Color Effects
    [104]  X10 Security
    [105]  Interlogix GE UTC Security Devices
    [106]* Dish remote 6.3
    [107]  SimpliSafe Home Security System (May require disabling automatic gain for KeyPad decodes)
    [108]  Sensible Living Mini-Plant Moisture Sensor
    [109]  Wireless M-Bus, Mode C&T, 100kbps (-f 868.95M -s 1200k)
    [110]  Wireless M-Bus, Mode S, 32.768kbps (-f 868.3M -s 1000k)
    [111]* Wireless M-Bus, Mode R, 4.8kbps (-f 868.33M)
    [112]* Wireless M-Bus, Mode F, 2.4kbps
    [113]  Hyundai WS SENZOR Remote Temperature Sensor
    [114]  WT0124 Pool Thermometer
    [115]  PMV-107J (Toyota) TPMS
    [116]  Emos TTX201 Temperature Sensor
    [117]  Ambient Weather TX-8300 Temperature/Humidity Sensor
    [118]  Ambient Weather WH31E Thermo-Hygrometer Sensor, EcoWitt WH40 rain gauge, WS68 weather station
    [119]  Maverick ET73
    [120]  Honeywell ActivLink, Wireless Doorbell
    [121]  Honeywell ActivLink, Wireless Doorbell (FSK)
    [122]* ESA1000 / ESA2000 Energy Monitor
    [123]* Biltema rain gauge
    [124]  Bresser Weather Center 5-in-1
    [125]  Digitech XC-0324 / AmbientWeather FT005TH temp/hum sensor
    [126]  Opus/Imagintronix XT300 Soil Moisture
    [127]  FS20 / FHT
    [128]* Jansite TPMS Model TY02S
    [129]  LaCrosse/ELV/Conrad WS7000/WS2500 weather sensors
    [130]  TS-FT002 Wireless Ultrasonic Tank Liquid Level Meter With Temperature Sensor
    [131]  Companion WTR001 Temperature Sensor
    [132]  Ecowitt Wireless Outdoor Thermometer WH53/WH0280/WH0281A
    [133]  DirecTV RC66RX Remote Control
    [134]* Eurochron temperature and humidity sensor
    [135]  IKEA Sparsnas Energy Meter Monitor
    [136]  Microchip HCS200/HCS300 KeeLoq Hopping Encoder based remotes
    [137]  TFA Dostmann 30.3196 T/H outdoor sensor
    [138]  Rubicson 48659 Thermometer
    [139]  AOK Weather Station rebrand Holman Industries iWeather WS5029, Conrad AOK-5056, Optex 990018
    [140]  Philips outdoor temperature sensor (type AJ7010)
    [141]  ESIC EMT7110 power meter
    [142]  Globaltronics QUIGG GT-TMBBQ-05
    [143]  Globaltronics GT-WT-03 Sensor
    [144]  Norgo NGE101
    [145]  Elantra2012 TPMS
    [146]  Auriol HG02832, HG05124A-DCF, Rubicson 48957 temperature/humidity sensor
    [147]  Fine Offset Electronics/Ecowitt WH51, WN31, SwitchDoc Labs SM23 Soil Moisture Sensor
    [148]  Holman Industries iWeather WS5029 weather station (older PWM)
    [149]  TBH weather sensor
    [150]  WS2032 weather station
    [151]  Auriol AFW2A1 temperature/humidity sensor
    [152]  TFA Drop Rain Gauge 30.3233.01
    [153]  DSC Security Contact (WS4945)
    [154]  ERT Standard Consumption Message (SCM)
    [155]* Klimalogg
    [156]  Visonic powercode
    [157]  Eurochron EFTH-800 temperature and humidity sensor
    [158]  Cotech 36-7959, SwitchDocLabs FT020T wireless weather station with USB
    [159]  Standard Consumption Message Plus (SCMplus)
    [160]  Fine Offset Electronics WH1080/WH3080 Weather Station (FSK)
    [161]  Abarth 124 Spider TPMS
    [162]  Missil ML0757 weather station
    [163]  Sharp SPC775 weather station
    [164]  Insteon
    [165]  ERT Interval Data Message (IDM)
    [166]  ERT Interval Data Message (IDM) for Net Meters
    [167]* ThermoPro-TX2 temperature sensor
    [168]  Acurite 590TX Temperature with optional Humidity
    [169]  Security+ 2.0 (Keyfob)
    [170]  TFA Dostmann 30.3221.02 T/H Outdoor Sensor (also 30.3249.02)
    [171]  LaCrosse Technology View LTV-WSDTH01 Breeze Pro Wind Sensor
    [172]  Somfy RTS
    [173]  Schrader TPMS SMD3MA4 (Subaru) 3039 (Infiniti, Nissan, Renault)
    [174]* Nice Flor-s remote control for gates
    [175]  LaCrosse Technology View LTV-WR1 Multi Sensor
    [176]  LaCrosse Technology View LTV-TH Thermo/Hygro Sensor
    [177]  Bresser Weather Center 6-in-1, 7-in-1 indoor, soil, new 5-in-1, 3-in-1 wind gauge, Froggit WH6000, Ventus C8488A
    [178]  Bresser Weather Center 7-in-1, Air Quality PM2.5/PM10 7009970, CO2 7009977, HCHO/VOC 7009978 sensors
    [179]  EcoDHOME Smart Socket and MCEE Solar monitor
    [180]  LaCrosse Technology View LTV-R1, LTV-R3 Rainfall Gauge, LTV-W1/W2 Wind Sensor
    [181]  BlueLine Innovations Power Cost Monitor
    [182]  Burnhard BBQ thermometer
    [183]  Security+ (Keyfob)
    [184]  Cavius smoke, heat and water detector
    [185]  Jansite TPMS Model Solar
    [186]  Amazon Basics Meat Thermometer
    [187]  TFA Marbella Pool Thermometer
    [188]  Auriol AHFL temperature/humidity sensor
    [189]  Auriol AFT 77 B2 temperature sensor
    [190]  Honeywell CM921 Wireless Programmable Room Thermostat
    [191]  Hyundai TPMS (VDO)
    [192]  RojaFlex shutter and remote devices
    [193]  Marlec Solar iBoost+ sensors
    [194]  Somfy io-homecontrol
    [195]  Ambient Weather WH31L (FineOffset WH57) Lightning-Strike sensor
    [196]  Markisol, E-Motion, BOFU, Rollerhouse, BF-30x, BF-415 curtain remote
    [197]  Govee Water Leak Detector H5054, Door Contact Sensor B5023
    [198]  Clipsal CMR113 Cent-a-meter power meter
    [199]  Inkbird ITH-20R temperature humidity sensor
    [200]  RainPoint soil temperature and moisture sensor
    [201]  Atech-WS308 temperature sensor
    [202]  Acurite Grill/Meat Thermometer 01185M
    [203]* EnOcean ERP1
    [204]  Linear Megacode Garage/Gate Remotes
    [205]* Auriol 4-LD5661/4-LD5972/4-LD6313 temperature/rain sensors
    [206]  Unbranded SolarTPMS for trucks
    [207]  Funkbus / Instafunk (Berker, Gira, Jung)
    [208]  Porsche Boxster/Cayman TPMS
    [209]  Jasco/GE Choice Alert Security Devices
    [210]  Telldus weather station FT0385R sensors
    [211]  LaCrosse TX34-IT rain gauge
    [212]  SmartFire Proflame 2 remote control
    [213]  AVE TPMS
    [214]  SimpliSafe Gen 3 Home Security System
    [215]  Yale HSA (Home Security Alarm), YES-Alarmkit
    [216]  Regency Ceiling Fan Remote (-f 303.75M to 303.96M)
    [217]  Renault 0435R TPMS
    [218]  Fine Offset Electronics WS80 weather station
    [219]  EMOS E6016 weatherstation with DCF77
    [220]  Emax W6, rebrand Altronics x7063/4/x7064A, Optex 990040/50/51, Orium 13093/13123, Infactory FWS-1200, Newentor Q9, Otio 810025, Protmex PT3390A, Jula Marquant 014331/32, TechniSat IMETEO X6 76-4924-00, Weather Station or temperature/humidity sensor
    [221]* ANT and ANT+ devices
    [222]  EMOS E6016 rain gauge
    [223]  Microchip HCS200/HCS300 KeeLoq Hopping Encoder based remotes (FSK)
    [224]  Fine Offset Electronics WH45 air quality sensor
    [225]  Maverick XR-30 BBQ Sensor
    [226]  Fine Offset Electronics WN34S/L/D and Froggit DP150/D35 temperature sensor
    [227]  Rubicson Pool Thermometer 48942
    [228]  Badger ORION water meter, 100kbps (-f 916.45M -s 1200k)
    [229]  GEO minim+ energy monitor
    [230]  TyreGuard 400 TPMS
    [231]  Kia TPMS (-s 1000k)
    [232]  SRSmith Pool Light Remote Control SRS-2C-TX (-f 915M)
    [233]  Neptune R900 flow meters
    [234]  WEC-2103 temperature/humidity sensor
    [235]  Vauno EN8822C
    [236]  Govee Water Leak Detector H5054
    [237]  TFA Dostmann 14.1504.V2 Radio-controlled grill and meat thermometer
    [238]* CED7000 Shot Timer
    [239]  Watchman Sonic Advanced / Plus, Tekelek
    [240]  Oil Ultrasonic SMART FSK
    [241]  Gasmate BA1008 meat thermometer
    [242]  Flowis flow meters
    [243]  Wireless M-Bus, Mode T, 32.768kbps (-f 868.3M -s 1000k)
    [244]  Revolt NC-5642 Energy Meter
    [245]  LaCrosse TX31U-IT, The Weather Channel WS-1910TWC-IT
    [246]  EezTire E618, Carchet TPMS, TST-507 TPMS
    [247]* Baldr / RainPoint rain gauge.
    [248]  Celsia CZC1 Thermostat
    [249]  Fine Offset Electronics WS90 weather station
    [250]* ThermoPro TX-2C Thermometer and Humidity sensor
    [251]  TFA 30.3151 Weather Station
    [252]  Bresser water leakage
    [253]* Nissan TPMS
    [254]  Bresser lightning
    [255]  Schou 72543 Day Rain Gauge, Motonet MTX Rain, MarQuant Rain Gauge, TFA Dostmann 30.3252.01/47.3006.01 Rain Gauge and Thermometer, ADE WS1907
    [256]  Fine Offset / Ecowitt WH55 water leak sensor
    [257]  BMW Gen4-Gen5 TPMS and Audi TPMS Pressure Alert, multi-brand HUF/Beru, Continental, Schrader/Sensata, Audi
    [258]  Watts WFHT-RF Thermostat
    [259]  Thermor DG950 weather station
    [260]  Mueller Hot Rod water meter
    [261]  ThermoPro TP28b Super Long Range Wireless Meat Thermometer for Smoker BBQ Grill
    [262]  BMW Gen2 and Gen3 TPMS
    [263]  Chamberlain CWPIRC PIR Sensor
    [264]  ThermoPro Meat Thermometers, TP829B 4 probes with temp only
    [265]* Arad/Master Meter Dialog3G water utility meter
    [266]  Geevon TX16-3 outdoor sensor
    [267]  Fine Offset Electronics WH46 air quality sensor
    [268]  Vevor Wireless Weather Station 7-in-1
    [269]  Arexx Multilogger IP-HA90, IP-TH78EXT, TSN-70E
    [270]  Rosstech Digital Control Unit DCU-706/Sundance/Jacuzzi
    [271]  Risco 2 Way Agility protocol, Risco PIR/PET Sensor RWX95P
    [272]  ThermoPro Meat Thermometers, TP828B 2 probes with Temp, BBQ Target LO and HI
    [273]  Bresser Thermo-/Hygro-Sensor Explore Scientific ST1005H
    [274]  DeltaDore X3D devices
    [275]* Quinetic
    [276]  Landis & Gyr Gridstream Power Meters 9.6k
    [277]  Landis & Gyr Gridstream Power Meters 19.2k
    [278]  Landis & Gyr Gridstream Power Meters 38.4k
    [279]  Revolt ZX-7717 power meter
    [280]  GM-Aftermarket TPMS
    [281]  RainPoint HCS012ARF Rain Gauge sensor
    [282]  Apator Metra E-RM 30 water meter
    [283]  ThermoPro TX-7B Outdoor Thermometer Hygrometer
    [284]  Nexus, CRX, Prego sauna temperature sensor
    [285]  Homelead HG9901 (Geevon, Dr.Meter, Royal Gardineer) soil moisture/temp/light level sensor
    [286]  Maverick XR-50 BBQ Sensor
    [287]  Orion Endpoint from Badger Meter, GIF2014W-OSE, water meter, hopping from 904.4 Mhz to 924.6Mhz (-s 1600k)
    [288]  Fine Offset Electronics WH43 air quality sensor
    [289]  Baldr E0666TH Thermo-Hygrometer
    [290]  bm5-v2 12V Battery Monitor
    [291]  Universal (Reverseable) 24V Fan Controller
    [292]  Fine Offset Electronics WS85 weather station
    [293]  Oria WA150KM freezer and fridge thermometer
    [294]  Voltcraft EnergyCount 3000 (ec3k)
    [295]  Orion Endpoint from Badger Meter, GIF2020OCECNA, water meter, hopping from 904.4 Mhz to 924.6Mhz (-s 1600k)
    [296]  Geevon TX19-1 outdoor sensor
    [297]  WallarGe CLTX001 Outdoor Temperature Sensor
    [298]  Sainlogic SA8, Gevanti SA8 Weather Station
    [299]  ThermoPro TP862b TempSpike XR Wireless Dual-Probe Meat Thermometer
    [300]  Airpuxem TPMS TYH11_EU6_ZQ
    [301]  Apator Metra E-ITN 30 heat cost allocator
    [302]  ThermoPro TP211B Thermometer
    [303]  TRW TPMS OOK OEM and Clone models
    [304]  TRW TPMS FSK OEM and Clone models
    [305]  Govee Water Leak Detector H5059

* Disabled by default, use -R n or a conf file to enable


		= Input device selection =
	RTL-SDR device driver is available.
  [-d <RTL-SDR USB device index>] (default: 0)
  [-d :<RTL-SDR USB device serial (can be set with rtl_eeprom -s)>]
	To set gain for RTL-SDR use -g <gain> to set an overall gain in dB.
	SoapySDR device driver is not available.
  [-d ""] Open default SoapySDR device
  [-d driver=rtlsdr] Open e.g. specific SoapySDR device
	To set gain for SoapySDR use -g ELEM=val,ELEM=val,... e.g. -g LNA=20,TIA=8,PGA=2 (for LimeSDR).
  [-d rtl_tcp[:[//]host[:port]] (default: localhost:1234)
	Specify host/port to connect to with e.g. -d rtl_tcp:127.0.0.1:1234


		= Gain option =
  [-g <gain>] (default: auto)
	For RTL-SDR: gain in dB ("0" is auto).
	For SoapySDR: gain in dB for automatic distribution ("" is auto), or string of gain elements.
	E.g. "LNA=20,TIA=8,PGA=2" for LimeSDR.


		= Flex decoder spec =
Use -X <spec> to add a flexible general purpose decoder.

<spec> is "key=value[,key=value...]"
Common keys are:
	name=<name> (or: n=<name>)
	modulation=<modulation> (or: m=<modulation>)
	short=<short> (or: s=<short>)
	long=<long> (or: l=<long>)
	sync=<sync> (or: y=<sync>)
	reset=<reset> (or: r=<reset>)
	gap=<gap> (or: g=<gap>)
	tolerance=<tolerance> (or: t=<tolerance>)
	priority=<n> : run decoder only as fallback
where:
<name> can be any descriptive name tag you need in the output
<modulation> is one of:
	OOK_MC_ZEROBIT :  Manchester Code with fixed leading zero bit
	OOK_PCM :         Non Return to Zero coding (Pulse Code)
	OOK_RZ :          Return to Zero coding (Pulse Code)
	OOK_PPM :         Pulse Position Modulation
	OOK_PWM :         Pulse Width Modulation
	OOK_DMC :         Differential Manchester Code
	OOK_PIWM_RAW :    Raw Pulse Interval and Width Modulation
	OOK_PIWM_DC :     Differential Pulse Interval and Width Modulation
	OOK_MC_OSV1 :     Manchester Code for OSv1 devices
	FSK_PCM :         FSK Pulse Code Modulation
	FSK_PWM :         FSK Pulse Width Modulation
	FSK_MC_ZEROBIT :  Manchester Code with fixed leading zero bit
<short>, <long>, <sync> are nominal modulation timings in us,
<reset>, <gap>, <tolerance> are maximum modulation timings in us:
PCM/RZ  short: Nominal width of pulse [us]
         long: Nominal width of bit period [us]
PPM     short: Nominal width of '0' gap [us]
         long: Nominal width of '1' gap [us]
PWM     short: Nominal width of '1' pulse [us]
         long: Nominal width of '0' pulse [us]
         sync: Nominal width of sync pulse [us] (optional)
common    gap: Maximum gap size before new row of bits [us]
        reset: Maximum gap size before End Of Message [us]
    tolerance: Maximum pulse deviation [us] (optional).
Available options are:
	bits=<n> : only match if at least one row has <n> bits
	rows=<n> : only match if there are <n> rows
	repeats=<n> : only match if some row is repeated <n> times
		use opt>=n to match at least <n> and opt<=n to match at most <n>
	invert : invert all bits
	reflect : reflect each byte (MSB first to MSB last)
	decode_uart : UART 8n1 (10-to-8) decode
	decode_dm : Differential Manchester decode
	decode_mc : Manchester decode
	match=<bits> : only match if the <bits> are found
	preamble=<bits> : match and align at the <bits> preamble
		<bits> is a row spec of {<bit count>}<bits as hex number>
	unique : suppress duplicate row output

	countonly : suppress detailed row output

E.g. -X "n=doorbell,m=OOK_PWM,s=400,l=800,r=7000,g=1000,match={24}0xa9878c,repeats>=3"



		= Output format option =
  [-F log|kv|json|csv|mqtt|influx|syslog|trigger|rtl_tcp|http|null] Produce decoded output in given format.
	Without this option the default is LOG and KV output. Use "-F null" to remove the default.
	Append output to file with :<filename> (e.g. -F csv:log.csv), defaults to stdout.
  [-F mqtt[s][:[//]host[:port][,<options>]] (default: localhost:1883)
	Specify MQTT server with e.g. -F mqtt://localhost:1883
	Default user and password are read from MQTT_USERNAME and MQTT_PASSWORD env vars.
	Add MQTT options with e.g. -F "mqtt://host:1883,opt=arg"
	MQTT options are: user=foo, pass=bar, retain[=0|1], <format>[=topic]
	Supported MQTT formats: (default is all)
	  availability: posts availability (online/offline)
	  events: posts JSON event data, default "<base>/events"
	  states: posts JSON state data, default "<base>/states"
	  devices: posts device and sensor info in nested topics,
	           default "<base>/devices[/type][/model][/subtype][/channel][/id]"
	A base topic can be set with base=<topic>, default is "rtl_433/HOSTNAME".
	Any topic string overrides the base topic and will expand keys like [/model]
	E.g. -F "mqtt://localhost:1883,user=USERNAME,pass=PASSWORD,retain=0,devices=rtl_433[/id]"
	For TLS use e.g. -F "mqtts://host,tls_cert=<path>,tls_key=<path>,tls_ca_cert=<path>"
	With MQTT each rtl_433 instance needs a distinct driver selection. The MQTT Client-ID is computed from the driver string.
	If you use multiple RTL-SDR, perhaps set a serial and select by that (helps not to get the wrong antenna).
  [-F influx[:[//]host[:port][/<path and options>]]
	Specify InfluxDB 2.0 server with e.g. -F "influx://localhost:9999/api/v2/write?org=<org>&bucket=<bucket>,token=<authtoken>"
	Specify InfluxDB 1.x server with e.g. -F "influx://localhost:8086/write?db=<db>&p=<password>&u=<user>"
	  Additional parameter -M time:unix:usec:utc for correct timestamps in InfluxDB recommended
  [-F syslog[:[//]host[:port] (default: localhost:514)
	Specify host/port for syslog with e.g. -F syslog:127.0.0.1:1514
  [-F trigger:/path/to/file]
	Add an output that writes a "1" to the path for each event, use with a e.g. a GPIO
  [-F rtl_tcp[:[//]bind[:port]] (default: localhost:1234)
	Add a rtl_tcp pass-through server
  [-F http[:[//]bind[:port]] (default: 0.0.0.0:8433)
	Add a HTTP API server, a UI is at e.g. http://localhost:8433/


		= Meta information option =
  [-M time[:<options>]|protocol|level|noise[:<secs>]|stats|bits] Add various metadata to every output line.
	Use "time" to add current date and time meta data (preset for live inputs).
	Use "time:rel" to add sample position meta data (preset for read-file and stdin).
	Use "time:unix" to show the seconds since unix epoch as time meta data. This is always UTC.
	Use "time:iso" to show the time with ISO-8601 format (YYYY-MM-DD"T"hh:mm:ss).
	Use "time:off" to remove time meta data.
	Use "time:usec" to add microseconds to date time meta data.
	Use "time:tz" to output time with timezone offset.
	Use "time:utc" to output time in UTC.
		(this may also be accomplished by invocation with TZ environment variable set).
		"usec" and "utc" can be combined with other options, eg. "time:iso:utc" or "time:unix:usec".
	Use "replay[:N]" to replay file inputs at (N-times) realtime.
	Use "protocol" / "noprotocol" to output the decoder protocol number meta data.
	Use "level" to add Modulation, Frequency, RSSI, SNR, and Noise meta data.
	Use "noise[:<secs>]" to report estimated noise level at intervals (default: 10 seconds).
	Use "stats[:[<level>][:<interval>]]" to report statistics (default: 600 seconds).
	  level 0: no report, 1: report successful devices, 2: report active devices, 3: report all
	Use "bits" to add bit representation to code outputs (for debug).


		= Read file option =
  [-r <filename>] Read data from input file instead of a receiver
	Parameters are detected from the full path, file name, and extension.

	A center frequency is detected as (fractional) number suffixed with 'M',
	'Hz', 'kHz', 'MHz', or 'GHz'.

	A sample rate is detected as (fractional) number suffixed with 'k',
	'sps', 'ksps', 'Msps', or 'Gsps'.

	File content and format are detected as parameters, possible options are:
	'cu8', 'cs16', 'cf32' ('IQ' implied), and 'am.s16'.

	Parameters must be separated by non-alphanumeric chars and are case-insensitive.
	Overrides can be prefixed, separated by colon (':')

	E.g. default detection by extension: path/filename.am.s16
	forced overrides: am:s16:path/filename.ext

	Reading from pipes also support format options.
	E.g reading complex 32-bit float: CU32:-


		= Write file option =
  [-w <filename>] Save data stream to output file (a '-' dumps samples to stdout)
  [-W <filename>] Save data stream to output file, overwrite existing file
	Parameters are detected from the full path, file name, and extension.

	File content and format are detected as parameters, possible options are:
	'cu8', 'cs8', 'cs16', 'cf32' ('IQ' implied),
	'am.s16', 'am.f32', 'fm.s16', 'fm.f32',
	'i.f32', 'q.f32', 'logic.u8', 'ook', and 'vcd'.

	Parameters must be separated by non-alphanumeric chars and are case-insensitive.
	Overrides can be prefixed, separated by colon (':')

	E.g. default detection by extension: path/filename.am.s16
	forced overrides: am:s16:path/filename.ext

```


Some examples:

| Command | Description
|---------|------------
| `rtl_433` | Default receive mode, use the first device found, listen at 433.92 MHz at 250k sample rate.
| `rtl_433 -C si` | Default receive mode, also convert units to metric system.
| `rtl_433 -f 868M -s 1024k` | Listen at 868 MHz and 1024k sample rate.
| `rtl_433 -M hires -M level` | Report microsecond accurate timestamps and add reception levels (depending on gain).
| `rtl_433 -R 1 -R 8 -R 43` | Enable only specific decoders for desired devices.
| `rtl_433 -A` | Enable pulse analyzer. Summarizes the timings of pulses, gaps, and periods. Can be used with `-R 0` to disable decoders.
| `rtl_433 -S all -T 120` | Save all detected signals (`g###_###M_###k.cu8`). Run for 2 minutes.
| `rtl_433 -K FILE -r file_name` | Read a saved data file instead of receiving live data. Tag output with filenames.
| `rtl_433 -F json -M utc \| mosquitto_pub -t home/rtl_433 -l` | Will pipe the output to network as JSON formatted MQTT messages. A test MQTT client can be found in `examples/mqtt_rtl_433_test_client.py`.
| `rtl_433 -f 433.53M -f 434.02M -H 15` | Will poll two frequencies with 15 seconds hop interval.

## Security

Please note: We aim to make `rtl_433` safe to use, but it should not be assumed secure.
There is no reason to e.g. run with `sudo`, we do read and write files without any checks.

The output is literally pulled from thin air, it's not to be trusted.
If you feed downstream systems with data make sure edge cases are checked and handled.
Network inputs and outputs are for use in a trusted local network, will contain unfiltered data, and might overload the recipient
(know that e.g. the MQTT output can be controlled by anyone with a radio sender).

## Google Group

Join the Google group, rtl_433, for more information about rtl_433:
https://groups.google.com/forum/#!forum/rtl_433


## Troubleshooting

If you see this error:

    Kernel driver is active, or device is claimed by second instance of librtlsdr.
    In the first case, please either detach or blacklist the kernel module
    (dvb_usb_rtl28xxu), or enable automatic detaching at compile time.

then

    sudo rmmod rtl2832_sdr dvb_usb_rtl28xxu rtl2832

or add

    blacklist dvb_usb_rtl28xxu

to /etc/modprobe.d/blacklist.conf

## Releases

Version numbering scheme used is year.month. We try to keep the API compatible between releases but focus is on maintainablity.
