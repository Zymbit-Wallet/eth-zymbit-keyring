const zkJS = require('../zkJS/build/Release/zkAppUtilsJS.node');
const { ethers } = require("ethers");
const { bytesToHex } = require('ethereum-cryptography/utils');
const { BN, stripHexPrefix } = require('ethereumjs-util')
const { concatSig } = require('@metamask/eth-sig-util');

const BIP44_ETH_BASE_PATH = `m/44'/60'/0'/0`
const type = 'Zymbit Hardware Wallet'
const zk = new zkJS.zkObj()



class ZymbitKeyring {

  constructor(opts = {}) {
    this.type = type;
    this.base_path = BIP44_ETH_BASE_PATH
    this.deserialize(opts);
  }


  serialize() {
    return Promise.resolve({
      wallet_name: this.wallet_name,
      master_slot: this.master_slot
    })
  }

  deserialize(opts = {}) {
    if (!opts.wallet_name && (typeof (opts.master_slot) != 'number' || opts.master_slot < 16 || opts.master_slot > 512)) {
      throw new Error("Valid wallet name or master slot required")
    }
    if (opts.wallet_name) {
      try {
        this.master_slot = zk.getWalletKeySlotFromNodeAddr("m", opts.wallet_name)
        this.wallet_name = opts.wallet_name
      } catch {
        throw new Error("Invalid Wallet Name")
      }
    } else {
      try {
        const slotDetails = zk.getWalletNodeAddrFromKeySlot(opts.master_slot)
        if (slotDetails.node_address == 'm') {
          this.master_slot = opts.master_slot
          this.wallet_name = slotDetails.wallet_name
        } else {
          throw new Error
        }
      } catch (e) {
        throw new Error("Invalid Master Key Slot")
      }
    }
    this.base_slot = 0
    this.account_slots = []

    const slots = []
    const inputBuffer = zk.getAllocSlotsList(false)
    for (let i = 0; i < inputBuffer.length / 4; i++) {
      const slot = inputBuffer.readInt32LE(4 * i)
      if (slot > 15) slots.push(slot)
    }

    for (const slot of slots) {
      const slotDetails = zk.getWalletNodeAddrFromKeySlot(slot)
      if (slotDetails.wallet_name == this.wallet_name) {
        if (slotDetails.node_address == this.base_path) {
          this.base_slot = slot
        } else if (slotDetails.node_address.includes(this.base_path + "/")) {
          slotDetails.slot = slot
          delete slotDetails.wallet_name
          this.account_slots.push(slotDetails)
        }
      }
    }
    if (this.base_slot == 0) {
      let deepestPath = { node_address: "m", slot: this.master_slot }
      for (const slot of slots) {
        const slotDetails = zk.getWalletNodeAddrFromKeySlot(slot)
        if (slotDetails.wallet_name == this.wallet_name) {
          if (this.base_path.includes(slotDetails.node_address) && slotDetails.node_address.length > deepestPath.node_address.length) {
            slotDetails.slot = slot
            delete slotDetails.wallet_name
            deepestPath = slotDetails
          }
        }
      }
      this.base_slot = this._generateBasePathKey(deepestPath)
    }
    console.log(this)
    return Promise.resolve()
  }


  addAccounts(n = 1) {
    const newAccounts = []
    if (n < 1) return newAccounts

    let nextAccountIndex = this.account_slots.reduce((prev, curr) => {
      if (Number(prev < Number(curr.node_address.slice(this.base_path.length + 1)))) {
        return Number(curr.node_address.slice(this.base_path.length + 1)) + 1
      } else return prev + 1
    }, 0)

    for (let i = 0; i < n; i++) {
      const slot = zk.genWalletChildKey(this.base_slot, nextAccountIndex, false, false)
      const slotDetails = zk.getWalletNodeAddrFromKeySlot(slot.slot)
      slotDetails.slot = slot.slot
      delete slotDetails.wallet_name
      this.account_slots.push(slotDetails)
      const newAccountEthAddress = ethers.computeAddress('0x' + bytesToHex(zk.exportPubKey(slotDetails.slot, false)))
      newAccounts.push(newAccountEthAddress)
      nextAccountIndex++
    }

    return newAccounts
  }

  getAccounts() {
    return this.account_slots.map(obj => ethers.computeAddress('0x' + bytesToHex(zk.exportPubKey(obj.slot, false))))
  }

  signTransaction(address, transaction) {
    const signingSlot = (this.account_slots.find(obj => ethers.computeAddress('0x' + bytesToHex(zk.exportPubKey(obj.slot, false))).toLowerCase() === address.toLowerCase())).slot
    if (!signingSlot) throw new Error("Keyring does not contain this address")

    const txHash = transaction.hash(true)
    const { signature, recovery_id } = zk.genECDSASigFromDigest(txHash, signingSlot)

    const validSignature = this._generateValidECDSASignature(signature, recovery_id, transaction.getChainId())
    transaction.r = validSignature.r
    transaction.s = validSignature.s
    transaction.v = validSignature.v

    return transaction
  }

  signMessage(address, data) {
    const signingSlot = (this.account_slots.find(obj => ethers.computeAddress('0x' + bytesToHex(zk.exportPubKey(obj.slot, false))).toLowerCase() === address.toLowerCase())).slot
    if (!signingSlot) throw new Error("Keyring does not contain this address")

    const messageToSign = Buffer.from(stripHexPrefix(data), 'hex')
    const { signature, recovery_id } = zk.genECDSASigFromDigest(messageToSign, signingSlot)

    const validSignature = this._generateValidECDSASignature(signature, recovery_id)

    return concatSig(validSignature.v, validSignature.r, validSignature.s)

  }

  exportAccount(address) {
    throw new Error('Not supported on Zymbit devices')
  }

  _generateBasePathKey(deepestPath) {
    let slot = 0
    switch (deepestPath.node_address) {
      case `m`:
        slot = zk.genWalletChildKey(deepestPath.slot, 44, true, false)
        break;
      case `m/44'`:
        slot = zk.genWalletChildKey(deepestPath.slot, 60, true, false)
        break;
      case `m/44'/60'`:
        slot = zk.genWalletChildKey(deepestPath.slot, 0, true, false)
        break;
      case `m/44'/60'/0'`:
        slot = zk.genWalletChildKey(deepestPath.slot, 0, false, false)
        break;
      case this.base_path:
        return deepestPath.slot
    }
    const slotDetails = zk.getWalletNodeAddrFromKeySlot(slot.slot)
    slotDetails.slot = slot.slot
    return this._generateBasePathKey(slotDetails)
  }

  _generateValidECDSASignature(signature, recovery_id, chain_Id = false) {
    const N = new BN('115792089237316195423570985008687907852837564279074904382605163141518161494337', 10)
    let s = new BN(signature.slice(32, 64).toString('hex'), 16)
    let y_parity = Boolean(recovery_id)
    if ((s.muln(2)).gte(N)) {
      y_parity = !y_parity
      s = N.sub(s)
    }

    const validSignature = {}
    validSignature.r = signature.slice(0, 32)
    validSignature.s = s.toBuffer()
    if (chain_Id) {
      validSignature.v = 35 + (chain_Id * 2) + (y_parity ? 1 : 0)
    } else {
      validSignature.v = 27 + (y_parity ? 1 : 0)
    }

    return validSignature
  }

}

ZymbitKeyring.type = type;
module.exports = ZymbitKeyring;