const keyring = require('./eth-zymbit-keyring')
const zkJS = require('../zkJS/build/Release/zkAppUtilsJS.node');
const { bytesToHex, bytesToUtf8 } = require('ethereum-cryptography/utils');
const { type } = require('./eth-zymbit-keyring');


const zk = new zkJS.zkObj()
opts = {
    wallet_name: "MyExampleWallet",
    master_slot: 16
}



// for (const value of inputBuffer.values()) {
//     console.log(value);
//   }
// const intArray = Array.from({ length: inputBuffer.length / 5 }).map((_, i) => inputBuffer.readUInt32BE(i * 5));


const zymbitKeyring = new keyring(opts)

console.log(zymbitKeyring.addAccounts(3))

console.log(zymbitKeyring)

console.log(zymbitKeyring.getAccounts())