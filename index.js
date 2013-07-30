// TODO: we should use https://github.com/TooTallNate/node-bindings for this
// see https://github.com/arunoda/node-usage as an example
var _rf24 = require("./build/Release/rf24");


var obj = new _rf24.Wrapper(10);
console.log( obj.plusOne() ); // 11
console.log( obj.plusOne() ); // 12
console.log( obj.plusOne() ); // 13
