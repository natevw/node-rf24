// TODO: we should use https://github.com/TooTallNate/node-bindings for this
// see https://github.com/arunoda/node-usage as an example
var _rf24 = require("./build/Release/rf24");

exports.radio = function (dev, speed, ce) {
    dev = "/dev/spidev0.0";
    speed = 8000000;
    ce = 25;
    // TODO: improve argument handling
    
    var xcvr = new _rf24.Wrapper(ce,cs),
        radio = {};
    
    radio.begin = function (cb) {
        cb || (cb = function () {});
        xcvr.begin(cb);
    };
    
    radio.listen = function (listen, cb) {
        if (typeof listen === 'function') {
            cb = listen;
            listen = true;
        }
        cb || (cb = function () {});
        xcvr.listen(listen, cb);
    };
    
    radio.write = function (data, cb) {
        if (typeof data === 'string') {
            data = new Buffer(data);
        }
        cb || (cb = function () {});
        xcvr.write(data, cb);
    };
    
    radio.available = xcvr.available;
    radio.read = xcvr.read;
    
    return radio;
}




/*
    ctor(s)
    begin
    reset
    startListening
    stopListening
    [start]write
    available[pipe]
    read
    openWritingPipe
    openReadingPipe
    writeAckPayload
    isAckPayloadAvailable
    
    setRetries
    setChannel
    get|setPayloadSize
    getDynamicPayloadSize
    enableAckPayload
    enableDynamicPayloads
    isPVariant
    setAutoAck/per-pipe
    get|setPALevel
    get|setDataRate
    get|setCRCLength
    disableCRC
    printDetails
    powerDown|Up
    whatHappened
    testCarrier/testRPD
*/