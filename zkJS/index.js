// import native addon
const addonZK = require('./build/Release/zkAppUtilsJS.node');

// expose module API
const zko = new addonZK.zkObj();
console.log('Time: ', zko.getTime());
console.log('Pub_key: ', zko.exportPubKey(1, false));
