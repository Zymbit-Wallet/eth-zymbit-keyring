const keyring = require('./eth-zymbit-keyring')
const zkJS = require('../zkJS/build/Release/zkAppUtilsJS.node');
const { bytesToHex, bytesToUtf8 } = require('ethereum-cryptography/utils');
const { type } = require('./eth-zymbit-keyring');
const { ethers } = require("ethers");
const EthereumTx = require('ethereumjs-tx').Transaction
const {
  BN,
  bufferToInt,
  ecrecover,
  rlphash,
  publicToAddress,
  ecsign,
  toBuffer,
  rlp,
  stripZeros,
} =  require('ethereumjs-util')

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

let accounts = zymbitKeyring.getAccounts()

let fromAccount = accounts[0]

const txParams = {
  from: fromAccount,
  nonce: '0x00',
  gasPrice: '0x09184e72a000',
  gasLimit: '0x2710',
  to: accounts[2],
  value: '0x1000',
};
  
  // The second parameter is not necessary if these values are used
  let tx = new EthereumTx(txParams, { chain: 'mainnet'})

const message =
  '0x879a053d4800c6354e76c7985a865d2922c82fb5b3f4577b2fe08b998954f2e0';

tx = zymbitKeyring.signTransaction(fromAccount.toLowerCase(), tx)
console.log(tx.verifySignature())

console.log(zymbitKeyring.signMessage(fromAccount, message))

// console.log(zymbitKeyring.addAccounts(3))
