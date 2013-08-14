// TODO: we should use https://github.com/TooTallNate/node-bindings for this
// see https://github.com/arunoda/node-usage as an example
var _rf24 = require("./build/Release/rf24");

exports.radio = function (ce, cs) {
    ce || (ce = 22);
    cs || (cs = 24);        // 8?
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