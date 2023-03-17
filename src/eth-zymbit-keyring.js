const { HDKey } = require('ethereum-cryptography/hdkey');
const { keccak256 } = require('ethereum-cryptography/keccak');
const { bytesToHex } = require('ethereum-cryptography/utils');

const zkJS = require('../zkJS/build/Release/zkAppUtilsJS.node');

const BIP44_ETH_BASE_PATH = `m/44'/60'/0'/0`
const type = 'Zymbit Hardware Wallet'
const zk = new zkJS.zkObj()

class ZymbitKeyring {

    constructor(opts = {}) {
        this.type = type;
        this._wallets = [];
        this.deserialize(opts);
      }

}

ZymbitKeyring.type = type;
module.exports = ZymbitKeyring;