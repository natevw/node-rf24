// TODO: we should use https://github.com/TooTallNate/node-bindings for this
// see https://github.com/arunoda/node-usage as an example
var _rf24 = require("./build/Release/rf24");


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




var obj = new _rf24.Wrapper(10);
console.log( obj.plusOne() ); // 11
console.log( obj.plusOne() ); // 12
console.log( obj.plusOne() ); // 13

exports.radio = function (onready) {           // 'ready', 'error'
    var xcvr = {};
    
    // TODO: init+begin on RF24 instance
    
    // object (acknowlegement?) vs. data (fragmentation?) pipe issues
    
    // blah…just write straight async wrapper for RF24 and make something more nice when someone cares
    
    xcvr.createStream = function () {     // write address, read addresses, options (end on switch?)
    
    }
    
    
    return xcvr;
}

